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

//    unset($form['donation-currency']);
//    unset($form['payment-fieldset']);
//    $form['submit-fieldset']['submit']['#value'] = t('Click to Donate');
    //$form['form_id']['#value'] = 'dw_campaigns_users_donate_co_page_form';
    unset($form['donation-comments']['picture-preview']);
    unset($form['donation-comments']['user-photo']);
    unset($form['donation-comments']['displayname']);
    unset($form['donation-comments']['contribution_comments']);
    $form['donation-comments']['#title'] = 'Donation Visibility';
    unset($form['submit-fieldset']);

    $form['payment-fieldset']['card_number']['#attribues']['data-stripe'] = 'number';

    $form['payment-fieldset']['card_number']['#attributes'] = array('data-stripe' => 'number');
    $form['payment-fieldset']['cvv']['#attributes'] = array('data-stripe' => 'cvv');
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
        Stripe.setPublishableKey("pk_test_enKr39D7BAZw6KQMoKpsJwcQ");

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

function dw_campaigns_users_donate_st_page_form_validate($form, &$form_state){

}

function dw_campaigns_users_donate_st_page_form_submit($form, &$form_state){
    //I'm trying to get my form to finally submit to here.
    $stripeToken = $form_state['values']['stripeToken'];

    if( empty($stripeToken) ){
        $errors['stripeToken'] = 'Stripe token was not set by the form, check your javascript!';
    }



    if( sizeof( $errors ) ){
        var_dump( $errors );exit;
    }

    var_dump('du eet!');exit;


}
