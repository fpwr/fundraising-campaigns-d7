<?php

function dw_campaigns_users_donate_st_page($username, $campaignPath, $account = null){
    $data = dw_campaigns_users_pcp_page($username, $campaignPath, true);
    extract($data);

    drupal_set_title($pcp->title);

    if (!is_null($account)) {
        echo theme('dw_campaigns_users_donate_st_redir_page_theme', array('data' => $account));
        $_SESSION['donate_page_redir'] = 'dw/users/' . arg(2) . '/' . arg(3) . '/join';
        die;
    } else {

        return theme(
            'dw_campaigns_users_donate_st_page_theme',
            array('thisUser' => $owner_user, 'campaign' => $campaign, 'pcp' => $pcp)
        );
    }

}


function dw_campaigns_users_donate_st_page_form($form, &$form_state, $campaign, $pcp, $include_donation_conf = TRUE) {
    $form = dw_campaigns_users_donate_page_form($form, $form_state, $campaign, $pcp, $include_donation_conf);

    unset($form['donation-comments']['picture-preview']);
    unset($form['donation-comments']['user-photo']);
    unset($form['donation-comments']['displayname']);
    unset($form['donation-comments']['contribution_comments']);
    $form['donation-comments']['#title'] = 'Donation Visibility';
    unset($form['submit-fieldset']);

    $form['payment-fieldset']['card_number']['#attribues']['data-stripe'] = 'number';

    $form['payment-fieldset']['card_number']['#attributes'] = array('data-stripe' => 'number');
    $form['payment-fieldset']['cvv']['#attributes'] = array('data-stripe' => 'cvc');
    $form['payment-fieldset']['exp-month']['#attributes'] = array('data-stripe' => 'exp-month');


    //stripe doesn't seem to like option selectors.. will have to work this out with some fancy hidden psuedo form stuff perhaps
    //$form['payment-fieldset']['exp-year']['#type'] = 'textfield';
    //$form['payment-fieldset']['exp-year']['#attributes'] = array('data-stripe' => 'exp-year', 'size' => '4');

    $form['cleanExpYear'] = array(
        '#type'             => 'hidden',
        '#attributes'        => array(
            'id'            => 'stripeFormattedExpYear',
            'data-stripe'   => 'exp-year'
        )
    );

    $form['stripeToken'] = array(
        '#type'             => 'hidden',
        '#attributes'       => array(
            'id'            => 'hiddenStripeTokenElement',
            'size'          => '4'
        )
    );

    $form['submit']      = array(
        '#type'             => 'submit',
        '#value'            => t('Submit'),
        '#attributes'       => array('class'=> array('btn'))
    );

    $values = isset($form_state['values']) ? $form_state['values'] : array();

    $countryId  = isset($values['country2']) ? $values['country2'] : 1228;
    $stateId    = isset($values['state2']) ? $values['state2'] : '';

    $stripe_public_api_key = variable_get('stripe_public_api_key', '');
    if(strlen($stripe_public_api_key) < 9) {
        drupal_set_message(t('Error - stripe processor setup not completed! Check drupal settings'), 'error');
    }


    drupal_add_js('https://js.stripe.com/v2/','external');
    drupal_add_js('
        var donation_form = undefined;

        function stripeResponseHandler( status, response ){
            if( response.error ){
                alert( response.error.message );

            }else{
                var token = response.id;

                //now that we have our token, throw it in the form, which will be submitted back to drupal for some nifty processing and stuff.
                var stripeTokenElement = document.getElementById("hiddenStripeTokenElement");
                    stripeTokenElement.value = token;

                // and submit
                donation_form.get(0).submit();
            }

        }

//@TODO: drupal way to set this from an admin form
        Stripe.setPublishableKey("'. $stripe_public_api_key .'");

        jQuery(function($) {
            $("#dw-campaigns-users-donate-st-page-form").submit(function(event) {
                donation_form = $(this);

                //@TODO display waiting dialog here and disable continued donation submissions

                //stripe doesn\'t like our drop down selector for year, so lets move the selected value into a field that it does like.
                var exp_year = document.getElementById("edit-exp-year");
                var exp_year_value = exp_year.value;
                var exp_year_text = exp_year[exp_year_value].innerHTML;

                //console.log(
                document.getElementById("stripeFormattedExpYear").value = exp_year_text;

                Stripe.card.createToken(donation_form, stripeResponseHandler);

                // Prevent the form from submitting with the default action
                return false;
            });
        });

    ','inline');

    return $form;
}

function dw_campaigns_users_donate_st_page_form_validate(&$form, &$form_state) {
    global $user;
    global $dw_campaign_module_path;

    $multi = FALSE;

    $thisUser        = dw_campaigns_get_selected_user();

    $donation_amount = preg_replace('/[^0-9\.]/', '', $form_state['values']['donation-amount']);
    $form_state['values']['donation-amount'] = $donation_amount;

    if($donation_amount <= 0) {
        form_set_error('donation-amount', t('Donation amount must be > 0.00'));
        return;
    }

    $stripeToken = $form_state['values']['stripeToken'];

    if( empty($stripeToken) ){
        form_set_error('', t('Stripe token was not set by the form, check your javascript!'));
        return;
    }

    $values = $form_state['values'];

    // we don't currently need this here
    $campaign           = node_load($values['campaign']);

    // $campaign is the drupal node id!!
    $contributionType   = dw_campaigns_create_contribution_type_for_campaign($campaign);
    $contributionPage   = dw_campaigns_get_contribution_page_for_campaign($campaign);


    $countries_longname = _dw_civicrm_pseudoconstant_country();
    // turn country 'id' 1228 (united states) into 'US'
    $countries          = _dw_civicrm_pseudoconstant_countryIsoCode();
    $countryID          = $values['country'];
    $country_iso_code   = isset($countries[$countryID])?$countries[$countryID]:'';

    $billing_countryID  = $values['billing_country'];
    $billing_country_iso_code   = isset($countries[$billing_countryID])?$countries[$billing_countryID]:'';

    $states             = _dw_civicrm_pseudoconstant_stateprovince();
    $state = isset($states[$values['state']])?$states[$values['state']]:'';

    if(empty($state))
        $state='CA';

    if($values['state'] == 'nostate') {
        $state = '';
    }

    $billing_state = isset($states[$values['billing_state']])?$states[$values['billing_state']]:'';


    /*
        if($values['include_in_honor_roll'] == 1 && empty($values['contribution_comments']) )
        {
            form_set_error('contribution_comment', t('You must include a comment if you want to be shown as a contributor'));
            return;
        }
    */

    if(isset($_SESSION['messages']['error']) && count($_SESSION['messages']['error']) > 0) {
        form_set_error('', 'Please fix errors and try again');
        return;
    }

    $remote_addr                                = $_SERVER['REMOTE_ADDR'];
    if($remote_addr == '::1') {
        $remote_addr                            = '127.0.0.1';
    }


    if($campaign->field_dw_currency['und']['0']['value']  == 'MULTI') {
        $multi = TRUE;
    }

    //$alt_billing = !empty($values['billing_first_name']) ? TRUE : FALSE;
    $alt_billing = empty($values['use_alt_billing']) ? TRUE : FALSE;

    $params['amount']                           = $values['donation-amount'];
    $params['amount_other']                     = $values['donation-amount'];
    $params['custom_1']                         = '';

// only save this as billing info if they didn't supply alternate information - Susan said not to save billing info if its on behalf of someone
    if(!$alt_billing) {
// -5 means 'billing'
        $params['email-5']                          = $values['email'];
        $params['billing_first_name']               = $values['first_name'];
        $params['billing_middle_name']              = '';
        $params['billing_last_name']                = $values['last_name'];
        $params['billing_street_address-5']         = $values['address_1'];
        $params['billing_city-5']                   = $values['city'];
        $params['billing_country_id-5']             = $country_iso_code; // $countries[$values['country']];
        $params['billing_state_province-5']         = $state;
        $params['billing_country-5']                = $country_iso_code;
    }

    //$params['credit_card_number']               = $values['card_number'];
    //$params['cvv2']                             = $values['cvv'];
    //$params['credit_card_exp_date']['M']        = $values['exp-month'];
    //$params['credit_card_exp_date']['Y']        = $values['exp-year']+date('Y');
    //$params['credit_card_type']                 = $values['card_type'];
    //$params['month']                            = $values['exp-month'];
    //$params['year']                             = $values['exp-year']+date('Y');
    $params['ip']                               = $remote_addr;
    $params['ip_address']                       = $remote_addr;
    $params['amount_level']                     = '';
    $params['currencyID']                       = $contributionPage->currency;
    $params['currency']                         = $contributionPage->currency;
    $params['payment_action']                   = 'Sale';
    $params['is_pay_later']                     = '';
    $params['is_recur']                         = '';
    $params['description']                      = 'Online Contribution: ' . $contributionType->name;
    $params['accountingCode']                   = '';
    $params['first_name']                       = $values['first_name'];
    $params['middle_name']                      = '';
    $params['last_name']                        = $values['last_name'];
    $params['email']                            = $values['email'];
    $params['street_address']                   = $values['address_1'];
    $params['city']                             = $values['city'];
    $params['state_province']                   = $state;
    $params['postal_code']                      = $values['postal_code'];
    $params['country']                          = $country_iso_code;
    $params['contribution_type_id']             = $contributionType->id;
    $params['contributionType_id']              = $contributionType->id;
    $params['contributionType_name']            = $contributionType->name;
    //$params['contributionType_accounting_code'] = $contributionType->accounting_code;
    $params['contributionPageID']               = $contributionPage->id;
    $params['subject']                          = '';

// save home address
    $params['email-1']                          = $values['email'];
    $params['street_address-1']                 = $values['address_1'];
    $params['city-1']                           = $values['city'];
    $params['state_province-1']                 = $state;
    $params['postal_code-1']                    = $values['postal_code'];
    $params['country-1']                        = $country_iso_code;

    $params['email-5']                          = $values['email'];
    $params['street_address-5']                 = $values['address_1'];
    $params['city-5']                           = $values['city'];
    $params['state_province-5']                 = $state;
    $params['postal_code-5']                    = $values['postal_code'];
    $params['country-5']                        = $country_iso_code;



    /*
        // this is what they use
        $fields=array(
            'email-Primary' => 1,
            'billing_first_name' => 1,
            'billing_middle_name' => 1,
            'billing_last_name' => 1,
            'billing_street_address-5' => 1,
            'billing_city-5' => 1,
            'billing_state_province_id-5' => 1,
            'billing_postal_code-5' => 1,
            'billing_country_id-5' => 1,
            'credit_card_number' => 1,
            'cvv2' => 1,
            'credit_card_exp_date' => 1,
            'credit_card_type' => 1,
            'first_name' => 1,
            'middle_name' => 1,
            'last_name' => 1,
            'email-5' => 1,
        );
    */

    /* since we are using the REST API we need to pass it the fields we want to dedupe on */
    foreach($params as $key => $value) {
        $dedupe['fields_' . $key] = $value;
    }

    $dedupe = array_merge($dedupe, $params);
    $contactID = _dw_campaigns_dwutils_get_deduped_contact_id($dedupe);

    // save this for the rest of the session
    $_SESSION['contact_id'] = $contactID;

    dw_campaigns_add_address_book($thisUser, $values['email'], $values['first_name'] . ' ' . $values['last_name']);

    $mode = $campaign->field_dw_processor_mode['und']['0']['value'];

    if($multi) {
        global $currency_processor;

        $ppID = $campaign->field_dw_processor['und']['0']['value'];
        $picked_currency = $values['donation-currency'];
        $params['currencyID']    = $picked_currency;

        if($ppID != 5) {
            if(!isset($currency_processor[$picked_currency])) {
                form_set_error('', t('Invalid Currency') . $picked_currency);
                return;
            }
            $ppID                    = $currency_processor[$picked_currency];
        }

    } else {
        $ppID = $campaign->field_dw_processor['und']['0']['value'];

    }

    $invoice_id                  = dw_campaigns_invoice_generate_id($params['currencyID']);

    $params['invoiceID']         = $invoice_id;

    // add in some fields we learn of now, so submit can use them
    $form_state['values']['contact_id'] = $contactID;
    $form_state['values']['invoice_id'] = $invoice_id;

    $override = variable_get('dw_campaign_offline_transaction_override', '1234588');
    // this will approve the transaction without actually processing it
    if($values['card_number'] == $override) {
        $form_state['values']['offline']       = 1;
        $form_state['values']['trxn_id']       = 'FAKECC-' . str_replace(".", "-", microtime(TRUE));;
        return;
    }

    if(is_null($mode) || is_null($ppID)) {
        form_set_error('', "Please contact support, unknown billing processor");
        return;
    }

    $paymentParams          = array();
    $paymentParams['mode']  = $mode;
    $paymentParams['id']    = $ppID;


    $paymentProcessor       = _dw_campaigns_paymentprocessor_getpayment($paymentParams);


/*
//$mode = 'test';
    if($mode === 'test') {

        //Visa   4155100421445993
        //Exp Date:  7/2015

        $paymentProcessor->url_api      = 'https://api.sandbox.paypal.com/';
        $paymentProcessor->user_name    = 'ryansm_1280248291_biz_api1.jemeznet.com';
        $paymentProcessor->password     = '1280248301';
        $paymentProcessor->signature    = 'A9KqCy2x6U0txLZNrROYvr3t-N5QAh0OlVXIQQRviR7B53jI2ipzfQDT';
    }
*/
    $paymentProcessor->subject      = '';


    if($mode === 'fake') {
        $form_state['values']['trxn_id'] = 'OFFLINE-' . str_replace(".", "-", microtime(TRUE));
        return;
    }



    $paymentParams                          = array();
    $paymentParams['paymentProcessor']      = $paymentProcessor;
    $paymentParams['mode']                  = $mode;
    $paymentParams['component']             = 'Contribute';
//  $paymentParams                        = array_merge($params, $paymentParams);

    $paymentParams['is_recur']              = '0'; // civi generates notices if this isn't set

    $paymentParams['payment_action']        = $params['payment_action'];
    $paymentParams['amount']                = $params['amount'];
    $paymentParams['currencyID']            = $params['currencyID'];
    $paymentParams['invoiceID']             = $params['invoiceID'];
    $paymentParams['ip_address']            = $params['ip_address'];
//    $paymentParams['credit_card_type']      = $params['credit_card_type'];
//    $paymentParams['credit_card_number']    = $params['credit_card_number'];
//    $paymentParams['month']                 = $params['month'];
//    $paymentParams['year']                  = $params['year'];
//    $paymentParams['cvv2']                  = $params['cvv2'];
    $paymentParams['description']           = $params['description'];

    $paymentParams['stripe_token']          = $form_state['values']['stripeToken'];

    if(!$alt_billing) {
        $paymentParams['first_name']            = $params['first_name'];
        $paymentParams['last_name']             = $params['last_name'];
        $paymentParams['billing_first_name']    = $params['first_name'];
        $paymentParams['billing_last_name']     = $params['last_name'];
        $paymentParams['email']                 = $params['email'];
        $paymentParams['street_address']        = $params['street_address'];
        $paymentParams['city']                  = $params['city'];
        $paymentParams['state_province']        = $state;
        $paymentParams['country']               = $country_iso_code;
        $paymentParams['postal_code']           = $params['postal_code'];
    } else {
        $paymentParams['first_name']            = $values['billing_first_name'];
        $paymentParams['last_name']             = $values['billing_last_name'];
        $paymentParams['billing_first_name']    = $values['billing_first_name'];
        $paymentParams['billing_last_name']     = $values['billing_last_name'];
        $paymentParams['email']                 = $values['billing_email'];
        $paymentParams['street_address']        = $values['billing_address_1'];
        $paymentParams['city']                  = $values['billing_city'];
        $paymentParams['state_province']        = $billing_state;
        $paymentParams['country']               = $billing_country_iso_code;
        $paymentParams['postal_code']           = $values['billing_postal_code'];
    }

    $result = _dw_campaigns_payment_dodirectpayment($paymentParams);


    if((is_object($result)) && isset($result->trxn_id))
    {
        $form_state['values']['trxn_id'] = $result->trxn_id;
        return;
    }
    else
    {
        watchdog('Stripe Failure', serialize($result) );

    }

    form_set_error('', "Unknown error, please contact support with code '33-9S'");

}

function dw_campaigns_users_donate_st_page_form_submit($form, &$form_state){

    return dw_campaigns_users_donate_page_form_submit( $form, $form_state );

    //I'm trying to get my form to finally submit to here.
    $stripeToken = $form_state['values']['stripeToken'];

    if( empty($stripeToken) ){
        $errors['stripeToken'] = 'Stripe token was not set by the form, check your javascript!';
    }

    if( sizeof( $errors ) ){
        var_dump( $errors );exit;
    }


    global $user;
    global $dw_campaign_module_path;

    $mode_type = dw_campaigns_get_selected_type();






}
