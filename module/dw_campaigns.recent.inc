<?php

function dw_campaigns_get_group_names($includeUnassigned = TRUE) {

    $rows           = array();

    if($includeUnassigned) {
        $rows[0]    = 'Unassigned';
    }

    $table = 'dw_campaigns_groups';

    if(!db_table_exists($table)) {
        return $rows;
    }

    $results    = db_select($table, 'g')->fields('g')->execute();

    foreach($results as $result) {
        $rows[$result->group_id] = $result->group_name;

    }
 
    return $rows;
}

function dw_campaigns_group_to_tag_id($id, $by_id = TRUE) {
    static $group_maps = NULL;

    if(is_null($group_maps)) {
        $group_maps = array();

        $res = db_select('dw_campaigns_groups', 'g') ->fields('g')->execute();

        foreach($res as $row) {
            $group_maps['id_to_tag'][$row->group_id]          = $row->group_tag_id;
            $group_maps['tag_to_id'][$row->group_tag_id]      = $row->group_id;
        }        
    }

    if($by_id) {
        return isset($group_maps['id_to_tag'][$id]) ? $group_maps['id_to_tag'][$id] : NULL;
    } else {
        return isset($group_maps['tag_to_id'][$id]) ? $group_maps['tag_to_id'][$id] : NULL;
    }
}

function dw_campaigns_campaign_id_to_contribution_page_id($id, $by_id = TRUE) {
    static $node_page_maps = NULL;

    if(is_null($node_page_maps)) {
        $node_page_maps = array('id_to_page' => array(), 'page_id_id' => array());

        $res = db_select('dw_campaigns_campaigns_to_contribution_pages', 'cc') ->fields('cc')->execute();

        foreach($res as $row) {
            $node_page_maps['id_to_page'][$row->nid]                    = $row->contribution_page_id;
            $node_page_maps['page_to_id'][$row->contribution_page_id]   = $row->nid;
        }        
    }

    if($by_id) {
        return isset($node_page_maps['id_to_page'][$id]) ? $node_page_maps['id_to_page'][$id] : NULL;
    } else {
        return isset($node_page_maps['page_to_id'][$id]) ? $node_page_maps['page_to_id'][$id] : NULL;
    }


}

function dw_campaigns_host_event_create_page() {

    global $user;

    $form = drupal_get_form('dw_campaigns_host_event_create_form');

    return theme('dw_campaigns_host_event_create_page_theme', array('form' => $form));

}

function dw_campaigns_host_event_create_form($form, &$form_state, $include_contact = TRUE, $include_hidden = FALSE) {
    global $user;
    global $accepted_currencies;

    $form = array();

    if($include_contact) {
        // I'm unsure which way they will end up wanting it, so do it both ways then show the simpiler version
        $contact        = dw_campaigns_resolve_contact($user);
    
        // FIXME - I don't like fixing this problem this way 
        $in_country = isset($form_state['input']['country']) ? $form_state['input']['country'] : 1228; 
        $address_form   = dw_campaigns_address_form_fields($contact, TRUE, $in_country);

        $userpass_form  = dw_campaigns_userpass_form_fields(); 
   
        unset($address_form['telephone']['#required']);
 
        $form['host-contact'] = array(
            '#type'             => 'fieldset',
            '#title'            => t('Primary Host Contact')
        );
    
    
        if($user->uid > 0) {
    
            $form['host-contact']['login-info'] = array(
                '#type'             => 'markup',
                '#markup'           => t('You are currently logged in as @username (@email) - this account will be submitted as the primary host for this event, if you would like to register to host with a new account !logout and revisit this form.', array('@username' => $user->data['displayname'], '@email' => $user->mail, '!logout' => '<a style="color:red" href="/user/logout?destination=dw/walking/host/create">Logout</a>'))
            );
    
        } else {
    
            $form['host-contact']['login-hint'] = array(
                '#type'             => 'markup',
                '#markup'           => '<h2 class="login-hint">' . t('Did you participate in a one small step event last year? !link', array('!link' => '<a style="color:white" class="btn login-here" href="/user/login?destination=dw/walking/host/create">Login Here</a>')) . '</h2>'
            );

            $form['host-contact']['explain-contact-info'] = array(
                '#type'             => 'markup',
                '#markup'           => '<h2 class="explain-contact-info">' . t('Please complete the following form so we may contact you regarding hosting an event') . '</h2>'
            );
        
            $form['host-contact']   = array_merge($form['host-contact'], $address_form);
            unset($form['host-contact']['address_2']);
  
            if(1==1) {    
            $form['host-login'] = array(
                '#type'             => 'fieldset',
                '#title'            => t('Primary Host Account Information')
            );

            unset($userpass_form['displayname-description']);            
            unset($userpass_form['displayname']);            
            unset($userpass_form['username-description']);            

            $form['host-login']   = array_merge($form['host-login'], $userpass_form);


            } 
            // username and password fields
        }

    }

    if(isset($form_state['input']['email']) && !empty($form_state['input']['email'])) {

        if(!dw_campaigns_user_is_email_unique($form_state['input']['email'])) {
            $form['host-contact']['email']['#description'] = '<div class="email-in-use" style="font-size:16px;color:red"><span style="display:block;" class="have-account">You already have an account on the One SMALL Step system.  Please <a href="#" class="btn">Login</a></span><span>Forgot your Login/Password? <a class="btn" href="#">Recover Login/Pass</a></span></div>';  
        }
    }

    $form['#attributes']    = array(
        'class'             => array('dw_campaigns-profile-form'),
        'enctype'           => "multipart/form-data"
    );


    $form['event-information'] = array(
        '#type'             => 'fieldset',
        '#title'            => t('Event Information')
    );

    $form['event-information']['location-city'] = array(
        '#type'             => 'textfield',
        '#title'            => t('Location (city)'),
        '#description'      => t('The city/location will be the title of your event and the name used to list your event.'),
        '#required'         => TRUE,
        '#default_value'    => ''
    );

    if($include_hidden) {
    
        $form['event-information']['fundraising-goal'] = array(
            '#type'             => 'textfield',
            '#title'            => t('Fundraising Goal'),
            '#description'      => t('This goal can be changed later'),
            '#required'         => TRUE,
            '#default_value'    => variable_get('dw_campaigns_fundraising_goal_start_value', '10000')
        );
    
    /*
        $currencies = $accepted_currencies;
    
        $form['event-information']['fundraising-currency'] = array(
            '#type'             => 'select',
            '#title'            => t('Fundraising Currency'),
            '#description'      => t('Currency that that transactions are captured in.  Please contact an admin if you have a need for a currency not listed or the ability to accept multiple currencies'),
            '#required'         => TRUE,
            '#options'          => $currencies,
            '#default_value'    => 'USD'
        );
    */
        $form['event-information']['date'] = array(
            '#type'             => 'date_popup',
            '#date_format'      => 'Y-m-d',
            '#title'            => t('Date of Event'),
            '#description'      => t('Strict date, in format "YYYY-MM-DD" (year month day)'),
        );
    
        $form['event-information']['event-date'] = array(
            '#type'             => 'textfield',
            '#title'            => t('Event Date (informal)'),
            '#description'      => t('You can specify "3rd saturday of March" - this is displayed in event details'),
        );
    
        $form['event-information']['event-time'] = array(
            '#type'             => 'textfield',
            '#title'            => t('Event Time (informal)'),
            '#description'      => t('Ex: 1:15pm or Afternoon'),
        );
    
        $form['event-information']['event-location'] = array(
            '#type'             => 'textfield',
            '#title'            => t('Location Name'),
            '#description'      => t('Ex: Chesterfield Highschool parking lot'),
        );
      
        $form['event-information']['event-address'] = array(
            '#type'             => 'textfield',
            '#title'            => t('Event Address'),
            '#description'      => t('Address, city, state, zip are used for mapping - these must be real values'),
        );
         
        $form['event-information']['event-city'] = array(
            '#type'             => 'textfield',
            '#title'            => t('Event City'),
        );
         
        $form['event-information']['event-state'] = array(
            '#type'             => 'textfield',
            '#title'            => t('Event State'),
        );
         
        $form['event-information']['event-postal_code'] = array(
            '#type'             => 'textfield',
            '#title'            => t('Event Postal Code'),
        );
         
        $form['event-information']['event-description'] = array(
            '#type'             => 'textarea',
            '#title'            => t('Event Description'),
        );
    
        $form['event-information']['event-details'] = array(
            '#type'             => 'textarea',
            '#title'            => t('Additional Details (optional)'),
        );
    
        $form['event-information']['event-prizes'] = array(
            '#type'             => 'textarea',
            '#title'            => t('Prizes (optional)'),
        );
    }
    
 // Locataion
 // Fundraising Goal
// Event Image ?
 // Event Date
 // Event Time
 // Location Name
 // Event Address
 // city, state, zip
 // event description
 // additional details (optional)
 // prizes (optional)

    $form['submit']         = array(
        '#type'             => 'submit',
        '#value'            => t('Submit'),
        '#attributes'       => array('class'=> array('btn'))
    );

    return $form;


}

function dw_campaigns_host_event_create_form_validate(&$form, &$form_state) {
    global $user;

    $values = $form_state['values'];

    if($user->uid < 1) {

        // username is only set if we do single step host event creation - we generate these values if its not set
        if(isset($values['username'])) {
            //double check username
            if(dw_campaigns_username_search_ajax($values['username'], FALSE) == 'found') {
                form_set_error('username',t('The username you have selected is already taken'));
                return;
            }
            
            //verify passwords
            if($values['password'] != $values['password2']) {
                form_set_error('password',t('Your passwords do not match, please try again'));
            }
        } 
        //check email
        if(!dw_campaigns_user_is_email_unique($values['email'])) {
            form_set_error('email',t('The email you entered was already taken.'));
        }

        if(!dw_campaigns_user_is_email_unique_civi($values['email'])) {
            form_set_error('email',t('The email you entered was already taken in civi.'));
        }
    } 
   
    // this is a place holder for a duplicate city check if we need one 
    if(!isset($values['location-city'])  || empty($values['location-city'])) {
        form_set_error('location-city', t('The Location City must be unique!'));
    }
   
 
    
}

function dw_campaigns_lookup_host_record($user_id) {
    $result = db_query('select entity_id from {field_data_field_dw_host_user} where field_dw_host_user_value = :uid', array(':uid' => $user_id))->fetchAssoc();

    return isset($result->entity_id) ? node_load($result->entity_id) : FALSE;
}

function dw_campaigns_host_event_create_form_submit(&$form, &$form_state) {
    global $user;

    $values         = $form_state['values'];
   
    $new_user       = FALSE;
 
    if($user->uid < 1) {
        $res        = dw_campaigns_create_civi_contact_from_form_helper($values);

        $new_user   = TRUE;

        $contact    = $res['contact'];
        $user       = $res['user'];
    } else {
        $contact    = _dw_civicrm_contact_get_from_user($user);
    }

    $host_record    = dw_campaigns_lookup_host_record($user->uid);

    $year   = date("Y"); 
	/*
	 FIXME our host registration window does not necessarily reflect the real year (in the fall you may sign
	 up hosts for the next year - this should be an admin form value
	 */
	
    $year   = '2014';
    
    // if we dont have a record for that host, create one and populate it with the values we have
    // EDIT - actually this has been changed in the spec - an admin could manually assign "old" host records to multiple walks, but for now each walk would have its own host record.  So   User ->> many host records
    if(1==1 || is_null($host_record) || !$host_record) {
        $node           = new stdClass();
        $node->type     = 'dw_host_record';
        node_object_prepare($node);
        $node->is_new   = true;
        $node->language = LANGUAGE_NONE;


        $title  = sprintf("%s - %s", $values['location-city'], $year);

        $node->title                                        = $title;
        $node->field_dw_host_name['und']['0']['value']      = $contact->display_name;
        $node->field_dw_host_phone['und']['0']['value']     = '';
        $node->field_dw_host_email['und']['0']['value']     = $contact->email;
        $node->field_dw_host_street['und']['0']['value']    = $contact->street_address;
        $node->field_dw_host_city['und']['0']['value']      = $contact->city;
        $node->field_dw_host_province['und']['0']['value']  = $contact->state_province;
        $node->field_dw_host_postal['und']['0']['value']    = $contact->postal_code;
        $node->field_dw_host_user['und']['0']['value']      = $user->uid;

        node_save($node);
        $host_record = $node;
    }

    $node           = new stdClass();
    $node->type     = 'dw_campaigns_derby';
    node_object_prepare($node);
    $node->is_new   = true;

    // copy over values
    $node->title = sprintf("%s, %s %s", variable_get('dw_campaigns_dw_node_title_prefix', ''), $values['location-city'], $year);
    $node->language = LANGUAGE_NONE;

    $node->field_dw_campaign_location['und']['0']['value']  = $values['location-city'];
    $node->field_dw_other_donations['und']['0']['value']    = 0;
    $node->field_dw_fundraising_goal['und']['0']['value']   = variable_get('dw_campaigns_fundraising_goal_start_value', '0');
    $node->field_dw_pcp_intro_text['und']['0']['value']     = variable_get('dw_campaigns_default_pcp_intro_text', ''); 
    $node->field_dw_event_tos['und']['0']['value']          = variable_get('dw_campaigns_walking_event_tos', '');
    $node->field_dw_bcc_receipt['und']['0']['value']        = variable_get('dw_campaigns_default_bcc_receipt', ''); 
    $node->field_dw_cc_receipt['und']['0']['value']         = variable_get('dw_campaigns_default_cc_receipt', ''); 
    $node->field_dw_receipt_from_email['und']['0']['value'] = variable_get('dw_campaigns_default_receipt_from_email', ''); 
    $node->field_dw_receipt_from_name['und']['0']['value']  = variable_get('dw_campaigns_default_receipt_from_name', ''); 
    $node->field_dw_receipt_subject['und']['0']['value']    = variable_get('dw_campaigns_default_receipt_subject', '');
    $node->field_dw_receipt_text['und']['0']['value']       = variable_get('dw_campaigns_default_receipt_text', '');
    $node->field_dw_thankyou_footer['und']['0']['value']    = variable_get('dw_campaigns_default_thankyou_footer', ''); 
    $node->field_dw_thankyou_text['und']['0']['value']      = variable_get('dw_campaigns_default_thankyou_text', ''); 
    $node->field_dw_thankyou_title['und']['0']['value']     = variable_get('dw_campaigns_default_thankyou_title', ''); 

    $node->field_dw_campaign_status['und']['0']['value']    = 'pending';
    $node->field_dw_host_primary['und']['0']['value']       = $host_record->nid;


    // this sets all the values from the form
    $node = dw_campaigns_host_event_set_variables($node, $values);

/*
      $form['event-information']['event-details'] = array(
      $form['event-information']['event-prizes'] = array(
*/
    node_save($node);

    $node_event = dw_campaigns_create_extended_event($node->nid);

    // copy over values
    $node_event->title      = sprintf("%s (extended), %s %s", variable_get('dw_campaigns_dw_node_title_prefix', ''), $values['location-city'], $year);

    $node_event->field_dw_eventdetails_prizes['und']['0']['value'] = isset($values['event-prizes']) ? $values['event-prizes'] : '';
    $node_event->body['und']['0']['value']                         = isset($values['event-details']) ? $values['event-details'] : '';

    node_save($node_event);

    $to     = variable_get('dw_campaigns_host_submit_email', '');
    $from   = variable_get('dw_campaigns_default_receipt_from_email', 'root@');

    if(!empty($to)) {

        $data['nid']    = $node->nid;
        $data['title']  = $node->title;
        $data['to']     = $to;
        $data['contact'] = $contact;

        drupal_mail('dw_campaigns_host_event_create_alert', 'event_create_alert', $to,  user_preferred_language($user), $data, $from);

    }

    // if username begins with PH1 treat as "headless" and do not create email forward, or tag contact
    if(strncmp($user->name, 'PH1', 3) == 0) {
        module_invoke_all('user_logout', $user);
        session_destroy();
    }

    $alias = sprintf("%s.%s@%s", $contact->first_name, $contact->last_name, variable_get('dw_campaigns_host_forward_domain_name', $_SERVER['HTTP_HOST']));

    $dbtable = variable_get('dw_campaigns_host_forward_database_table', '');

    if(!empty($dbtable)) {
        try { 
            db_query('insert into :db VALUES(:from, :to)', array(':db' => $dbtable, ':from' => $alias, ':to' => $user->mail));
        } catch (Exception $e) {
    
        }
    }

    $suffix = variable_get('dw_campaigns_cfg_tag_suffix', $year);
    $name   = variable_get('dw_campaigns_cfg_tag_prefix', 'OSS') . ' host ' . $suffix;
    $tag_id = dw_campaigns_get_tag_id_by_name($name);
    dw_campaigns_tag_contact($tag_id, array($contact->contact_id));

    //drupal_set_message("Request has been submitted, you will receive an email in the near future to give you further instructions");
    drupal_set_message("Thank you for registering your event.  Your One SMALL Step coordinator will be in touch with you shortly to help you set up your event.");
    drupal_goto('dw');
   
}

function dw_campaigns_host_event_create_alert_mail($key, &$message, $params){
    global $user;

    $contact_fields = array(
        'first_name'            => 'First Name',
        'last_name'             => 'Last Name',
        'street_address'        => 'Address',
        'city'                  => 'city',
        'state_province_name'   => 'State',
        'country'               => 'Country',
        'postal_code'           => 'Postal Code',
        'email'                 => 'Email',
        'phone'                 => 'Phone'
    );

    switch($key){

        case 'event_create_alert':
            $subject            = 'New Event Created';

            $message['subject'] = $subject;

            $contact_body       = '';
            foreach($contact_fields as $key => $label) {
                $contact_body .= $label . ' : ' . $params['contact']->$key . "\n";
            } 

            $body               = "A new campaign has been created: {$params['title']}\n <a href=\"http://{$_SERVER['HTTP_HOST']}/node/{$params['nid']}/edit\">Click here to edit this campaign</a>";
            $body               .= "\n\n\\n$contact_body";

            //$body.= " original to: " . $params['to'];
            $message['body'][]  = $body;
            break;
    }

}

function dw_campaigns_host_event_set_variables($node, $values) {
    $fund_goal  = isset($values['fundraising-goal']) ? $values['fundraising-goal'] : 0;
    $fund_goal  = preg_replace('/[^0-9\.]/', '', $fund_goal);
    $fund_goal  = round($fund_goal);

    $node->field_dw_fundraising_goal['und']['0']['value']   = $fund_goal;

// FIXME - this should be on an admin form
    $node->field_dw_date_range['und']['0']['value']         = isset($values['date']) ? $values['date'] . 'T00:00:00' : '2014-12-31T23:59:59'; 
    $node->field_dw_date_range['und']['0']['value2']        = isset($values['date']) ? $values['date'] . 'T00:00:00' : '2014-12-31T23:59:59'; 

    $node->field_dw_therm_currency['und']['0']['value']     = isset($values['thermometer'])    ? $values['thermometer']                : 'USD';
    $node->field_dw_event_date['und']['0']['value']         = isset($values['event-date'])     ? $values['event-date']                 : '';
    $node->field_dw_event_time['und']['0']['value']         = isset($values['event-time'])     ? $values['event-time']                 : '';
    $node->field_dw_event_location['und']['0']['value']     = isset($values['event-location']) ? $values['event-location']             : '';
    $node->field_dw_address_street['und']['0']['value']     = isset($values['event-address'])  ? $values['event-address']              : '';
  //$node->field_dw_address_country['und']['0']['value']    = isset($values['event-address'])  ? $values['event-address']              : '';;
    $node->field_dw_address_city['und']['0']['value']       = isset($values['event-city'])     ? $values['event-city']                 : '';
    $node->field_dw_address_province['und']['0']['value']   = isset($values['event-state'])    ? $values['event-state']                : '';
    $node->field_dw_address_postal_code['und']['0']['value']= isset($values['event-postal_code'])    ? $values['event-postal_code']    : '';
    if(isset($values['fundraising-currency'])) {
        $node->field_dw_currency['und']['0']['value']       = !empty($values['fundraising-currency']) ? $values['fundraising-currency'] : 'USD';
    }

    $node->body['und']['0']['value']                        = isset($values['event-description']) ? $values['event-description'] : '';


    return $node;
}

// this is the "add offline donation" page
function dw_campaigns_user_donations_add_page_new($campaignId = NULL) {
    global $user;

    //return "donations are currently disabled";

    $type = dw_campaigns_get_selected_type();

    if(is_null($campaignId)) {
        if($type == 'walking') {
            // if we have one, lets get fancy and look up everything for the user from this
            $pcp          = dw_campaigns_user_get_pcp_for_walking($user);
   
            if(!is_null($pcp->id)) {
                $contribution_page_id   = $pcp->contribution_page_id;
   
                $campaigns  = dw_campaigns_get_active_campaigns(TRUE);
                $campaign   = $campaigns[$contribution_page_id];
                $campaignId = $campaign->nid;
            }
        }
    
        if(is_null($campaignId)) {
            $campaignId  = dw_campaigns_get_default_campaign();
        }

        if($type != 'walking') {
            $campaign = node_load($campaignId);

            $pcp          = dw_campaigns_user_get_pcp_for_campaign($user, $campaign);
        }
    }
    $campaign = node_load($campaignId);

    $form = drupal_get_form('dw_campaigns_user_donations_add_form_new', $campaign, $pcp);
// NOTE - campaign and pcp dont seem to be used by this theme..
    return theme('dw_campaigns_user_donations_add_theme', array('form' => $form, 'campaign' => $campaign, 'pcp' => $pcp));
}

function dw_campaigns_user_donations_add_page_new_host($campaignId = NULL, $pcp_id) {
    global $user;

    //return "donations are currently disabled";
    $type = dw_campaigns_get_selected_type();

    list($pcp, $campaign) = dw_campaigns_verify_campaign_matches_pcp_id($campaignId, $pcp_id);

    $form = drupal_get_form('dw_campaigns_user_donations_add_form_new', $campaign, $pcp);
// NOTE - campaign and pcp dont seem to be used by this theme..
    return theme('dw_campaigns_user_donations_add_theme', array('form' => $form, 'campaign' => $campaign, 'pcp' => $pcp));
}

function dw_campaigns_user_donations_add_form_new($form, &$form_state, $campaign, $pcp){
    $form = array();

    $form['#attributes']    =  array(
        'class'             => array('dw_campaigns-profile-form'),
        'enctype'           => 'multipart/form-data'
    );

    $multi = FALSE;

    $currency = ' (' . $campaign->field_dw_currency['und']['0']['value'] . ')';
    if($currency == ' (USD)') {
        $currency   = '';
    }
 
    if($currency == ' (MULTI)') {
        $currency   = '';
        $multi      = TRUE;
    }

    $form['donation_amount'] = array(
        '#type'             => 'textfield',
        '#title'            => t('Donation Amount') . ' ' . $currency,
        '#default_value'    => intval(variable_get('default_contribution_amount', '100'))
    );


    if($multi) {
	$currencies = dw_campaigns_all_currencies();

        if(isset($currencies['MULTI'])) {
            unset($currencies['MULTI']);
        }

        $form['donation-currency'] = array(
            '#type'         => 'select',
            '#title'        => t('Currency'),
            '#options'      => $currencies
        );
    }

    // FIXME - I don't like fixing this problem this way 
    $in_country = isset($form_state['input']['country']) ? $form_state['input']['country'] : 1228; 
    $address_form   = dw_campaigns_address_form_fields(NULL, FALSE, $in_country);

    $form           = array_merge($form, $address_form);
    

    unset($form['address_1']['#required']);
    unset($form['address_2']['#required']);
    unset($form['city']['#required']);
    unset($form['postal_code']['#required']);
    unset($form['email']['#required']);
    
    $form['address_1']['#prefix']   = '<div class="terse-hide">';
    $form['address_2']['#prefix']   = '<div class="terse-hide">';
    $form['city']['#prefix']        = '<div class="terse-hide">';
    $form['postal_code']['#prefix'] = '<div class="terse-hide">';
    $form['state']['#title']        = 'State / Province';
    $form['address_1']['#suffix']   = '</div>';
    $form['address_2']['#suffix']   = '</div>';
    $form['city']['#suffix']        = '</div>';
    $form['postal_code']['#suffix'] = '</div>';
    
    unset( $form['address_2'] );

  
    $form['payment_instrument'] = array(
        '#type'             => 'select',
        '#title'            => t('Payment Instrument'),
        '#options'          => array('check' => 'Check', 'cash' => 'Cash')
    );
    
    $form['payment_check_number'] = array(
        '#type'             => 'textfield',
        '#title'            => t('Check Number')
    );
    
 
    $form['include_in_honor_roll'] = array(
        '#type'             => 'checkbox',
        '#title'            => t('Show First and Last name on donation list (will show anonymous otherwise)'),
        '#default_value'    => 1
    );

    $form['non_deductible'] = array(
        '#type'             => 'checkbox',
        '#title'            => t('Non-deductible'),
        '#description'      => t('Check this box if this contribution does NOT require a tax receipt.'),
        '#default_value'    => 0,
    );

    global $user;

    if(in_array('site admin', array_values($user->roles))) {
        $form['receive_date'] = array(
            '#type'             => 'textfield',
            '#title'            => t('Receive Date'),
            '#description'      => t('Date Format of YYYY-MM-DD Eg: 2012-07-11 for July 11th 2012')
        );

        $form['trxn_id'] = array(
            '#type'             => 'textfield',
            '#title'            => t('Transaction Id'),
            '#default_value'    => NULL
        );
    }

    $form['submit-fieldset']['submit'] = array(
        '#type'             => 'submit',
        '#value'            => 'Donate Now',
        '#attributes'       => array('class' => array('btn', 'confirm-amount'))
    );


    $form['campaign'] = array(
        '#type'             => 'hidden',
        '#default_value'    => $campaign->nid
    );

    $form['pcpId'] = array(
        '#type'             => 'hidden',
        '#default_value'    => $pcp->id
    );

    return $form;
}

function dw_campaigns_user_donations_add_form_new_validate(&$form, &$form_state) {
    global $user;

    $form_state['values']['receive_date']       = isset($form_state['values']['receive_date']) ? $form_state['values']['receive_date']  : date('Y-m-d');
    $form_state['values']['trxn_id']            = isset($form_state['values']['trxn_id']) ? $form_state['values']['trxn_id']  : '';

    $form_state['values']['donation_amount']    = preg_replace("/[^0-9\.]/", '', $form_state['values']['donation_amount']);
    
    $values             = $form_state['values'];
 
    $campaign           = node_load($values['campaign']);

    // $campaign is the drupal node, not an id
    $contributionType   = dw_campaigns_create_contribution_type_for_campaign($campaign);
    $contributionPage   = dw_campaigns_get_contribution_page_for_campaign($campaign);
 
    $currency           = $contributionPage->currency;

    $multi = FALSE;

    if($campaign->field_dw_currency['und']['0']['value'] == 'MULTI') {
        $multi = TRUE;
    }

    // this is not yet working LOOK
    if($multi) {
        global $currency_processor;

        $picked_currency = $values['donation-currency'];
        if(!isset($currency_processor[$picked_currency])) {
            form_set_error('', t('Invalid Currency') . $picked_currency);
            return;
        }
        $currency    = $picked_currency;
    }

    if(empty($currency)) {
        form_set_error('','Currency unknown');
        return;
    }
 
    $remote_addr        = $_SERVER['REMOTE_ADDR'];
    if($remote_addr == '::1') {
        $remote_addr    = '127.0.0.1';
    }

    if($values['payment_instrument'] == 'check') {
        if(strlen($values['payment_check_number'] < 2)) {
            form_set_error('payment_check_number','Must include check number for checks');
            return;
        }
    }

    if($values['donation_amount'] <= 0) {
            form_set_error('donation_amount','Must be a value > 0.00');
            return;
    }

    if(isset($_SESSION['messages']['error']) && count($_SESSION['messages']['error']) > 0) {
        form_set_error('', 'Please fix errors and try again');
        return;
    }

    if(!empty($values['receive_date'])) {  
        $r_d = trim($values['receive_date']);
        if(preg_match('/^\d{4}-\d{2}-\d{2}$/', $r_d) == 0) {
            form_set_error('receive_date', t('Receive date must be YYYY-MM-DD !'));
            return;
        }

        $form_state['values']['receive_date'] = $r_d;
    }
    
    // we do all the customer creation now to be like a real donation, on a normal donation we don't know if we should accept it or not until the validate step tries to bill the card, but we need a contact to do that, so we build the contact here...
    if(empty($form_state['values']['email'])) {
	$temp_mail       = variable_get('site_mail', '@templatesandbox.com');
	$parts           = explode('@', $temp_mail);
	$base_email      = isset($parts[1]) ? $parts[1] : 'templatesandbox.com';

        $email           = sprintf("od-%s-%s@%s", time(), $values['pcpId'], $base_email);
	$form_state['values']['email'] = $email;
        $values['email']               = $email;
    }

    $form_state['values']['currency']       = $currency;
    $form_state['values']['remote_addr']    = $remote_addr;

    $table = 'dw_campaigns_offline_donation';
    if(!db_table_exists($table)) {
        dw_campaigns_create_offline_donation_table();
    }
}

function dw_campaigns_create_offline_donation_table($table = 'dw_campaigns_offline_donation') {
        // just rounded off sizes for storage, I'd rather it be too big than not big enough
        db_query(
            "CREATE TABLE {$table} (
            `offline_id`            INT UNSIGNED NOT NULL AUTO_INCREMENT,
            `contribution_id`       INT NOT NULL DEFAULT -1,
            `deleted`               INT NOT NULL DEFAULT 0,
            `responsible_drupal_id` INT NOT NULL DEFAULT 0,
            `remote_addr`           char(20) NOT NULL DEFAULT '',
            `currency`              char(20) NOT NULL DEFAULT '',
            `first_name`            char(200) NOT NULL DEFAULT '',
            `last_name`             char(200) NOT NULL DEFAULT '',
            `email`                 char(200) NOT NULL DEFAULT '',
            `address_1`             char(200) NOT NULL DEFAULT '',
            `city`                  char(200) NOT NULL DEFAULT '',
            `state`                 char(200) NOT NULL DEFAULT '',
            `postal_code`           char(200) NOT NULL DEFAULT '',
            `country`               char(200) NOT NULL DEFAULT '',
            `pcp_id`                char(20) NOT NULL DEFAULT '',
            `campaign`              char(20) NOT NULL DEFAULT '',
            `include_in_honor_roll` char(20) NOT NULL DEFAULT '',
            `donation_amount`       char(20) NOT NULL DEFAULT '',
            `payment_instrument`    char(20) NOT NULL DEFAULT '',
            `payment_check_number`  char(20) NOT NULL DEFAULT '',
            `trxn_id`               char(20) NOT NULL DEFAULT '',
            `receive_date`          char(20) NOT NULL DEFAULT '',
            PRIMARY KEY ( `offline_id` ),
            INDEX ( `pcp_id` ),
            INDEX ( `deleted` )
            ) auto_increment=100;
            "
        );
}

// NOTE - SCHEMA CHANGE: ALTER TABLE `dw_campaigns_offline_donation` ADD `non_deductible` INT( 11 ) NOT NULL DEFAULT '0'
function dw_campaigns_user_donations_add_form_new_submit(&$form, &$form_state) {
    global $user;
    global $dw_campaign_module_path;

    $values = $form_state['values'];

    if(empty($values['receive_date'])) {
        $values['receive_date'] = date('Y-m-d h:i:s');
    }

    cache_clear_all($values['pcpId'] . '_CONVERT', 'cache');
    cache_clear_all($values['pcpId'] . '_DONT', 'cache');

    $table = 'dw_campaigns_offline_donation';
    db_insert($table)->fields(array(
        'responsible_drupal_id' => $user->uid,
        'remote_addr'           => $values['remote_addr'],
        'currency'              => $values['currency'],
        'first_name'            => $values['first_name'],
        'last_name'             => $values['last_name'],
        'email'                 => $values['email'],
        'address_1'             => $values['address_1'],
        'city'                  => $values['city'],
        'state'                 => $values['state'],
        'postal_code'           => $values['postal_code'],
        'country'               => $values['country'],
        'pcp_id'                => $values['pcpId'],
        'campaign'              => $values['campaign'],
        'include_in_honor_roll' => $values['include_in_honor_roll'],
        'donation_amount'       => $values['donation_amount'],
        'payment_instrument'    => $values['payment_instrument'],
        'payment_check_number'  => $values['payment_check_number'],
        'trxn_id'               => $values['trxn_id'],
        'receive_date'          => $values['receive_date'],
        'non_deductible'        => $values['non_deductible'],
    ))->execute();

    drupal_set_message("Offline Contribution Added");

    echo '<p>Donation has been recorded.. reloading donations list</p><script language="javascript" type="text/javascript"> parent.location.reload(); </script>';
    die;
}

function dw_campaigns_get_offline_donations_for_pcp($pcpId) {
    $offlines   = array();

    $table = 'dw_campaigns_offline_donation';
    if(!db_table_exists($table)) {
        return $offlines;
    }

    $results    = db_query("select * from {$table} where deleted=0 and pcp_id=:pcp_id", array(':pcp_id' => $pcpId));
    foreach($results as $row) {
        $offlines[] = $row;
    }

    return $offlines;
}

function dw_campaigns_offline_permissions($offline_id) {
    global $user;

    $permission         = FALSE;

    $offline_donation   = dw_campaigns_get_offline_donation($offline_id);
    $pcp                = dw_campaigns_user_get_pcp_by_id($offline_donation->pcp_id); 

    if(!isset($pcp->id)) {

        drupal_set_message(t('You do not have permission to this offline donation') . ' (1)', 'error');
        drupal_goto('dw');
        return FALSE;
    }

    $dummy              = new stdClass;
    $dummy->id          = $pcp->contribution_page_id;
    $campaign           = dw_campaigns_get_campaign_from_contribution_page($dummy);
    $user_id            = _dw_campaigns_contact_id_get_user($pcp->contact_id);

    $offline_user       = user_load($user_id);

    // if the logged in user owns it
    if($offline_user->uid == $user->uid) {
        $permission     = TRUE;
    }

    if($user->uid == 0) {
        drupal_set_message(t('You do not have permission to this offline donation') . ' (2)', 'error');
        drupal_goto('dw');
        return FALSE;
    }

    // if its the admin user 
    if($user->uid == 1) {
        $permission     = TRUE;
    }

    $ids = dw_campaigns_get_hosts_for_uid($user->uid);

    $primary_id = isset($campaign->field_dw_host_primary['und']['0']['value']) ? $campaign->field_dw_host_primary['und']['0']['value'] : -1;
    $secondary_id = isset($campaign->field_dw_host_secondary['und']['0']['value']) ? $campaign->field_dw_host_secondary['und']['0']['value'] : -1;

    // if they are logged in as the host for this event
    if($primary_id != -1  && isset($ids[$primary_id])) {
        $permission     = TRUE;
    }

    if($secondary_id != -1  && isset($ids[$secondary_id])) {
        $permission     = TRUE;
    }

    // regardless of other permissions if the offline donation has been posted it can not be edited
    if($offline_donation->contribution_id != -1) {
        return FALSE;
    } 

    if(!$permission) {
        drupal_set_message(t('You do not have permission to this offline donation') . ' (3)', 'error');
        $target = isset($_REQUEST['destination']) ? $_REQUEST['destination'] : 'dw/'; 
        drupal_goto($target);

        return FALSE;
    }
    
    return TRUE; 
}

function dw_campaigns_get_offline_donation($offline_id) {
    static $donation    = array();

    $offline_donation   = NULL;


    if(!isset($donation[$offline_id])) {

        $offline_donation       = db_query('select * from {dw_campaigns_offline_donation} where offline_id=:offline_id', array(':offline_id' => $offline_id))->fetchObject();

        $donation[$offline_id]  = $offline_donation;

    } else {
        $offline_donation       = $donation[$offline_id];
    }

    return $offline_donation;
}

function dw_campaigns_offline_edit($offline_id) {

    if(!dw_campaigns_offline_permissions($offline_id)) {
        return;
    }

    $offline_donation   = dw_campaigns_get_offline_donation($offline_id);

    $pcp                = dw_campaigns_user_get_pcp_by_id($offline_donation->pcp_id); 

    $form               = drupal_get_form('dw_campaigns_offline_edit_form', $pcp, $offline_donation);

    return drupal_render($form);
}

function dw_campaigns_offline_edit_form($form, &$form_state, $pcp, $offline_donation) {

    $dummy      = new stdClass;
    $dummy->id  = $pcp->contribution_page_id;
    $campaign   = dw_campaigns_get_campaign_from_contribution_page($dummy);

    $form       = dw_campaigns_user_donations_add_form_new($form, $form_state, $campaign, $pcp);

    $form['first_name']['#default_value']               = $offline_donation->first_name;
    $form['last_name']['#default_value']                = $offline_donation->last_name;
    $form['email']['#default_value']                    = $offline_donation->email;
    $form['address_1']['#default_value']                = $offline_donation->address_1;
    $form['city']['#default_value']                     = $offline_donation->city;
    $form['state']['#default_value']                    = $offline_donation->state;
    $form['postal_code']['#default_value']              = $offline_donation->postal_code;
    $form['country']['#default_value']                  = $offline_donation->country;
    $form['pcpId']['#default_value']                    = $offline_donation->pcp_id;
    $form['campaign']['#default_value']                 = $offline_donation->campaign;
    $form['include_in_honor_roll']['#default_value']    = $offline_donation->include_in_honor_roll;
    $form['donation_amount']['#default_value']          = $offline_donation->donation_amount;
    $form['payment_instrument']['#default_value']       = $offline_donation->payment_instrument;
    $form['payment_check_number']['#default_value']     = $offline_donation->payment_check_number;
    $form['trxn_id']['#default_value']                  = $offline_donation->trxn_id;
    $form['receive_date']['#default_value']             = $offline_donation->receive_date;
    $form['non_deductible']['#default_value']           = $offline_donation->non_deductible;

    $form['offline_id'] = array(
        '#type'             => 'hidden',
        '#default_value'    => $offline_donation->offline_id
    );

    return $form;
}

function dw_campaigns_offline_edit_form_validate(&$form, &$form_state) {
// TODO LOOK SECURITY
// reuse the other validator, we should also check to make sure we have the rights to do this operation
    dw_campaigns_user_donations_add_form_new_validate($form, $form_state);
}

function dw_campaigns_offline_edit_form_submit(&$form, &$form_state) {

    global $user;

    $values = $form_state['values'];

    $table  = 'dw_campaigns_offline_donation';

    cache_clear_all($values['pcpId'] . '_CONVERT', 'cache');
    cache_clear_all($values['pcpId'] . '_DONT', 'cache');

    db_update($table)->fields(array(
        'responsible_drupal_id' => $user->uid,
        'remote_addr'           => $values['remote_addr'],
        'currency'              => $values['currency'],
        'first_name'            => $values['first_name'],
        'last_name'             => $values['last_name'],
        'email'                 => $values['email'],
        'address_1'             => $values['address_1'],
        'city'                  => $values['city'],
        'state'                 => $values['state'],
        'postal_code'           => $values['postal_code'],
        'country'               => $values['country'],
        'pcp_id'                => $values['pcpId'],
        'campaign'              => $values['campaign'],
        'include_in_honor_roll' => $values['include_in_honor_roll'],
        'donation_amount'       => $values['donation_amount'],
        'payment_instrument'    => $values['payment_instrument'],
        'payment_check_number'  => $values['payment_check_number'],
        'trxn_id'               => $values['trxn_id'],
        'receive_date'          => $values['receive_date'],
        'non_deductible'        => $values['non_deductible'],
    ))->condition('offline_id', $values['offline_id'], '=')->execute();
}

function dw_campaigns_offline_delete($offline_id) {

// LOOK TODO SECURITY - need to add a check that this user can actually do this
    if(!dw_campaigns_offline_permissions($offline_id)) {
        return;
    }

    db_update('dw_campaigns_offline_donation')->fields(array('deleted' => 1))->condition('offline_id', $offline_id, '=')->execute();

    $target = isset($_REQUEST['destination']) ? $_REQUEST['destination'] : 'dw/'; 
    drupal_goto($target);
}

// the civicrm dedupe/merge process can renumber contacts that we use.  So a helper module has been wrote which will notify us of changes.  We will blindly try to adjust our local mappings to accomidate these changes
function dw_campaigns_callback_merge() {
    $login          = isset($_REQUEST['login']) ? $_REQUEST['login'] : NULL;
    $pass           = isset($_REQUEST['pass']) ? $_REQUEST['pass'] : NULL;
    
    // we don't allow empty logins or passes, so don't bother with a lookup
    if(strlen($login) < 1 || strlen($pass) < 1) {
        return;
    }
    
    $r_login        = variable_get('dw_campaigns_dedupe_callback_login', '');
    $r_pass         = variable_get('dw_campaigns_dedupe_callback_pass', '');
    
    if($login != $r_login || $pass != $r_pass) {
        return;
    }
    
    $civi_id_from   = isset($_REQUEST['civi_id_from']) ? intval($_REQUEST['civi_id_from']) : NULL;
    $civi_id_to     = isset($_REQUEST['civi_id_to'])   ? intval($_REQUEST['civi_id_to'])   : NULL;
    
    if(is_null($civi_id_from) || is_null($civi_id_to)) {
        return;
    }
    
    db_query('update {dw_campaigns_donation_privacy} set civi_contact_id=:to where civi_contact_id=:from', array('from' => $civi_id_from, 'to' => $civi_id_to));
    
    db_query('update {dw_campaigns_drupal_civi_contact_mapping} set civi_contact_id=:to where civi_contact_id=:from', array('from' => $civi_id_from, 'to' => $civi_id_to));

    db_query('update {dw_campaigns_event_participants} set contact_id=:to where contact_id=:from', array('from' => $civi_id_from, 'to' => $civi_id_to));

}

function dw_campaigns_get_drupal_user() {
    return dw_campaigns_get_drupal_users();
}

/**
 * Return list of drupal users, with uid / email
 *
 */
function dw_campaigns_get_drupal_users() {
    $users = array(); 

    $temp_users = dw_campaigns_cache_simple_user_data('ALL');

    foreach($temp_users as $index => $user) {
        $users[$index] = trim(strtolower($user->mail)); 
    }

    asort($users);

    return $users;
}

function dw_campaigns_get_hosts() {
    static $hosts = NULL;

    if(is_null($hosts)) {

        $hosts  = array();

        $results = db_query("select * from {node} where type = 'dw_host_record'");
        foreach($results as $row) {
            $node               = node_load($row->nid); 
            $hosts[$row->nid]   = $node;
        }
    }

    return $hosts;   
}

function dw_campaigns_get_hosts_for_uid($uid) {
    static $hosts   = array();

    if(!isset($hosts[$uid])) {
        $all_hosts  =  dw_campaigns_get_hosts();
        $host_ids   = array();

        foreach($all_hosts as $host) {
            if($host->field_dw_host_user['und']['0']['value'] == $uid) {
                $host_ids[$host->nid] = $host;
            }
        }

        $hosts[$uid] = $host_ids;
    }

    return $hosts[$uid];   
}

function dw_campaigns_get_hosts_assignment_array() {
    $hosts  = dw_campaigns_get_hosts();

    $list   = array();
    foreach($hosts as $host) {
        //$nid        = $host->field_dw_host_user['und']['0']['value'];
        $nid        = $host->nid;

        $name       = $host->title;

        $email      = isset($host->field_dw_host_email['und']['0']['value']) ? $host->field_dw_host_email['und']['0']['value'] : '';
        $city       = isset($host->field_dw_host_city['und']['0']['value']) ? $host->field_dw_host_city['und']['0']['value'] : '';
        $state      = isset($host->field_dw_host_province['und']['0']['value']) ? $host->field_dw_host_province['und']['0']['value'] : '';

        $list[$nid] = "$name ($city, $state) - $email"; 
    }

    return $list;
}

function dw_campaigns_host_find_campaigns($host_user_id, $include_admin = TRUE) {
    static $status = array();

    if(!isset($status[$host_user_id])) {
        $matches = array();
   
        $host_user = user_load($host_user_id);

        $low_node = dw_campaigns_get_selected_type() == 'walking' ? 100 : 0;

// TODO make this actually do proper role checks =) 
        if($include_admin && ($host_user_id == 1 || (isset($host_user->roles['4']) && $host_user->roles['4'] == 'site admin'))) {
            $result = db_query("select nid from {node} where type='dw_campaigns_derby' and nid > $low_node");
            
            foreach($result as $row) {
                $matches[$row->nid] = $row->nid;
            }
        
        } else {   
    
            $ids = dw_campaigns_get_hosts_for_uid($host_user_id);
            $keys = array_keys($ids);
    
            if(count($keys)>0) {
        
                $result = db_select('field_data_field_dw_host_primary', 'n')->fields('n')->condition('bundle', 'dw_campaigns_derby', '=')->condition('field_dw_host_primary_value', $keys, 'IN')->execute();
            
                foreach($result as $row) {
                    $matches[$row->entity_id] = $row->entity_id;
                }
            
                $result = db_select('field_data_field_dw_host_secondary','n')->fields('n')->condition('bundle', 'dw_campaigns_derby', '=')->condition('field_dw_host_secondary_value', $keys, 'IN')->execute();
            
                foreach($result as $row) {
                    $matches[$row->entity_id] = $row->entity_id;
                }
            }
        }
        $status[$host_user_id]  = $matches;
    }

    return $status[$host_user_id];
}

function dw_campaigns_host_permission_check($campaign_id = NULL) {
    global $user;

    if($user->uid == 1) {
        return true;
    }

    $hosts = dw_campaigns_host_find_campaigns($user->uid);

    if(is_null($campaign_id)) {
        return (count($hosts) > 0);
    }   

    return isset($hosts[$campaign_id]);

}

function dw_campaigns_user_host_page() {
    global $user;

    $campaigns  = array();

    $hosts = dw_campaigns_host_find_campaigns($user->uid);
    $keys = array_keys($hosts);

    if(count($hosts) == 1) {
        drupal_goto('dw/user/host/' . $keys[0]);
    }

    $result = db_select("node", "n")->fields("n")->condition("nid", $keys, "IN")->execute();



    $campaigns_sort_array = array();

    foreach($result as $row) {
        $event = node_load( $row->nid );

        $event_title = $event->title;
        $date = isset($event->field_dw_date_range['und'][0]['value']) ? $event->field_dw_date_range['und'][0]['value'] : 'NoDateSet';
        $event_date = substr( $date, 0, 10 );

        //array of campaign titles with added date
        $campaigns_sort_array[ $event_title . ' (' . $event_date . ')' ] = $event;

    }

    //sort by newly formatted title/date
    ksort( $campaigns_sort_array );

    //transform list into an array that can be used by dw_campaigns_user_host_page_form
    foreach( $campaigns_sort_array as $campaign_title => $campaign ){
        $campaigns[ $campaign->nid ] = $campaign_title;
    }

    $form = drupal_get_form('dw_campaigns_user_host_page_form', $campaigns);

    return theme('dw_campaigns_user_host_page_theme', array('form' => $form));
}

function dw_campaigns_user_host_page_form($form, &$form_state, $campaigns) {

    $form = array();

    $form['#attributes']    = array(
        'class'             => array('dw_campaigns-user-host-form'),
        'enctype'           => "multipart/form-data"
    );
  
    $campaigns = array('-1' => 'Pick an Event') + $campaigns;
 
    $form['event_id'] = array(
        '#type'         => 'select',
        '#title'        => t('Event To Manage'),
        '#options'      => $campaigns
    );

    $form['submit']         = array(
        '#type'             => 'submit',
        '#value'            => t('Submit'),
        '#attributes'       => array('class'=> array('btn'))
    );

    return $form;
}

function dw_campaigns_host_event_donors_active($event_id){
    $csvDataArray = array();

    $header = array(
        'name',
        'email',
        'street',
        'city',
        'state',
        'postal'
    );

    $csvDataArray[] = $header;

    $event = node_load($event_id);

    $pcps = _dw_campaigns_get_pcps_for_campaign( $event );

    foreach( $pcps as $id => $pcp ){

        $supporters = dw_campaigns_pcp_get_supporters($pcp);

        foreach( $supporters as $supporter ){
            $pcp_contact = _dw_civicrm_contact_get(array(
                    'contact_id'  => $supporter->contact_id,
                    'returnFirst' => 1
                ));

            $csvData = array(
                $pcp_contact->display_name,
                $pcp_contact->email,
                $pcp_contact->street_address,
                $pcp_contact->city,
                $pcp_contact->state_province_name,
                $pcp_contact->postal_code
            );

            $csvDataArray[] = $csvData;
        }

    }

    return output_csv( 'host_event_donors_active_'.date('Y_m_d').'.csv', $csvDataArray );
}

function dw_campaigns_host_event_donors_pending($event_id){
    $csvDataArray = array();

    $header = array(
        'name',
        'email',
        'street',
        'city',
        'state',
        'postal'
    );

    $csvDataArray[] = $header;

    $event = node_load($event_id);

    $pcps = _dw_campaigns_get_pcps_for_campaign( $event );

    foreach( $pcps as $id => $pcp ){

        $supporters = dw_campaigns_pcp_get_supporters($pcp);

        foreach( $supporters as $supporter ){
            $contribution_id = $supporter->contribution_id;
            $contribution = dw_campaigns_get_contribution_by_id( $contribution_id );

            if( $contribution->contribution_status_id == 2 ){   //status 2 for pending
                $pcp_contact = _dw_civicrm_contact_get(array(
                        'contact_id'  => $supporter->contact_id,
                        'returnFirst' => 1
                    ));

                $csvData = array(
                    $pcp_contact->display_name,
                    $pcp_contact->email,
                    $pcp_contact->street_address,
                    $pcp_contact->city,
                    $pcp_contact->state_province_name,
                    $pcp_contact->postal_code
                );

                $csvDataArray[] = $csvData;
            }

        }

    }

    return output_csv( 'host_event_donors_pending_'.date('Y_m_d').'.csv', $csvDataArray );
}

function dw_campaigns_host_event_donors_csv($event_title, $event_date, $output_type){
    global $user;
    //get the event id for the title and date provided.

    $hosts = dw_campaigns_host_find_campaigns( $user->uid );
    $keys = array_keys( $hosts );

    foreach( $keys as $index => $nodeId ){
        $thisEvent = node_load( $nodeId );

        $thisEventTitle = $thisEvent->title;
        $thisEventDate = substr( $thisEvent->field_dw_date_range['und'][0]['value'], 0, 10 );

        if( $thisEventTitle == $event_title && $thisEventDate == $event_date ){
            $csvDataArray = array();

            $header = array(
                'name',
                'email',
                'street',
                'city',
                'state',
                'postal'
            );

            $csvDataArray[] = $header;

            $pcps = _dw_campaigns_get_pcps_for_campaign( $thisEvent );

            foreach( $pcps as $id => $pcp ){
                $supporters = dw_campaigns_pcp_get_supporters($pcp);

                foreach( $supporters as $supporter ){
                    $contribution_id = $supporter->contribution_id;

                    $pcp_contact = _dw_civicrm_contact_get(array(
                            'contact_id'  => $supporter->contact_id,
                            'returnFirst' => 1
                        ));

                    $csvData = array(
                        $pcp_contact->display_name,
                        $pcp_contact->email,
                        $pcp_contact->street_address,
                        $pcp_contact->city,
                        $pcp_contact->state_province_name,
                        $pcp_contact->postal_code
                    );

                    $csvDataArray[] = $csvData;

                }

            }

            return output_csv( 'host_event_donors_for_date_'.$event_date.'.csv', $csvDataArray );
        }

    }



}

function dw_campaigns_host_event_participants_csv($event_id){

    $csvDataArray = array();

    $header = array(
        'name',
        'email',
        'street',
        'city',
        'state',
        'postal'
    );

    $csvDataArray[] = $header;


    $participants = dw_campaign_get_event_participants_by_campaign($event_id);

    foreach( $participants as $participant ){
        $participant = $participant['contact'];
        $participant_name   = $participant->display_name;
        $participant_email  = $participant->email;
        $participant_street = $participant->street_address . ' ' .$participant->supplemental_address_1;
        $participant_city   = $participant->city;
        $participant_state  = $participant->state_province_name;
        $participant_postal = $participant->postal_code;


        $csvData = array(
            $participant_name,
            $participant_email,
            $participant_street,
            $participant_city,
            $participant_state,
            $participant_postal
        );

        $csvDataArray[] = $csvData;

        return output_csv( 'host_event_participants'.date('Y_m_d').'.csv', $csvDataArray );
    }

}

function dw_campaigns_user_host_page_form_validate($form, &$form_state) {
    $values = $form_state['values'];

    if($values['event_id'] == -1) {
        form_set_error('event_id',t('You must select an event to manage!'));
    }
}

function dw_campaigns_user_host_page_form_submit($form, &$form_state) {
    $values = $form_state['values'];

    drupal_goto('dw/user/host/' . $values['event_id']);
}

function dw_campaigns_user_host_manage_page($event_id) {
    global $user;

    $manage_form  = drupal_get_form('dw_campaigns_user_host_manage_page_form', $event_id);

    //$request_form = drupal_get_form('dw_campaigns_user_host_request_services_form', $event_id);
    $request_form = array();

    //$reports_form = drupal_get_form('dw_campaigns_user_host_reports_form', $event_id);
$reports_form = array();
    $totals       = _dw_campaigns_campaign_total($event_id);

    $totals_form  = drupal_get_form('dw_campaigns_user_host_totals_form', $event_id, $totals);

    $fundraisers_form = drupal_get_form('dw_campaigns_user_host_fundraisers_form', $event_id);

    $document_form = drupal_get_form('dw_campaigns_user_host_manage_files_form', $event_id);

    $event_gallery_form = drupal_get_form( 'flickr_event_gallery_form', $event_id );

    return theme('dw_campaigns_user_host_manage_page_theme', array('manage_form' => $manage_form, 'totals_form' => $totals_form, 'request_form' => $request_form, 'reports_form' => $reports_form, 'totals' => $totals, 'event_id' => $event_id, 'fundraisers_form' => $fundraisers_form, 'event_gallery_form' => $event_gallery_form));
}

function dw_campaigns_user_host_totals_form($form, &$form_state, $event_id, $totals) {

    $form = array();

    $campaign = node_load($event_id);

    $form['event_statistics'] = array(
        '#type'                 => 'fieldset',
        '#title'                => 'Event Statistics',
        '#collapsible'          => TRUE,
        '#collapsed'            => FALSE
    );

    $headings = array(
        t('Total Offline Donations'),
        t('Total Online Donations'),
        t('# Personal Pages'),
        t('# Donations')
    );

    $values = array(
        dw_campaigns_force_decimal($totals['offline'], $campaign->field_dw_currency['und']['0']['value']),
        dw_campaigns_force_decimal($totals['online'], $campaign->field_dw_currency['und']['0']['value']),
        $totals['pcp_count'],
        $totals['count']
    );

    $headers    = '<tr><th>' . implode('</th><th>', $headings) . '</th></tr>';
    $row        = '<tr><td>' . implode('</td><td>', $values) . '</td></tr>';

    $form['event_statistics']['markup'] = array(
        '#type'                 => 'markup',
        '#markup'               => '<table>' . $headers . $row . '</table>'
    );


    return $form;
}

function dw_campaigns_user_host_fundraisers_form($form, &$form_state, $event_id) {

    $form = array();

    $campaign = node_load($event_id);

    $form['event_fundraisers'] = array(
        '#type'                 => 'fieldset',
        '#title'                => 'Event Fundraisers',
        '#collapsible'          => FALSE,
        '#collapsed'            => FALSE
    );

    $headings = array(
        t('Name'),
        t('Page Link'),
        t('Manage Offline Donations')
    );
    
    $res = _dw_campaigns_get_pcps_for_campaign($campaign);


    $headers    = '<tr><th>' . implode('</th><th>', $headings) . '</th></tr>';

    $row = '';

    //create an array of pcps with a key of each element set as the host's last name
    $list = array();

    foreach($res as $pcp) {
        $params = array(
            'contact_id'    => $pcp->contact_id,
            'returnFirst'   => true
        );

        $contact = _dw_civicrm_contact_get( $params );

        $contact_last_name = $contact->last_name;
        $contact_first_name = $contact->first_name;

        $list[ $contact_last_name . $contact_first_name ] = $pcp;

    }

    ksort( $list );

    foreach($list as $pcp) {
        $drupal_id      = _dw_campaigns_contact_id_get_user($pcp->contact_id);
        $fake_user      = dw_campaigns_cache_simple_user_data($drupal_id);
        $url            = dw_campaigns_user_get_pcp_url($fake_user, $campaign);

        $params = array(
            'contact_id'    => $pcp->contact_id,
            'returnFirst'   => 1
        );

        $contact = _dw_civicrm_contact_get($params);

        //We were asked to display this pcp title with the name first, followed by location, some pcp titles end with ' Page', so for completeness sake, we'll strip off that part if it exists on this title.
        //also, our example showed us {{name}}, OSS, {{location}}, with OSS replacing the default of "One SMALL Step" so we'll shorten that string down as well, if it exists.
        $pcp_title_display = str_replace(' Page', ' ', $contact->display_name.', '.$pcp->title );
        $pcp_title_display = str_replace( 'One SMALL Step', 'OSS', $pcp_title_display );


        $values = array(
            $pcp_title_display,
            "<a href=\"{$url}\">" . $contact->display_name . "</a>", 
            "<a href=\"/dw/user/host/{$event_id}/users/{$pcp->id}\">" . t('Manage Offline Donations') . "</a>"
        );

        $row        .= '<tr><td>' . implode('</td><td>', $values) . '</td></tr>';
    }

    $form['event_fundraisers']['markup'] = array(
        '#type'                 => 'markup',
        '#markup'               => '<table>' . $headers . $row . '</table>'
    );


    return $form;
}

function dw_campaigns_user_host_manage_page_form($form, &$form_state, $event_id) {

    $node_mapping = array(
        'location-city'         => 'node|field_dw_campaign_location',
        'fundraising-goal'      => 'node|field_dw_fundraising_goal',
        'fundraising-currency'  => 'node|field_dw_currency',
        'date'                  => 'node|field_dw_date_range',
        'event-date'            => 'node|field_dw_event_date',
        'event-time'            => 'node|field_dw_event_time',
        'event-location'        => 'node|field_dw_event_location',
        'event-address'         => 'node|field_dw_address_street',
        'event-city'            => 'node|field_dw_address_city',
        'event-state'           => 'node|field_dw_address_province',
        'event-postal_code'     => 'node|field_dw_address_postal_code',
        'event-description'     => 'node|body',
        'event-details'         => 'node_event|body',
        'event-prizes'          => 'node_event|field_dw_eventdetails_prizes',
    );


    $node           = node_load($event_id);

    $node_event     = dw_campaigns_create_extended_event($node->nid);

    $form   = array();

    $form['#attributes']    = array(
        'class'             => array('dw_campaigns-user-host-form'),
        'enctype'           => "multipart/form-data"
    );

    $fid    = NULL;

// these are all needed for the "image"
    // lookup picture
    $temp_filename='';
    $temp_file=_dw_campaigns_host_manage_upload_photo();

    if(isset($temp_file->filepath)) {
        $temp_filename=$temp_file->filepath;
    }

    $image_params = array(
        'w'                 => 692,
        'src'               => $temp_filename
    );

    $matched_image = '';

    $theme_type     = dw_campaigns_get_selected_type();
    $no_image       = 'sites/all/themes/dw_campaigns_' . $theme_type . '/images/no-image.gif';
    
    $found=false;

    //check if we are in the process of uploading a user photo
    if(isset($image_params['src']) && $image_params['src']) {
        $imageSrc  = $image_params['src'];

        if(file_exists($imageSrc)) {
            $found  = TRUE;
            $fid    = $temp_file->fid;
        }
    } else {
        $temp   = isset($node->field_dw_campaign_image['und']['0']['uri']) ? $node->field_dw_campaign_image['und']['0']['uri'] : NULL;
        $fid    = isset($node->field_dw_campaign_image['und']['0']['fid']) ? $node->field_dw_campaign_image['und']['0']['fid'] : NULL;
        if(!is_null($temp)) {
            $doc_root   = $_SERVER['DOCUMENT_ROOT'] . base_path();

            $relative   = variable_get('file_public_path', conf_path() . '/files/');
            $imageSrc   = $relative . '/' . str_replace("public://", '', $temp);

            if(file_exists($doc_root . $imageSrc)) {
                $found  = TRUE;
            }
        }
    }

    if(!$found) {
        $imageSrc = $no_image;
    }



    {

        $form['event_details'] = array(
            '#type'             => 'fieldset',
            '#title'            => t('Event Details'),
            '#collapsible'      => FALSE,
            '#collapsed'        => FALSE
        );


        $form['event_details']['user-photo-tmp'] = array(
            '#type'             => 'hidden',
            '#value'            => $temp_filename
        );

        $form['event_details']['event-fid'] = array(
            '#type'             => 'hidden',
            '#value'            => $fid
        );


        $form['event_details']['event-picture-preview'] = array(
            '#type'             => 'markup',
            '#markup'           => '<div id="event-image-wrapper"><img style="width:692px" src="/' . $imageSrc . '" /></div><div style="clear:both"></div><div class="dimensions">692px x 356px</div>'
        );
// FIXME - this entire form should be done differently
        $form['event_details']['field_dw_campaign_image_und_0']    = array(
            '#type'             => 'file', // FIXME - make this a managed file - requires adding in a file_usage_add - https://drupal.org/node/979158
            '#title'            => t('Photo'),
            '#attributes'       => array(
                'size'          => ''
            ), 
        );
    }

    $fake           = array();
    $fake2          = array();

    $temp_form      = dw_campaigns_host_event_create_form($fake, $fake2, FALSE, TRUE);
    foreach($temp_form['event-information'] as $key => $value) {

        if(is_array($value)) {

            if(isset($value['#description'])) {
                unset($value['#description']);
            }

            if(isset($node_mapping[$key])) {

                list($type, $field) = explode('|', $node_mapping[$key]);

                $part  = $$type->$field;
                if(is_null($part)) {
                    $node_value = NULL;
                } else {
                    $node_value = isset($part['und']['0']['value']) ? $part['und']['0']['value'] : NULL;
                        if($key == 'date') {
                        $node_value = substr($node_value, 0, 10);
                    }
                }

                if(!is_null($node_value)) {
                    $value['#default_value'] = $node_value;
                }
            }

            $form['event_details'][$key] = $value;

            if($key == 'fundraising-currency' && ($value['#default_value'] == 'MULTI' || 1==1)) {
                $form['event_details']['thermometer'] = $value;
                $form['event_details']['thermometer']['#title']  = 'Thermometer Currency';
                $form['event_details']['thermometer']['#default_value'] = isset($node->field_dw_therm_currency['und']['0']['value']) ? $node->field_dw_therm_currency['und']['0']['value'] : 'USD';
            }
        }
    }

    $form['event_details']['fundraising-goal']['#description'] = 'Values will be rounded to nearest dollar';

    global $accepted_currencies;

    if(!isset($form['event_details']['thermometer'])) {
        $form['event_details']['thermometer'] = array(
            '#type'             => 'select',
            '#title'            => 'Thermometer Currency',
            '#options'          => $accepted_currencies,
            '#default_value'    => isset($node->field_dw_therm_currency['und']['0']['value']) ? $node->field_dw_therm_currency['und']['0']['value'] : 'USD'
        );
    }

    $form['event_details']['submit']         = array(
        '#type'             => 'submit',
        '#value'            => t('Submit'),
        '#attributes'       => array('class'=> array('btn'))
    );

    $form['node']           = array(
        '#type'             => 'hidden',
        '#value'            => $node->nid
    );

    $form['node_extended']  = array(
        '#type'             => 'hidden',
        '#value'            => $node_event->nid
    );


    return $form;
}

function _dw_campaigns_host_manage_upload_photo() {

    //if user failed validating the form we may have a hidden field holding the image
    if(isset($_POST['user-photo-tmp']) && $_POST['user-photo-tmp'] && empty($_FILES['files']['name']['user-photo']) ) {
        $file = new stdClass();
        $file->fid      = $_POST['event-fid'];
        $file->filepath = $_POST['user-photo-tmp'];
        return $file;
    }

    //this is a fresh file upload
    if(isset($_FILES['files']['size']['field_dw_campaign_image_und_0']) && $_FILES['files']['size']['field_dw_campaign_image_und_0'] > 0) {

        $validators = array(
            'file_validate_extensions' => array('jpg jpeg gif png')
        );


        $uploadDir = "public://derby_campaigns";

        $file       = file_save_upload('field_dw_campaign_image_und_0', $validators, $uploadDir);

        // this is critical - otherwise they'll be deleted when the cron runs
        $file->status |= FILE_STATUS_PERMANENT;
        file_save($file);

        //an error occured
        if($file->status == 0) {
            echo "we failed on the file!";die;
            return false;
        }

        // d7 work around
        $temp_filename = str_replace("public://", "sites/default/files/", $file->destination);
        $file->filepath = $temp_filename;

        return $file;

    }


    return false;

}

function dw_campaigns_user_host_manage_page_form_validate(&$form, &$form_state) {

}

function dw_campaigns_user_host_manage_page_form_submit(&$form, &$form_state) {

    $values = $form_state['values'];

    $node   = node_load($values['node']);
    $node   = dw_campaigns_host_event_set_variables($node, $values);

    $old_fid = isset($node->field_dw_campaign_image['und']['0']['fid']) ? $node->field_dw_campaign_image['und']['0']['fid'] : NULL;

    // only update if the file id is different
    if(isset($values['event-fid']) && !empty($values['event-fid']) && $old_fid != $values['event-fid']) {
        $node->field_dw_campaign_image['und']['0'] = array('fid' => $values['event-fid']);
    }

    node_save($node);


    $node_event = node_load($values['node_extended']);
    $node_event->field_dw_eventdetails_prizes['und']['0']['value'] = isset($values['event-prizes']) ? $values['event-prizes'] : '';
    $node_event->body['und']['0']['value']                         = isset($values['event-details']) ? $values['event-details'] : '';
    node_save($node_event);

    drupal_set_message(t('Your Event details have been changed'));

}

function dw_campaigns_verify_campaign_matches_pcp_id($event_id, $pcp_id) {

    $pcp                = dw_campaigns_user_get_pcp_by_id($pcp_id);
    $dummy              = new stdClass;
    $dummy->id          = $pcp->contribution_page_id;

    $campaign           = dw_campaigns_get_campaign_from_contribution_page($dummy);

    if(is_null($campaign->nid)) {
        drupal_set_message(t('Could not determine campaign from Fundraising Page - Contact support'), 'error');
        drupal_goto('dw');
        return;
    }

    if($event_id != $campaign->nid) {
        drupal_set_message(t('Campaign / Fundraising Page mismatch - Contact support'), 'error');
        drupal_goto('dw');
        return;
    }

    return array($pcp, $campaign);
}

function dw_campaigns_user_host_manage_users_page($event_id, $pcp_id = NULL) {

    if(is_null($pcp_id)) {
        // show a list of pcps?
        // for now redirect them to host page
        drupal_goto('dw/user/host/' . $event_id);
    } else {

        list($pcp, $campaign) = dw_campaigns_verify_campaign_matches_pcp_id($event_id, $pcp_id);

        $perms       = dw_campaigns_host_permission_check($campaign->nid);
        if(!$perms) {
            drupal_set_message(t('You do not have permission to view this resource'), 'error');
            drupal_goto('dw');
            return;
        }

        $owner_user_id  = _dw_campaigns_contact_id_get_user($pcp->contact_id);
        $owner_user     = user_load($owner_user_id);

        return theme('dw_campaigns_user_donations_page_theme', array('thisUser' => $owner_user, 'campaign' => $campaign));
    }


}

function dw_campaigns_user_history_page() {
    global $user;
    $pcps       = array();

    $campaigns  = array();

    $history    = dw_campaigns_user_get_historical_pcps($user);
    $keys       = array_keys($history);

    if(count($history) == 1) {
        drupal_goto('dw/user/history/' . $keys[0]);
    }

    foreach($history as $pcp) {
        $pcps[$pcp->id] = $pcp->title;
    }

    $form = drupal_get_form('dw_campaigns_user_history_page_form', $pcps);

    return theme('dw_campaigns_user_history_page_theme', array('form' => $form));
}

function dw_campaigns_user_history_page_form($form, &$form_state, $pcps) {

    $form = array();
    $form['#attributes']    = array(
        'class'             => array('dw_campaigns-user-history-form'),
        'enctype'           => "multipart/form-data"
    );
  
    $pcp_options = array('-1' => 'Pick a Fundraising Page') + $pcps;
 
    $form['pcp_id'] = array(
        '#type'         => 'select',
        '#title'        => t('Fundraising Page To View'),
        '#options'      => $pcp_options
    );

    $form['submit']         = array(
        '#type'             => 'submit',
        '#value'            => t('Submit'),
        '#attributes'       => array('class'=> array('btn'))
    );

    return $form;
}

function dw_campaigns_user_history_page_form_validate($form, &$form_state) {
    $values = $form_state['values'];

    if($values['pcp_id'] == -1) {
        form_set_error('pcp_id',t('You must select a Fundraising Page!'));
    }
}

function dw_campaigns_user_history_page_form_submit($form, &$form_state) {
    $values = $form_state['values'];

    drupal_goto('dw/user/history/' . $values['pcp_id']);
}

function dw_campaigns_user_history_manage_page($pcp_id) {
    global $user;


    $pcp                    = dw_campaigns_user_get_pcp_by_id($pcp_id);
    $contribution_page_id   = $pcp->contribution_page_id;

    $campaigns  = dw_campaigns_get_all_campaigns(TRUE);
    $campaign   = $campaigns[$contribution_page_id];

    return theme('dw_campaigns_user_history_manage_page_theme', array('campaign' => $campaign, 'pcp' => $pcp));
}

function dw_campaigns_create_extended_event($event_id) {

    $node_event_id  = db_query('select entity_id from {field_data_field_dw_eventdetails_node} where field_dw_eventdetails_node_value = :nid', array(':nid' => $event_id))->fetchColumn();

    if(!$node_event_id) {
        $node_event             = new stdClass();
        $node_event->type       = 'dw_campaigns_event_page';
        node_object_prepare($node_event);
        $node_event->is_new     = true;

        $node_event->language   = LANGUAGE_NONE;
        $node_event->title      = 'temp' . microtime(TRUE);
        $node_event->field_dw_eventdetails_node['und']['0']['value']   = $event_id;

        node_save($node_event);
    } else {
        $node_event = node_load($node_event_id);
    }

    return $node_event;
}

function dw_campaigns_user_host_request_services_form($form, &$form_state, $event_id) {

    $form = array();

    return $form;



    //JFN - request for removal/replacement, delete below this line sometime in the near future (september 30)
    $values = dw_campaigns_get_host_request_data($event_id);

    $form   = array();

    $form['request_services'] = array(
        '#type'             => 'fieldset',
        '#title'            => t('Request Services'),
        '#collapsible'      => FALSE,
        '#collapsed'        => FALSE,
    );


    $form['insurance'] = array(
        '#type'             => 'fieldset',
        '#title'            => t('Event Insurance'),
        '#collapsible'      => TRUE,
        '#collapsed'        => TRUE
    );

    $form['insurance']['ins_location'] = array(
        '#type'             => 'textfield',
        '#title'            => t('Name of Park or Location'),
        '#default_value'    => $values['ins_location']
    );
    $form['insurance']['ins_ai_name'] = array(
        '#type'             => 'textfield',
        '#title'            => t('Name of Additional Insured'),
        '#default_value'    => $values['ins_ai_name']
    );
    $form['insurance']['ins_ai_address'] = array(
        '#type'             => 'textarea',
        '#title'            => t('Address of Additional Insured'),
        '#default_value'    => $values['ins_ai_address']
    );
    $form['insurance']['ins_event_date'] = array(
        '#type'             => 'date_popup',
        '#date_format'      => 'Y-m-d',
        '#title'            => t('Date of Event'),
        '#description'      => t('Strict date, in format "YYYY-MM-DD" (year month day)'),
        '#default_value'    => $values['ins_event_date']
    );

 
    $form['shirts'] = array(
        '#type'             => 'fieldset',
        '#title'            => t('T-Shirts'),
        '#collapsible'      => TRUE,
        '#collapsed'        => TRUE
    );

    $form['shirts']['shirt_event_date'] = array(
        '#type'             => 'date_popup',
        '#date_format'      => 'Y-m-d',
        '#title'            => t('Date of Event'),
        '#description'      => t('Strict date, in format "YYYY-MM-DD" (year month day)'),
        '#default_value'    => $values['shirt_event_date']
    );

    $form['shirts']['shirt_latest_delivery_date'] = array(
        '#type'             => 'date_popup',
        '#date_format'      => 'Y-m-d',
        '#title'            => t('Lastest date for delivery'),
        '#description'      => t('Strict date, in format "YYYY-MM-DD" (year month day)'),
        '#default_value'    => $values['shirt_latest_delivery_date']
    );


    $form['shirts']['shirt_quantity'] = array(
        '#type'             => 'textfield',
        '#title'            => t('# of Shirts'),
        '#default_value'    => $values['shirt_quantity']
    );

    $form['shirts']['shirt_special_requests'] = array(
        '#type'             => 'textarea',
        '#title'            => t('Special Size Requests'),
        '#default_value'    => $values['shirt_special_requests']
    );

    $form['shirts']['shirt_shipping_address'] = array(
        '#type'             => 'textarea',
        '#title'            => t('Shipping Address'),
        '#default_value'    => $values['shirt_shipping_address']
    );

    $form['request_services']['insurance'] = $form['insurance'];
    unset($form['insurance']);

    $form['request_services']['shirts'] = $form['shirts'];
    unset($form['shirts']);

/*
    $form['request_services']['literature'] = array(  
        '#type' => 'checkboxes',  
        '#options' => array(
            'lit_custom_brochure'   => t('Customized OSS Brochure'),
            'lit_custom_flier'      => t('Customized OSS Flier'),
            'lit_fpwr_brochure'     => t('FPWR Brochure'),
        ),
        '#default_value' => array('custom_brochure'),
        '#title' => t('Literature'),
        '#description' => t('Host will receive customized items to print, FPWR brochure will be mailed to the hosts address')
    );
*/
    $form['request_services']['lit_custom_brochure'] = array(  
        '#type'             => 'checkbox',
        '#title'            =>  t('Customized OSS Brochure'),
        '#default_value'    => $values['lit_custom_brochure'],
        '#attributes'       => array('class'=> array('chkqty'))
    );

    $form['request_services']['lit_custom_brochure_qty'] = array(  
        '#type'             => 'textfield',
        '#title'            =>  t('Qty'),
        '#default_value'    => $values['lit_custom_brochure_qty'],
        '#attributes'       => array('class'=> array('qty'))
    );

    $form['request_services']['lit_custom_flier'] = array(  
        '#type'             => 'checkbox',
        '#title'            =>  t('Customized OSS Flier'),
        '#default_value'    => $values['lit_custom_flier'],
        '#attributes'       => array('class'=> array('chkqty'))
    );

    $form['request_services']['lit_custom_flier_qty'] = array(  
        '#type'             => 'textfield',
        '#title'            =>  t('Qty'),
        '#default_value'    => $values['lit_custom_flier_qty'],
        '#attributes'       => array('class'=> array('qty'))
    );

    $form['request_services']['lit_fpwr_brochure'] = array(  
        '#type'             => 'checkbox',
        '#title'            =>  t('FPWR Brochure'),
        '#default_value'    => $values['lit_fpwr_brochure'],
        '#attributes'       => array('class'=> array('chkqty'))
    );

    $form['request_services']['lit_fpwr_brochure_qty'] = array(  
        '#type'             => 'textfield',
        '#title'            =>  t('Qty'),
        '#default_value'    => $values['lit_fpwr_brochure_qty'],
        '#attributes'       => array('class'=> array('qty'))
    );


    $form['request_services']['submit']         = array(
        '#type'             => 'submit',
        '#value'            => t('Submit'),
        '#attributes'       => array('class'=> array('btn'))
    );

    $form['nid']           = array(
        '#type'             => 'hidden',
        '#value'            => $event_id
    );

    return $form;
}

function dw_campaigns_user_host_request_services_form_validate(&$form, &$form_state) {
    $form_state['values']['shirt_quantity'] = intval($form_state['values']['shirt_quantity']);
}

function dw_campaigns_user_host_request_services_form_submit(&$form, &$form_state) {
    global $user;

    $values = $form_state['values'];

    $possible_values = dw_campaigns_user_host_request_defaults();

    foreach($possible_values as $key => $junk) {
        if(isset($values[$key])) {
            $possible_values[$key] = $values[$key];
        }
    }

    db_merge('dw_campaigns_request_services')->key(array('nid' => $values['nid']))->fields($possible_values)->execute();

    $node = node_load($values['nid']);

    $contactId  = _dw_campaigns_user_get_contact_id($user);

    $params = array(
        'contact_id'    => $contactId,
        'returnFirst'   => 1
    );
    $contact        = _dw_civicrm_contact_get($params);

    $whom = array(
        'Location'   => $node->title,
        'Host Name'  => $contact->display_name,
        'Host Email' => $contact->email
    );

    $email_values = $whom + $possible_values;
// MARK
    $from = variable_get('dw_campaigns_host_request_services_email', '');
    drupal_mail('dw_campaigns_host_request_services', 'request_services', $from,  NULL, $email_values);

}

function dw_campaigns_host_request_services_mail($key, &$message, $params) {

    $mapping    = array(
        'ins'   => 'Insurance',
        'lit'   => 'Literature',
        'shirt' => 'Shirts',
    );

    $key_mapping = array(
        'ins_location'   => 'Walk Location',
        'ins_ai_name'    => 'Name Additional Insured',
        'ins_ai_address' => 'Address Additional Insured',
        'ins_event_date' => 'Event Date',
    );

    $body               = '';
    $last_prefix        = '';

    foreach($params as $k => $value) {

        list($prefix, $junk) = explode('_', $k, 2);

        if($prefix == 'nid') {
            continue;
        }


        if($prefix!=$last_prefix) {
            $heading = isset($mapping[$prefix]) ? $mapping[$prefix] : $prefix;

            $last_prefix = $prefix;
           
            $body .= "\n--- $heading ---\n";
        } 

        $label = isset($key_mapping[$k]) ? $key_mapping[$k] : $k;
        $body .= "$label  :  $value\n";

    }

    switch($key) {   
        case 'request_services':
            $subject            = 'Event Request Services';
            $message['subject'] = $subject;
            $message['body'][]  = $body;
            break;
    }
}

function dw_campaigns_user_host_request_defaults() {
    return array( 
        'ins_location'                  => '', 
        'ins_ai_name'                   => '', 
        'ins_ai_address'                => '', 
        'ins_event_date'                => '', 
        'shirt_event_date'              => '', 
        'shirt_latest_delivery_date'    => '', 
        'shirt_quantity'                => '0', 
        'shirt_special_requests'        => '', 
        'shirt_shipping_address'        => '', 
        'lit_custom_brochure'           => '0', 
        'lit_custom_brochure_qty'       => '0', 
        'lit_custom_flier'              => '0', 
        'lit_custom_flier_qty'          => '0', 
        'lit_fpwr_brochure'             => '0', 
        'lit_fpwr_brochure_qty'         => '0', 
        'nid'                           => -1,        
    );  
}

function dw_campaigns_get_host_request_data($event_id) {
    $row = db_query('select * from {dw_campaigns_request_services} where nid = :nid', array(':nid' => $event_id))->fetchAssoc();

    if(!$row) {
        return dw_campaigns_user_host_request_defaults();
    }
    return $row;
}


function dw_campaigns_user_host_reports_form($form, &$form_state, $event_id) {

    $form = array();

    $form['reports'] = array(
        '#type'             => 'fieldset',
        '#title'            => t('Reports'),
        '#collapsible'      => TRUE,
        '#collapsed'        => FALSE,
    );

    $form['reports']['report_type'] = array(
        '#type'         => 'select',
        '#title'        => t('Report Type'),
        '#options'      => array(
            'registration'          => t('Registration'), 
            'participant_contacts'  => t('Participant Contacts'), 
            'donations'             => t('Donations'), 
        )
    );

    $form['reports']['report_action'] = array(
        '#type'         => 'select',
        '#title'        => t('Action'),
        '#options'      => array(
            'print' => t('Print'), 
            'view'  => t('View'), 
            'csv'   => t('Download CSV'), 
        )
    );

    $form['reports']['submit']         = array(
        '#type'             => 'submit',
        '#value'            => t('Submit'),
        '#attributes'       => array('class'=> array('btn'))
    );

    $form['nid']           = array(
        '#type'             => 'hidden',
        '#value'            => $event_id
    );

    return $form;
}

function dw_campaigns_user_host_reports_form_validate(&$form, &$form_state) {

}

function dw_campaigns_user_host_reports_form_submit(&$form, &$form_state) {

}

function dw_campaigns_event_countries_get() {
   static $countries    = NULL;

   if(is_null($countries)) {
       $countries       = array();

       $results = db_query("select * from {field_data_field_dw_country_active} where field_dw_country_active_value = 'enabled'");
    
       foreach($results as $result) {
    
           $node                    = node_load($result->entity_id);
           $short                   = $node->field_dw_country_short_name['und']['0']['value'];

           $countries[$short]       = $node;
       } 
   }

   return $countries;
}

// FIXME - we should document that this "other" is always an option even if a country group does not exist 
function dw_campaigns_event_countries_get_options() {
    $options    = array();
    $have_other = FALSE;

    $raw        = dw_campaigns_event_countries_get();

    foreach($raw as $short => $country) {
        if($short == 'other') {
            $have_other     = TRUE;
        }

        $options[$short]    = $country->title;
    }

    if(!$have_other) {
        $options['other']   = 'Other'; 
    }

    return $options;
}

function dw_campaigns_event_countries_resolve_selection($selection) {

    $raw        = dw_campaigns_event_countries_get();

    /* 
       if our selection doesn't match anything, we will use the first record
       the assumption being that its our "fake" other
    */ 
    if(!isset($raw[$selection])) {
        if(count($raw) < 1) {
           $row     = NULL;
        } else {
            $row    = array_shift($raw);
        }
    } else {
        $row    = $raw[$selection];
    }

    return $row;
}

function dw_campaigns_get_event_country_by_campaign($campaign) {
    $campaign   = is_object($campaign) ? $campaign : node_load($campaign);

    $countries  = dw_campaigns_event_countries_get();

    $country    = isset($campaign->field_dw_country_grouping['und']['0']['value']) ? $campaign->field_dw_country_grouping['und']['0']['value'] : 'us';

    return isset($countries[$country]) ? $countries[$country] : NULL;
}

function dw_campaigns_add_address_book($user, $email, $name) {

    $res       = NULL;


    $addresses = __dw_campaigns_Address_book_cache($user);

    if(!isset($addresses[$email])) {

        try {
            $res = db_insert('dw_campaigns_address_book')->fields(array('owner_uid' => $user->uid, 'email' => $email, 'name' => $name))->execute();
        } catch (Exception $e) {
            // I don't know what to do
        }

        __dw_campaigns_Address_book_cache($user, $email);

    } elseif($addresses[$email] == 1) {
        db_update('dw_campaigns_address_book')->fields(array('hidden' => 0))->condition('owner_uid', $user->uid, '=')->condition('email', $email, '=')->execute();

       $res = db_query("select address_id from {dw_campaigns_address_book} where owner_uid = :owner_uid and email = :email", array(':owner_uid' => $user->uid, ':email' => $email))->fetchColumn();

    }
    return $res;
}

function __dw_campaigns_address_book_cache($user, $add = FALSE) {
    static $emails      = array();
    static $by_email    = array();


    if(!isset($emails[$user->uid])) { 
        $results    = db_query("select email,hidden from {dw_campaigns_address_book} where owner_uid = :owner_uid", array(':owner_uid' => $user->uid));
    
        foreach($results as $result) {
            $by_email[$result->email]       = $result->hidden;
        }
 
        $emails[$user->uid] = $by_email;
    }

    if($add != FALSE) {
        $emails[$user->uid][$add] = $by_email;
    }    

    return $by_email; 
}

function dw_campaigns_get_address_book($user, $pcp_id, $index_by_email = FALSE, $hide_hidden = TRUE) {
    $addresses  = array();
    $by_email   = array();

    if($hide_hidden) {
        $hide_option = 'a.hidden = 0 and  ';
    }

    if(is_object($pcp_id) && isset($pcp_id->id)) {
        // should warn them
        $pcp_id = $pcp_id->id;
    }

    $table  = 'dw_campaigns_address_book';

    if(!db_table_exists($table)) {
        return $addresses;
    }

    $results    = db_query("select a.*, b.invite_dates, b.pcp_id from {$table} a left join {dw_campaigns_address_book_status} b on a.address_id = b.address_id where $hide_option a.owner_uid = :owner_uid and (b.pcp_id is NULL OR pcp_id=:pcp_id)", array(':owner_uid' => $user->uid, ':pcp_id' => $pcp_id));

    foreach($results as $result) {
        $addresses[$result->address_id] = $result;
        $by_email[$result->email]       = $result;
    }
    if($index_by_email) {
        return $by_email; 
    } else { 
        return $addresses; 
    }
}

function dw_campaigns_get_address_by_id($id) {

    $result = db_query("select a.*, b.invite_dates, b.pcp_id from {dw_campaigns_address_book} a left join {dw_campaigns_address_book_status} b on a.address_id = b.address_id where a.address_id = :address_id", array(':address_id' => $id))->fetchObject();

    return $result;
}

function dw_campaigns_set_address_book_status($id, $pcp_id, $new_date) {

    // db_merge is an option, but I'm guessing I'll need to add more into this later so I'll do it the long ways

    $result = db_query("select * from {dw_campaigns_address_book_status} where address_id = :address_id and pcp_id = :pcp_id", array(':address_id' => $id, ':pcp_id' => $pcp_id))->fetchObject();

    if(!$result) {

        try {
            db_query("insert into {dw_campaigns_address_book_status} (address_id, pcp_id, invite_dates) VALUES(:address_id, :pcp_id, :new_date)", array(':address_id' => $id, ':pcp_id' => $pcp_id, ':new_date' => $new_date));
        } catch (Exception $e) {
            return;
        }

        return;
    }

    $dates   = explode(",", $result->invite_dates);
    $dates[] = $new_date;

    $dates   = array_unique($dates);
    $date_l  = implode(',', $dates);

    db_update('dw_campaigns_address_book_status')->fields(array('invite_dates' => $date_l))->condition('address_id', $id, '=')->condition('pcp_id', $pcp_id, '=')->execute();

    return;
}

function dw_campaigns_get_thankyous($pcp_id) {

    $thankyous  = array();

    $table  = 'dw_campaigns_thankyou';

    if(!db_table_exists($table)) {
        return $thankyous;
    }

    $result     = db_query("select * from {$table} where pcp_id = :pcp_id", array(':pcp_id' => $pcp_id));
    
    foreach($result as $ty) {
        $thankyous[$ty->contribution_id] = $ty;
    } 

    return $thankyous;
}

function dw_campaigns_thankyou_page($target_id, $contribution_id) {
    return drupal_get_form('dw_campaigns_thankyou_form', $target_id, $contribution_id);
}

function dw_campaigns_thankyou_validate_params($target_id, $contribution_id) {
    global $user;

    $target     = dw_campaigns_get_address_by_id($target_id);

    $mode_type  = dw_campaigns_get_selected_type();

    if($mode_type == 'walking') {
        $pcp = dw_campaigns_user_get_pcp_for_walking($user);
    } else {
        $campaignId = dw_campaigns_get_default_campaign();

        $campaign   = node_load($campaignId);
        $pcp        = dw_campaigns_user_get_pcp_for_campaign($user, $campaign);
    }

    $supporters  = dw_campaigns_get_contributions_for_pcp($pcp, TRUE);

    if(!isset($supporters[$contribution_id])) {
        drupal_set_message('You do not have permission to send email for this contribution', 'error');
        return FALSE;
    }

    if($user->uid != $target->owner_uid) {
        drupal_set_message('You do not have permission to email this address!', 'error');
        return FALSE;
    }
/*
    if($target->status == 'thanked') {
        drupal_set_message('You have already thanked this user!', 'error');
        return FALSE;
    }
*/
    return array('target' => $target, 'pcp' => $pcp);
}

function dw_campaigns_thankyou_form($form, &$form_state, $target_id, $contribution_id) {
    global $user;

    $ret = dw_campaigns_thankyou_validate_params($target_id, $contribution_id);
    if(!$ret) {
        return;
    }

    extract($ret);
 
    $form = array();

    $form['#attributes']    = array(
        'class'             => array('dw-campaigns-thankyou-form'),
        'enctype'           => "multipart/form-data"
    );

    $form['offline'] = array(
        '#type'             => 'markup',
        '#markup'           => '<div class="offline-thanks"><a href="/dw/user/send_thankyou/' . $target_id . '/' . $contribution_id . '/thanked" onclick="confirm(\'' . t('Clicking OK will mark this person as thanked, this can not be changed') . '\')">' . t('Click here to mark this person as thanked without sending an email') . '</a></div>',
    );

    $form['thank-you'] = array(
        '#type'             => 'fieldset',
        '#title'            => t('Thank You Email')
    );

    $form['thank-you']['target'] = array(
        '#type'             => 'markup',
        '#markup'           => '<div class="target"><span class="words">To:</span><span class="to">' . $target->name . ' &lt;' . $target->email . '&gt;</span></div>',
    );

    $thankyou_title = variable_get('dw_campaigns_user_thankyou_title', 'Thank you for your donation to my campaign');
    $thankyou_text  = variable_get('dw_campaigns_user_thankyou_text', 'Thank you for your donation to my campaign');

    $form['thank-you']['subject'] = array(
        '#type'             => 'textfield',
        '#title'            => t('Subject'),
        '#required'         => TRUE,
        '#default_value'    => $thankyou_title
    );

    $form['thank-you']['text'] = array(
        '#type'             => 'text_format',
        '#format'           => variable_get('dw_campaigns_cfg_default_filter', null),
        '#title'            => t('Email Content'),
        '#required'         => TRUE,
        '#default_value'    => $thankyou_text
    );

    $form['thank-you']['to'] = array(
        '#type'             => 'hidden',
        '#value'            => serialize($target)
    );

    $form['thank-you']['contribution_id'] = array(
        '#type'             => 'hidden',
        '#value'            => $contribution_id
    );

    $form['thank-you']['pcp_id'] = array(
        '#type'             => 'hidden',
        '#value'            => $pcp->id
    );

    $form['submit']         = array(
        '#type'             => 'submit',
        '#value'            => t('Submit'),
        '#attributes'       => array('class'=> array('btn'))
    );

    return $form;
}

function dw_campaigns_thankyou_form_validate(&$form, &$form_state) {
    $values = $form_state['values'];

    $subject = trim($values['subject']);

    if(empty($subject)) {
        form_set_error('subject', t('You must supply a subject!'));
    }

    // we do this so that we can change away from a 'text_format' later and not break the page logic
    $text = is_array($values['text']) ? ( isset($values['text']['value']) ? $values['text']['value'] : '' ) : $values['text'];
    $text = trim($text);

    if(strlen($text) < 9) {
        form_set_error('text', t('A longer message is required'));
    }
}

function dw_campaigns_thankyou_form_submit(&$form, &$form_state) {
    global $user;
        
    $params = array();

    $values = $form_state['values'];

    $target = unserialize($values['to']);

    $result = db_query('select * from {dw_campaigns_thankyou} where pcp_id = :pcp_id and contribution_id = :contribution_id', array(':pcp_id' => $values['pcp_id'], ':contribution_id' => $values['contribution_id']));

    $params['from_email']   = variable_get('dw_campaigns_user_thankyou_from_email_address', 'no-reply');

    if($result->rowCount() > 0) {
        drupal_set_message(t('A thank you had already been sent for this donation'));
    } else {

        $text = is_array($values['text']) ? ( isset($values['text']['value']) ? $values['text']['value'] : '' ) : $values['text'];
        $params['text']         = $text;
        $params['subject']      = $values['subject'];
        $params['reply_to']     = $user->mail;
        $params['from_name']    = variable_get('dw_campaigns_user_thankyou_from_email_name', '');

        if(empty($paramts['from_name'])) {
            $params['from_name'] = $user->data['displayname'];
        }

        $params['from_email'] = '"'. addslashes(mime_header_encode($params['from_name'])) .'" <'. $params['from_email'] .'>';

        $res = drupal_mail('dw_campaigns_thankyou', 'thankyou', $target->email,  user_preferred_language($user), $params, $params['from_email']);

        if($res['result']) {
            db_query("insert into {dw_campaigns_thankyou} (pcp_id, contribution_id, status) VALUES(:pcp_id, :contribution_id, :status)", array(':pcp_id' => $values['pcp_id'], ':contribution_id' => $values['contribution_id'], ':status' => 'sent'));
        }
    }

    header('Location: /dw/noframe?destination=dw/user/donations');
    die;
}

function dw_campaigns_thankyou_mail($key, &$message, $params) {

    $language = $message['language'];
    switch($key)
    {   
        case 'thankyou':
            $message['headers']['Reply-To']     = $params['reply_to'];
            $message['body'][]  = $params['text'];
            $message['subject'] = $params['subject'];
            break;
    }
}

function dw_campaigns_thankyou_thanked($target_id, $contribution_id) {

    $ret = dw_campaigns_thankyou_validate_params($target_id, $contribution_id);
    if(!$ret) {
        drupal_goto('dw/user/donations');    
        return;
    }

    extract($ret);

    db_query("insert into {dw_campaigns_thankyou} (pcp_id, contribution_id, status) VALUES(:pcp_id, :contribution_id, :status)", array(':pcp_id' => $pcp->id, ':contribution_id' => $contribution_id, ':status' => 'offline'));
    
    drupal_goto('dw/user/donations');    
}

function dw_campaigns_user_addressbook_page() {
   // not sure what I'm doing with this
}

function dw_campaigns_address_book_table($selected_emails) {
    global $user;
    $rows   = array();

    $mode_type  = dw_campaigns_get_selected_type();

    $pcp        = dw_campaigns_get_selected_pcp();

    // all addresses
    $addresses  = dw_campaigns_get_address_book($user, $pcp->id, TRUE, TRUE);
    // all donations
    $supporters  = dw_campaigns_get_contributions_for_pcp($pcp, TRUE);
    // all people who have been thanked (by contribution id); 
    $thankyous  = dw_campaigns_get_thankyous($pcp->id);
    // people currently in the box at the top of the page (from an import)
    $targets    = preg_split("/[\n,;]/", $selected_emails, -1, PREG_SPLIT_NO_EMPTY);

    if(is_null($supporters)) {
        $supporters = array();
    }

    $contacts   = array();

    // I hate doing this, but we need to if we want to know the contacts emails
    foreach($supporters as $support) {
            $params = array(
                'contact_id'    => $support->contact_id,
                'returnFirst'   => 1
            );

            $contact            = _dw_civicrm_contact_get($params);
            $email              = $contact->email;
 
            $contacts[$email]   = $contact;
    }

    $headers        = array(
        array(
            'data'  => t('Select') . '<br><input type="checkbox" class="select-all">',
        ),
        array(
            'data'  => t('Contact'),
        ),
        array(
            'data'  => t('Contact History'),
        ),
        array(
            'data'  => t('Donated'),
        ),
        array(
            'data'  => t('Actions'),
        )
    );

    foreach($addresses as $address) {
        if($address->name != $address->email && !empty($address->name)) {
            $contact_name = '<span class="name">' . $address->name . '</span><span class="email">&lt;' . $address->email . '&gt;</span>';
        } else {
            $contact_name = '<span class="email-only">' . $address->email . '</span>';
        }


        $history    = '';
        if(strlen($address->invite_dates) == 0) { 
            $history = 'N/A';
        } else {
            $dates = explode(",", $address->invite_dates);
            foreach($dates as $date)  {
                if(is_numeric($date)) {
                    $history .= '<span class="date">' . date('M d, Y', $date);
                }
            }
        }
         
        $contact    = isset($contacts[$address->email]) ? $contacts[$address->email] : new stdClass;
        $donation_l = array();

        foreach($supporters as $support) {
            if(isset($support->contact_id) && isset($contact->contact_id) && ($support->contact_id == $contact->contact_id)) {
                if(isset($support->contribution_id)) {
                    $donation_l[$support->contribution_id] = $support;
                }
            }    
        }

        if(count($donation_l) > 0) {
            $donated_box    = '<span class="did-donate"></span>';
            $donate_status  = 'did';
            $select_opt     = '';
        } else {
            $donated_box    = '<span class="did-not-donate"></span>';
            $donate_status  = 'did-not';
            $select_opt     = '<input type="checkbox" class="address-action" name="address-action" id="act-' . $address->address_id . '" value="' . $address->address_id . '">';
        }

        $actions    = '';

        if(count($donation_l) > 0) {
            $found   = FALSE;
            
            foreach($donation_l as $cont_id => $donation) {
                $actions = '';
                if(isset($thankyous[$cont_id])) {
                    $actions    .= '<div class="thank-yous-sent" title="A thank you has been sent for this donation"><span>Thank you sent</span></div>';
                    $found      = TRUE;
                } else {
                    $link       = '/dw/user/send_thankyou/' . $address->address_id . '/' . $cont_id . "?ajax=1";
                    $actions    .= '<div class="thank-yous-notsent"><span class="thank-yous-notsent" title="Send Thank You"><a href="' . $link . '" class="fb_tall thank-yous-not-sent"></a></span></div>';
                }
            }
        } else {

            $extra  = '';

            if(strlen($history)>4) {
                $extra  = ' sent-previously';
            }

            $actions = '<div class="share-with"><span class="share-with' . $extra . '"><a href="mailto:' . $address->email . '" class="add_to_share" title="Add to Message Recipients"></a></span></div>';

        }

        $rows[] = array(
            'data' => array(
                array('data' => $select_opt, 'class' => array('contact')),
                array('data' => $contact_name, 'class' => array('contact')),
                array('data' => $history, 'class' => array('history') ),
                array('data' => $donated_box, 'class' => array('donated', $donate_status)),
                array('data' => $actions, 'class' => array('action')),
            )    
        );
    }
 
    drupal_add_js('
      (function($) {
        $(document).ready(function() {
          $(".select-all").click(function() {
              var res = $(this).is(":checked");
              if(res) {
                  $(".address-action").attr("checked", "checked");
              }
          });


          $("a.act-delete").click(function() {
             if(!confirm("Are you sure you want to remove these contacts from your address book?")) {
                 return false;

             }


             var ac = $(".address-action:checked").map(function(i,n) { return $(n).val(); }).get();

             $(".address-action:checked").closest("tr").remove();             

             $(".address-book table tr").removeClass("odd");
             $(".address-book table tr").removeClass("even");

             $(".address-book tr:nth-child(odd)").addClass("odd");
             $(".address-book tr:nth-child(even)").addClass("even");



             $.post("/dw/ajax/address_remove/now" , {"address-selected[]" : ac});

          });
        });
     })(jQuery);
    ', 'inline');
 

    $button = '<div class="delete-button"><a class="btn act-delete">' . t('Remove Selected') . '</a></div>';
 
    return '<div class="address-book"><div class="header">' . t('Manage Contacts') . '</div>' .  theme('table', array('header' => $headers, 'rows' => $rows)) . $button . '</div>';
}

function dw_campaigns_donor_participation_page($username, $campaignPath) {
    $data = dw_campaigns_users_pcp_page($username, $campaignPath, true);
    extract($data);

    // array('thisUser' => $owner_user, 'campaign' => $campaign, 'pcp' => $pcp));

    $contact_id = isset($_SESSION['contact_id']) ? $_SESSION['contact_id'] : NULL;

    if(is_null($contact_id)) {
        return t('Unable to comlete this action: No contact defined.  Please contact customer support');
    }

    $form = drupal_get_form('dw_campaigns_donor_participation_form', $campaign, $pcp, $contact_id);

    return drupal_render($form);
}

function dw_campaigns_donor_participation_form($form, &$form_state, $campaign, $pcp, $contact_id) {

  //echo '<a href="/dw/test/1?ajax=1" class="fb">fancy</a>';
    $form = array();

    $campaign = !is_object($campaign) ? node_load($campaign) : $campaign;

    $event_max = variable_get('dw_campaigns_walking_event_max', '6');

    for($i=0; $i<=$event_max; $i++) {
        $attending[$i] = $i;
    }

    $form['choice'] = array(
                '#type'             => 'markup',
                '#markup'           => '<div class="participation-words">' . t('Do you plan to attend the @campaign walk?', array('@campaign' => $campaign->title)) . '</div><div class="buttons"><div class="btn yes fb-unhide">Yes</div><div class="btn no fb-close">No</div></div>'
    );

    list($children, $adults) = dw_campaigns_get_event_participants_count($campaign->nid, 0, $contact_id);

    $form['event-participants'] = array(
                '#type'             => 'fieldset',
    );

    $form['event-participants']['children'] = array(
        '#type'             => 'select',
        '#title'            => t(variable_get('dw_campaigns_walking_children_attending_prompt', 'How many children will be attending the event')),
        '#options'          => $attending,
        '#default_value'    => $children
    );

    $form['event-participants']['adults'] = array(
        '#type'             => 'select',
        '#title'            => t(variable_get('dw_campaigns_walking_adults_attending_prompt', 'How many adults will be attending the event')),
        '#options'          => $attending,
        '#default_value'    => $adults
    );

    if(variable_get('dw_campaigns_walking_children_attending_prompt_enabled', 1) == 0) {
        $form['event-participants']['children'] = array(
            '#type'             => 'hidden',
            '#value'            => 0
        );
    }

    if(variable_get('dw_campaigns_walking_adults_attending_prompt_enabled', 1) == 0) {
        $form['event-participants']['adults'] = array(
            '#type'             => 'hidden',
            '#value'            => 0
        );
    }
    
    $form['contact_id']     = array(
        '#type'             => 'hidden',
        '#value'            => $contact_id
    );

    $form['campaign_id']    = array(
        '#type'             => 'hidden',
        '#value'            => $campaign->nid
    );



    $form['event-participants']['submit']         = array(
        '#type'             => 'submit',
        '#value'            => t('Submit'),
        '#attributes'       => array('class'=> array('btn'))
    );
    return $form;
}

function dw_campaigns_donor_participation_form_validate(&$form, &$form_state){

}

function dw_campaigns_donor_participation_form_submit(&$form, &$form_state) {
    $values = $form_state['values'];

    dw_campaigns_set_event_participants_count(array('campaignId' => $values['campaign_id'], 'children' => $values['children'], 'adults' => $values['adults'], 'participation_type' => 'donator', 'contactId' => $values['contact_id']));

    if($values['children'] > 0 || $values['adults'] > 0) {
        $suffix = variable_get('dw_campaigns_cfg_tag_suffix', date('Y'));
        $name   = variable_get('dw_campaigns_cfg_tag_prefix', 'OSS') . ' participant ' . $suffix;
        $tag_id = dw_campaigns_get_tag_id_by_name($name);
        dw_campaigns_tag_contact($tag_id, array($values['contact_id']));
    }
}

function dw_campaigns_walking_event_list_page() {
    return theme('dw_campaigns_walking_event_list');
}

function dw_campaigns_noframe() {

    $target  = isset($_REQUEST['destination']) ? $_REQUEST['destination'] : '/dw/';

    // we are used to getting dw/something  so add a / so the browser doesnt get confused
    if(strstr($target, 'http://') === FALSE) {
        $target = '/' . $target;
    }

    $payload = sprintf('<script type="text/javascript"> top.location = "%s"</script>', $target);
    echo $payload;die;
}

function dw_campaign_show_on_pcp_page() {
    if(arg(1) != 'users' || !is_null(arg(4))) {
        return FALSE;
    }

    return TRUE;
}

function dw_campaigns_event_status_change_pending_enabled($node) {

    $host_id    = isset($node->field_dw_host_primary['und']['0']['value']) ? $node->field_dw_host_primary['und']['0']['value'] : NULL;
    $cohost_id  = isset($node->field_dw_host_secondary['und']['0']['value']) ? $node->field_dw_host_secondary['und']['0']['value'] : NULL;

    $send_to    = arraY();

// I wrote it like this because I'm anticipating that in the future we won't have host/cohost we'll have an array of up to 5 or 10 possible host

    if(!is_null($host_id)) {
        $send_to[]  = $host_id;
    }

    if(!is_null($cohost_id)) {
        $send_to[]  = $cohost_id;
    }

    $from       = variable_get('dw_campaigns_signup_from');
    $params     = array();

    foreach($send_to as $host_id) {
        $email  = '';
        $params = array();

        $target = node_load($host_id);
        $d_uid  = isset($target->field_dw_host_user['und']['0']['value']) ? $target->field_dw_host_user['und']['0']['value'] : NULL;
        if(!is_null($d_uid)) {
            $t_user   = user_load($d_uid);
            
            $email    = $t_user->mail;
            $language = user_preferred_language($t_user);

        } else  {
            
            $email    = isset($target->field_dw_host_email['und']['0']['value']) ? $target->field_dw_host_email['und']['0']['value'] : '';
            $language = language_default();
        }

        drupal_mail('dw_campaigns_host_event_contact', 'enabled', $email, $language, $params, $from);
    }    

}

function dw_campaigns_host_event_contact_mail($key, &$message, $params) {

    switch($key)
    {
        case 'enabled':
            $message['subject'] = variable_get('dw_campaigns_host_event_accepted_email_subject', 'Your event has been approved');

            $body               = variable_get('dw_campaigns_host_event_accepted_email', 'Your event has been accepted, please login to manage your event');

            $message['body'][]  = $body;
            break;
    }
}

function dw_campaigns_host_contact_page($host_id) {
    $_REQUEST['ajax']   = 1; 

    $host = node_load($host_id);
    if(!isset($host->type) || $host->type != 'dw_host_record') {
        echo 'Error looking up record, please contact support';
        die;
    }

    $form = drupal_get_form('dw_campaigns_host_contact_page_form', $host);

    return theme('dw_campaigns_host_contact_page', array('form' => $form, 'host' => $host));
}

function dw_campaigns_host_contact_page_form($form, &$form_state, $host) {
    $form = array();

    $form['name']           = array(
        '#type'             => 'textfield',
        '#title'            => t('Name'),
        '#required'         => true
    );
    
    $form['email']          = array(
        '#type'             => 'textfield',
        '#title'            => t('Email'),
        '#required'         => true
    );

    $form['telephone']      = array(
        '#type'             => 'textfield',
        '#title'            => t('Telephone'),
        '#required'         => false
    );

    $form['message']        = array(
/*
        '#type'             => 'text_format',
        '#format'           => variable_get('dw_campaigns_cfg_default_filter', null),
*/
        '#type'             => 'textarea',
        '#title'            => 'Message',
        '#required'         => true,
        '#rows'             => 8,
        '#attributes'       => array('class'=> array('message'))
    );
    
    $form['host_id']         = array(
        '#type'             => 'hidden',
        '#value'            => $host->nid
    );


    $form['captcha'] = array(
        '#type'             => 'captcha',
        '#captcha_type'     => 'captcha/Math',
        //'#captcha_type'     => 'captcha/recaptcha',
    );

    $form['submit']         = array(
        '#type'             => 'submit',
        '#value'            => t('Send Email'),
        '#attributes'       => array('class'=> array('btn'))
    );

    return $form;
}

function dw_campaigns_host_contact_page_form_validate($form, &$form_state) {
    $values     = $form_state['values'];

    $message    = is_array($values['message']) ? ( isset($values['message']['value']) ? $values['message']['value'] : '' ) : $values['message'];

    if(strlen($message) < 3) {
        form_set_error('message', '');
    }
}

function dw_campaigns_host_contact_page_form_submit($form, &$form_state) {
    $values     = $form_state['values'];

    $host       = node_load($values['host_id']);

    $host_email = isset($host->field_dw_host_email['und']['0']['value']) ? $host->field_dw_host_email['und']['0']['value'] : '';

    if(empty($host_email)) {
        $host_email = 'no-reply';
    }

    $message    = is_array($values['message']) ? ( isset($values['message']['value']) ? $values['message']['value'] : '' ) : $values['message'];

    $language               = language_default();
    $params                 = array();
    $params['message']      = $message;
    $params['name']         = $values['name'];
    $params['email']        = $values['email'];
    $params['telephone']    = $values['telephone'];

    drupal_mail('dw_campaigns_host_contact', 'contact-host', $host_email, $language, $params);
  
    drupal_set_message(t('Your message has been sent'));

    header('Location: /dw/noframe/?destination=' . 'dw/walking/event_list');
    die;
}

function dw_campaigns_host_contact_mail($key, &$message, $params) {

    switch($key)
    {
        case 'contact-host':
            $message['subject']             = variable_get('dw_campaigns_host_contact_subject', 'Host Contact Form Email');
            $message['headers']['reply-to']  = $params['email'];
          
            $body                           = "From: {$params['name']}\nEmail: {$params['email']}\nTelephone: {$params['telephone']}\n\n--Message Follows--\n\n";
            $body                           .= $params['message'];

            $message['body'][]              = $body;
            break;
    }
}

function dw_campaigns_mail_alter(&$message) {

    $bccs   = array();
    
    $bccs[] = "all_oss_emails@jemeznet.com";
  
// LOOK this is a work around to avoid using elasticemail for hosts they are blacklisted by 
    $bl_hosts = array(
        'comcast.net',
        'comcast.com',
        'verizon.com',
        'verizon.net',
    );

    file_put_contents("/tmp/last_email_raw", print_r($message, TRUE));

    $raw_to = $message['to'];
    $parts = explode("@", $raw_to);

    if(isset($parts[1])) {
        if(in_array($parts[1], $bl_hosts)) {
            $message['from'] = str_replace("@", "bl@", $message['from']);
        }
    }
 
    $t      = variable_get('dw_campaign_auto_bcc', '');
    $t      = trim($t);
    if(!empty($t)) {
        $bccs[] = $t;
    }

    if(!isset($message['plaintext']) || is_null($message['plaintext'])) {
        $message['plaintext'] = strip_tags($message['body']['0']);
    }

    $message['headers']['Bcc'] = implode(",", $bccs); 
}

function dw_campaigns_ajax_address_book_remove($param) {
    global $user;

    if($user->uid < 0) {
        return; 
    }

    foreach($_REQUEST['address-selected'] as $id) {
echo $id;
        $res = db_update('dw_campaigns_address_book')->fields(array('hidden' => 1))->condition('owner_uid', $user->uid, '=')->condition('address_id', $id, '=')->execute();
    }

    die;
}

function dw_campaigns_get_active_campaigns_host_list() {        
    $matches_by_campaign    = array();
    $matches                = array();

    $active_campaigns   = dw_campaigns_get_active_campaigns('nodeid');

    $keys               = array_keys($active_campaigns);
 

    $result = db_select('field_data_field_dw_host_primary', 'n')->fields('n')->condition('bundle', 'dw_campaigns_derby', '=')->condition('entity_id', $keys, 'IN')->execute();
            
    foreach($result as $row) {
        if(!isset($matches_by_campaign[$row->entity_id])) {
            $matches_by_campaign[$row->entity_id] = array();
        }
        $matches_by_campaign[$row->entity_id][] = $row->field_dw_host_primary_value;

        $matches[$row->field_dw_host_primary_value] = $row->field_dw_host_primary_value;
    }
        
    $result = db_select('field_data_field_dw_host_secondary','n')->fields('n')->condition('bundle', 'dw_campaigns_derby', '=')->condition('entity_id', $keys, 'IN')->execute();
         
    foreach($result as $row) {
        if(!isset($matches_by_campaign[$row->entity_id])) {
            $matches_by_campaign[$row->entity_id] = array();
        }
        $matches_by_campaign[$row->entity_id][] = $row->field_dw_host_secondary_value;

        $matches[$row->field_dw_host_secondary_value] = $row->field_dw_host_secondary_value;
    }

    return $matches;
}
/**
FIXME - we may want to actually replace the above one with the version that handles filtering  - leaving the old one for now
**/
function dw_campaigns_get_active_campaigns_host_list_filtered($filter) {        

    $matches_by_campaign    = array();
    $matches                = array();

    $active_campaigns   = dw_campaigns_get_active_campaigns('nodeid');
   
    $filtered_active_campaigns = array();
 
    foreach($active_campaigns as $nid => $node) {

        if($filter == 'upcoming') {
            if($node->field_dw_campaign_status['und']['0']['value'] == 'enabled') {
                $filtered_active_campaigns[$nid] = $node;
            }
        }

        if($filter == 'completed') {
            if($node->field_dw_campaign_status['und']['0']['value'] == 'completed') {
                $filtered_active_campaigns[$nid] = $node;
            }
        }

        if(strncmp($filter, 'country-', 8) === 0) {

            $country = substr($filter,8);

            if($node->field_dw_country_grouping['und']['0']['value'] == $country) {
                $filtered_active_campaigns[$nid] = $node;
            }
        }
    }

    $keys               = array_keys($filtered_active_campaigns);

    $result = db_select('field_data_field_dw_host_primary', 'n')->fields('n')->condition('bundle', 'dw_campaigns_derby', '=')->condition('entity_id', $keys, 'IN')->execute();
            
    foreach($result as $row) {
        if(!isset($matches_by_campaign[$row->entity_id])) {
            $matches_by_campaign[$row->entity_id] = array();
        }
        $matches_by_campaign[$row->entity_id][] = $row->field_dw_host_primary_value;

        $matches[$row->field_dw_host_primary_value] = $row->field_dw_host_primary_value;
    }
        
    $result = db_select('field_data_field_dw_host_secondary','n')->fields('n')->condition('bundle', 'dw_campaigns_derby', '=')->condition('entity_id', $keys, 'IN')->execute();
         
    foreach($result as $row) {
        if(!isset($matches_by_campaign[$row->entity_id])) {
            $matches_by_campaign[$row->entity_id] = array();
        }
        $matches_by_campaign[$row->entity_id][] = $row->field_dw_host_secondary_value;

        $matches[$row->field_dw_host_secondary_value] = $row->field_dw_host_secondary_value;
    }

    return $matches;
}

global $g_contact_fundraisers;
$g_contact_fundraisers = array();

function dw_campaigns_user_host_contact_fundraisers($campaign_id) {

    global $g_contact_fundraisers;

    $campaign = node_load($campaign_id);

    $res = _dw_campaigns_get_pcps_for_campaign($campaign);
    foreach($res as $pcp) {

        $params = array(
            'contact_id'    => $pcp->contact_id,
            'returnFirst'   => 1
        );

        $pcp_contact                                        = _dw_civicrm_contact_get($params);
        $rows[$pcp_contact->contact_id]                     = $pcp_contact->display_name;
        $g_contact_fundraisers[$pcp_contact->contact_id]    = array('name' => $pcp_contact->display_name, 'email' => $pcp_contact->email);
    }

    $form = drupal_get_form('dw_campaigns_user_host_contact_fundraisers_form', $rows);

    return theme('dw_campaigns_user_host_contact_fundraisers', array('campaign' => $campaign, 'form' => $form));

    return; 
}

function dw_campaigns_user_host_contact_fundraisers_form($form, &$form_state, $rows) {

    $form = array();
    $form['#attributes']    = array(
        'enctype'           => "multipart/form-data"
    );
    
    $form['email_subject'] = array(
        '#type'             => 'textfield',
        '#title'            => t('Email Subject'),
        '#required'         => TRUE,
        '#default_value'    => '',
    );

    $form['email_text'] = array(
        '#type'             => 'text_format',
        '#format'           => variable_get('dw_campaigns_cfg_default_filter', null),
        '#title'            => t('Email Content'),
        '#required'         => TRUE,
        '#default_value'    => '',
    );

    $form['fundraisers_pre'] = array(
        '#type'         => 'markup',
        '#markup'       => '<a href="#" class="select-all">' . t('Select All') . '</a>'
    );

    drupal_add_js('
      (function($) {
        $(document).ready(function() {
          $(".select-all").click(function() {
              $(".form-checkbox").attr("checked", "checked");
          });
        });
     })(jQuery);
    ', 'inline');

    $form['fundraisers'] = array(
        '#type'         => 'checkboxes',
        '#title'        => t('Fundraisers Contact'),
        '#options'      => $rows
    );

    $form['submit']         = array(
        '#type'             => 'submit',
        '#value'            => t('Send Email'),
        '#attributes'       => array('class'=> array('btn'))
    );

    return $form;
}

function dw_campaigns_user_host_contact_fundraisers_form_submit($form, &$form_state) {
    global $user;
    global $g_contact_fundraisers;
    
    $values     = $form_state['values'];
    $email_text = is_array($values['email_text']) ? ( isset($values['email_text']['value']) ? $values['email_text']['value'] : '' ) : $values['email_text'];
    $values['email_text']   = $email_text;

    $targets    = array();


    $default_cc = variable_get('dw_campaigns_host_contact_fundraisers_default_cc', '');
    //$default_cc = ''; // for testing

    if(!empty($default_cc)) {
        $targets[]  = $default_cc;
    }

    foreach($values['fundraisers'] as $fundraiser_id => $set) {

        // all checkboxes get returned, but their value is set to 0 if not checked
        if($set == 0) {
            continue;
        }  

        $em_contact = $g_contact_fundraisers[$fundraiser_id];
        $em_to      = '"'. addslashes(mime_header_encode($em_contact['name'])) .'" <'. $em_contact['email'] .'>';
        $targets[]  = $em_to;
    }


    $from       = NULL;

    $from_name  = variable_get('dw_campaigns_user_host_contact_fundraisers_from_name', '');
    $from_email = variable_get('dw_campaigns_user_host_contact_fundraisers_from_email', '');


    if(!empty($from_email)) {
        $from   = '"'. addslashes(mime_header_encode($from_name)) .'" <'. $from_email .'>';
    }

    $params              = $values;


    // Susan asked for this to be changed on 2012-05-23 to come from the host
    $from                = $user->mail;

    try {
        // Revisited 2012-07-25 - now we use their alias if we can find it - destination == who its really to
        $result     = db_query("select * from postfix.forwardings where destination = :destination", array(':destination' => $user->mail));
        if($result->rowCount() > 0) {
            $record = $result->fetchObject();
            $from   = $record->source;
        }
    } catch (PDOException $e) {
        // do nothing
    }

    $params['from']      = $from;
    $params['user_from'] = $user->mail;


    foreach($targets as $email) {
        $language       = language_default();
        drupal_mail('dw_campaigns_user_host_contact_fundraisers', 'contact-fundraisers', $email, $language, $params, $from);

        if($email != $default_cc) {
            drupal_set_message(t('Your message has been sent to @name', array('@name' => $email) ));
        }

    }
}

function dw_campaigns_user_host_contact_fundraisers_mail($key, &$message, $params) {

    switch($key)
    {   
        case 'contact-fundraisers':

            $message['subject']             = $params['email_subject'];
            $message['headers']['reply-to'] = $params['user_from'];

            $body                           = $params['email_text'];

            $message['body'][]              = $body;
            break;
    }
}

function dw_campaigns_admin_contact_hosts($selected = NULL) {

    $form = drupal_get_form('dw_campaigns_admin_contact_hosts_form', $selected);

    if($selected == 'hey') {
        dw_campaigns_admin_contact_hosts_process();
        die;
    }

    return theme('dw_campaigns_admin_contact_hosts', array('form' => $form));
}

function dw_campaigns_admin_grab_exchange_rate_data_cron() {
    // we want to run at most once per day

    $last_date  = variable_get('dw_campaigns_cron_hosts_process_last_run', 0);
    $day        = date("Ymd", REQUEST_TIME);
    $day_dashed = date("Y-m-d", REQUEST_TIME);
    $d_year     = date('Y', REQUEST_TIME);
    $d_month    = date('m', REQUEST_TIME);
    $d_day      = date('d', REQUEST_TIME);


    $dates      = dw_campaigns_get_campaign_dates();

    if($day > $last_date) {
        dw_campaigns_admin_grab_exchange_rate_data();
    }

}

function dw_campaigns_admin_grab_exchange_rate_data() {

    $apiKey = variable_get('currency_service_provider_api_key');

    if(is_null($apiKey)) {

        drupal_set_message(t('Exchange rate apiKey not set'), 'error');

    } else {

        $result = file_get_contents("http://openexchangerates.org/api/latest.json?app_id=".$apiKey, true );

        $data   = json_decode($result);

        if(!isset($data->rates)) {

            drupal_set_message(t('Exchange rates update failed - invalid data returned'), 'error');

        } else {
            drupal_set_message(t('Exchange rates have been updated'));
            variable_set("exchangeRates", $data->{'rates'} );

        }
    }
}

function dw_campaigns_admin_grab_exchange_rate_data_submit($form, $form_state){
    dw_campaigns_admin_grab_exchange_rate_data();

    variable_set( 'currency_service_provider_api_key', $form['currency_service_provider_api_key']['#value'] );

    return $form;
}

function dw_campaigns_admin_save_stripe_api_key($form,$form_state){
    variable_set('stripe_public_api_key', $form['stripeApi']['#value']);
    drupal_set_message('Stripe Configuration Saved.');
}

function dw_campaigns_admin_contact_hosts_process() {

    // we want to run at most once per day

    $last_date  = variable_get('dw_campaigns_cron_hosts_process_last_run', 0);
    $day        = date("Ymd", REQUEST_TIME);
    $day_dashed = date("Y-m-d", REQUEST_TIME);
    $d_year     = date('Y', REQUEST_TIME);
    $d_month    = date('m', REQUEST_TIME);
    $d_day      = date('d', REQUEST_TIME);


    $dates      = dw_campaigns_get_campaign_dates();

    if($day > $last_date) {

        variable_set('dw_campaigns_cron_hosts_process_last_run', $day);

        if(!db_table_exists('dw_campaigns_scheduled_host_contacts')) {
            watchdog('dw cron', 'Admin Contact Table does not exist');
        }

        watchdog('dw cron', 'Processed Admin Contact');

        $results = db_query("select * from {dw_campaigns_scheduled_host_contacts} where enabled = 1 and deleted = 0");
        foreach($results as $row) {
            if(($row->schedule_type == 'exact') && ($row->action_date == $day_dashed)) {
                $values = (array) $row;

                watchdog('contact hosts', 'Started contact hosts: - ' . $row->email_subject);
                dw_campaigns_admin_contact_hosts_send_mails($values);
                watchdog('contact hosts', 'Ended contact hosts: - ' . $row->email_subject);

            } else {

                $target_date = NULL;

                if($row->schedule_type == 'after_event') {
                    $target_date = date("Y-m-d", mktime(0, 0, 0, $d_month, $d_day - $row->day_offset, $d_year));
                }
                if($row->schedule_type == 'before_event') {
                    $target_date = date("Y-m-d", mktime(0, 0, 0, $d_month, $d_day + $row->day_offset, $d_year));
                }

                if(!is_null($target_date)) {
                    foreach($dates as $nid => $node_dates) {

                        if($node_dates['start']  == $target_date) {

                            $targets    = array(); // targets was not being cleared so whoever was in the first loop would get reemailed on the second loop, and the third, and so on

                            $node       = node_load($nid);
                            $primary    = isset($node->field_dw_host_primary['und']['0']['value']) ? $node->field_dw_host_primary['und']['0']['value'] : NULL;
                            $secondary  = isset($node->field_dw_host_secondary['und']['0']['value']) ? $node->field_dw_host_secondary['und']['0']['value'] : NULL;

                            if(!is_null($primary)) {
                                $targets[$primary]      = $primary;
                            }

                            if(!is_null($secondary)) {
                                $targets[$secondary]    = $secondary;
                            }

                            if(count($targets) > 0) {
                                $row->hosts = $targets;
                                $values = (array) $row;
                                dw_campaigns_admin_contact_hosts_send_mails($values);
                            }
                        }
                    } 
                }
            }
        } 
    } 
}

function dw_campaigns_admin_generate_cdn_receipts() {
    $last_date  = variable_get('dw_campaigns_cron_generate_cdn_last_run', 0);
    $now        = time();


    variable_set('dw_campaigns_cron_generate_cdn_lock', 0);
    $lock = variable_get('dw_campaigns_cron_generate_cdn_lock', 0);


    if($now > $last_date + 300) {

        variable_set('dw_campaigns_cron_generate_cdn_last_run', $now);

        if($lock == 1) {
            watchdog("generate cdn receipts", "Lock found, skipping");
            return;
        }

        variable_set('dw_campaigns_cron_generate_cdn_lock', 1);
        watchdog("generate cdn receipts", "started processing");
            
        $cnt     = 0;

        if(db_table_exists('dw_campaigns_contribution_receipts_cdn')) {
            $results = db_query("select * from {dw_campaigns_contribution_receipts_CAD} where contribution_id != -1 and tax_receipt_sent = 0 order by rid");
    
            foreach($results as $row) {
                if($row->rid>3410) {
                    watchdog("generate cdn receipts", "creating #" . $row->contribution_id);
       
                    dw_campaign_receipt_report_email($row->contribution_id, FALSE, TRUE);
    
                    $cnt++;
                }
            }
        } else {
            watchdog('generate cdn receipts', 'dw_campaigns_contribution_receipts_cdn does not exist, skipping generation');
        }

        variable_set('dw_campaigns_cron_generate_cdn_last_run', time());

	watchdog("generate cdn receipts", "finished processing: $cnt " . date("Y-d-m H:i:s"));
        variable_set('dw_campaigns_cron_generate_cdn_lock', 0);
       
    }
}

function dw_campaigns_get_campaign_dates() {
    static $dates = NULL;

    if(is_null($dates)) {
        $dates  = array();
        $result = db_query("SELECT b.* FROM {dw_campaigns_campaigns_to_contribution_pages} a left join {field_data_field_dw_date_range} b on a.nid = b.entity_id where active='Y' and b.entity_id is not null");

        //$result = db_query("SELECT entity_id,field_dw_date_range_value,field_dw_date_range_value2 from {field_data_field_dw_date_range}");
        foreach($result as $row) {
            if(!is_null($row)) {
// LOOK - we want to limit how many campaigns we loop through, we don't really care about the past - but we need to improve the query to only include active campaigns, for now we will filter only on the event occuring this year
                $ts = strtotime($row->field_dw_date_range_value);
                if(1==1 || $ts > mktime(0,0,0,1,1,date('Y'))) {
                    $start = date("Y-m-d", strtotime($row->field_dw_date_range_value));
                    $end   = date("Y-m-d", strtotime($row->field_dw_date_range_value2));
                    $dates[$row->entity_id] = array('start' => $start, 'end' => $end);
                }
            }
        }
    }

    return $dates; 
}

function dw_campaigns_admin_contact_hosts_form($form, &$form_state, $selected = NULL) {

    $form = array();
    $form['#attributes']    = array(
        'enctype'           => "multipart/form-data"
    );

    drupal_add_css('.form-item label { color: black !important;}', 'inline');

    drupal_add_js('
      (function($) {
        $(document).ready(function() {
            $("#edit-schedule-type").change(function() { set_colors(); });

            function set_colors() {

                var val = $("#edit-schedule-type").val();
                if(val == "immediately") {

                    $(".form-item-day-offset").css("background-color", "#ff8888");
                    $(".container-inline-date div").css("background-color", "#ff8888");

                } else if(val == "exact") {

                    $(".form-item-day-offset").css("background-color", "#ff8888");
                    $(".container-inline-date div").css("background-color", "#88ff88");

                } else if(val == "before_event") {
                    $(".form-item-day-offset").css("background-color", "#88ff88");
                    $(".container-inline-date div").css("background-color", "#ff8888");

                } else if(val == "after_event") {

                    $(".form-item-day-offset").css("background-color", "#88ff88");
                    $(".container-inline-date div").css("background-color", "#ff8888");

                }
            }

            $("#edit-schedule-type").change();

        });
      })(jQuery);
    ', 'inline');

    $defaults                   = new stdClass();
    $defaults->schedule_type    = 'before_event';
    $defaults->target_type   = 'all';
    $defaults->day_offset       = 0;
    $defaults->action_date      = '';
    $defaults->from_name        = '';
    $defaults->email_subject    = '';
    $defaults->email_text       = '';
    //$defaults->enabled          = '';


    if(!is_null($selected)) {
        $result = db_query("select * from {dw_campaigns_scheduled_host_contacts} where eid = :eid", array(':eid' => $selected))->fetchObject();
        if(is_object($result)) {
            $defaults = $result;
        }
    }

    $type_options   = array(
        'immediately'    => t('Immediately'),
        'exact'          => t('Specific Date'), 
//        'after_approval' => t('Days After Approval'),
        'before_event'   => t('Days Before Event'),
        'after_event'    => t('Days After Event'),
    );
   
    $form['schedule_type'] = array(
        '#type'             => 'select',
        '#title'            => t('Schedule Type'),
        '#description'      => t('Please select'),
        '#options'          => $type_options,
        '#default_value'    => $defaults->schedule_type
    );

    $target_filter_options = array(
        'all' => t('All'),
        'upcoming' => 'Upcoming Events',
        'completed' => 'Completed Events'
    );

    $countries = dw_campaigns_event_countries_get();
    foreach($countries as $short => $country) {
        $target_filter_options['country-' . $short] = t('By Country - !title', array('!title' => $country->title));
    } 

    if(!isset($target_filter_options['country-other'])) {
        $target_filter_options['country-other'] = 'By Country - Other (default)';
    }
 
    $form['target_filter'] = array(
        '#type'             => 'select',
        '#title'            => t('Target Filter'),
        '#description'      => t('Please select'),
        '#options'          => $target_filter_options,
        '#default_value'    => $defaults->target_type
    );


    $form['day_offset'] = array(
        '#type'             => 'textfield',
        '#title'            => t('Days Before/After'),
        '#description'      => t('Eg. If you want an email sent 5 days before an event select type "Days Before Event" and enter "5" here'),
        '#default_value'    => $defaults->day_offset
    );
    
    $form['action_date'] = array(
        '#type'             => 'date_popup',
        '#date_format'      => 'Y-m-d',
        '#title'            => t('Date of Event'),
        '#description'      => t('Strict date, in format "YYYY-MM-DD" (year month day)'),
        '#default_value'    => $defaults->action_date
    );

    $form['from_name'] = array(
        '#type'             => 'textfield',
        '#title'            => t('From Email'),
        '#required'         => TRUE,
        '#description'      => t('Eg.  onesmallstep@fpwr.org'),
        '#default_value'    => $defaults->from_name
    );
    
    $form['email_subject'] = array(
        '#type'             => 'textfield',
        '#title'            => t('Email Subject'),
        '#required'         => TRUE,
        '#default_value'    => $defaults->email_subject
    );

    $form['email_text'] = array(
        '#type'             => 'text_format',
        '#format'           => variable_get('dw_campaigns_cfg_default_filter', null),
        '#title'            => t('Email Content'),
        '#required'         => TRUE,
        '#default_value'    => $defaults->email_text
    );

    $form['contact_hosts_id'] = array(
        '#type'             => 'hidden',
        '#value'            => $selected
    );

    $form['submit']         = array(
        '#type'             => 'submit',
        '#value'            => t('Send Email'),
        '#attributes'       => array('class'=> array('btn'))
    );

    $form['save']         = array(
        '#type'             => 'submit',
        '#value'            => t('Save'),
        '#attributes'       => array('class'=> array('btn'))
    );

    return $form;
}

function dw_campaigns_admin_contact_hosts_form_validate($form, &$form_state) {

}

// NOTE - SCHEMA CHANGE: ALTER TABLE `dw_campaigns_scheduled_host_contacts` ADD `target_filter` CHAR( 64 ) CHARACTER SET utf8 COLLATE utf8_bin NULL AFTER `schedule_type` 
function dw_campaigns_admin_contact_hosts_form_submit($form, &$form_state) {
    global $user;
    
    $values     = $form_state['values'];
    $email_text = is_array($values['email_text']) ? ( isset($values['email_text']['value']) ? $values['email_text']['value'] : '' ) : $values['email_text'];
    $values['email_text']   = $email_text;
    $values['day_offset']   = intval($values['day_offset']);

    $db_keys = array('schedule_type', 'day_offset', 'action_date', 'from_name', 'email_subject', 'email_text', 'target_filter');
 
    $db_values = array();
    foreach($db_keys as $key) {
        if(isset($values[$key])) {
            $db_values[$key] = $values[$key];
        }
    }
        
    $db_values['enabled']   = 1;

    if($values['schedule_type'] == 'immediately') {
        $db_values['enabled']   = 0;
    }

    if($values['contact_hosts_id'] > 0) {
        db_update('dw_campaigns_scheduled_host_contacts')->fields($db_values)->condition('eid', $values['contact_hosts_id'], '=')->execute();
    } else {
        db_insert('dw_campaigns_scheduled_host_contacts')->fields($db_values)->execute();
    } 

    if($values['schedule_type'] == 'immediately') {
        if($form_state['clicked_button']['#value'] != 'Save') {
            dw_campaigns_admin_contact_hosts_send_mails($values); 
        }
    } else {

    }

    drupal_goto('admin/dw/contact_hosts');
}

function dw_campaigns_admin_contact_hosts_delete($contact_hosts_id) {
    $db_values = array(
        'deleted' => 1,
    );

    db_update('dw_campaigns_scheduled_host_contacts')->fields($db_values)->condition('eid', $contact_hosts_id, '=')->execute();

    $parts = explode('/', $_GET['q']);

    array_pop($parts);
    array_pop($parts);

    $dest = implode('/', $parts);

    drupal_set_message(t('Deleted Host Contact Email #:eid', array(':eid' => $contact_hosts_id)));

    drupal_goto($dest);
}

function dw_campaigns_admin_contact_hosts_send_mails($values) {
    $targets    = array();


    $default_cc = variable_get('dw_campaigns_admin_contact_hosts_default_cc', '');
    //$default_cc = ''; // for testing

    if(!empty($default_cc)) {
        $targets[]  = $default_cc;
    }

    if(!isset($values['hosts'])) { 
        if(!isset($values['target_filter']) || is_null($values['target_filter']) || $values['target_filter'] == 'all') {
//REMOVE
            $hosts = dw_campaigns_get_active_campaigns_host_list();
        } else {
            $hosts = dw_campaigns_get_active_campaigns_host_list_filtered($values['target_filter']);
        }
    } else {
        $hosts = $values['hosts'];
    }
 
    foreach($hosts as $hostid) {
        $uid =  _dw_campaigns_get_userid_by_hostid($hostid);
        $contact_id = _dw_campaigns_user_get_contact_id($uid); 

        $params = array(
            'contact_id'    => $contact_id,
            'returnFirst'   => 1
        );

        $em_contact = _dw_civicrm_contact_get($params);
        $em_to      = '"'. addslashes(mime_header_encode($em_contact->display_name)) .'" <'. $em_contact->email .'>';
        $targets[$uid]  = $em_to; // add them by uid, so we don't send dupes if someones a member of multiple campaigns and this is to everyone
    }

    $from       = NULL;

    if(!empty($values['from_name'])) {
        $from       = $values['from_name'];
    }

/*
    if(!empty($from_email)) {
        $from   = '"'. addslashes(mime_header_encode($from_name)) .'" <'. $from_email .'>';
    }
*/

    $params              = $values;
    $params['from']      = $from;
    $params['user_from'] = $from;

    foreach($targets as $email) {
        $language       = language_default();

        drupal_mail('dw_campaigns_admin_contact_hosts', 'contact-hosts', $email, $language, $params, $from);

        if($email != $default_cc) {
            drupal_set_message(t('Your message has been sent to @name', array('@name' => $email) ));
        }

    }
}

function dw_campaigns_admin_contact_hosts_mail($key, &$message, $params) {

    switch($key)
    {   
        case 'contact-hosts':

            $message['subject']             = $params['email_subject'];
            $message['headers']['reply-to'] = $params['user_from'];

            $body                           = $params['email_text'];

            $message['body'][]              = $body;
            break;
    }
}

function _dw_campaigns_get_userid_by_hostid($hostid) {
    static $mapping = NULL;

    if(is_null($mapping)) {
        $mapping = array();

        $result    = db_query("SELECT entity_id,field_dw_host_user_value  from {field_data_field_dw_host_user}");
        foreach($result as $row) {
            if(!is_null($row)) {
                $mapping[$row->entity_id] = $row->field_dw_host_user_value;
            }
        }
    }
 
    if(isset($mapping[$hostid])) {
        return $mapping[$hostid];
    }

    return NULL;
}

function dw_campaigns_get_campaign_hosts($campaign, $only_titles = FALSE) {
    $hosts          = array();

    $titles         = array();

    if(is_object($campaign) && isset($campaign->field_dw_host_primary)) {  
        $primary_host   = isset($campaign->field_dw_host_primary['und']['0']['value']) ? $campaign->field_dw_host_primary['und']['0']['value'] : NULL;
        $secondary_host = isset($campaign->field_dw_host_secondary['und']['0']['value']) ? $campaign->field_dw_host_secondary['und']['0']['value'] : NULL;
    
        if(!is_null($primary_host)) {
            if(!isset($host_data[$primary_host])) {
                $host_data[$primary_host] = node_load($primary_host);
            }
    
            $host_record = $host_data[$primary_host];
            $title = isset($host_record->field_dw_host_name['und']['0']['value']) ? $host_record->field_dw_host_name['und']['0']['value'] : 'Host';
            $email = isset($host_record->field_dw_host_email['und']['0']['value']) ? $host_record->field_dw_host_email['und']['0']['value'] : NULL;
    
            if(!is_null($title)) {
                $hosts[]    = '<span class="host"><a class="fb_ns" href="/dw/host/contact/' . $primary_host . '">' . $title . '</a></span>';
                $titles[]   = $title;
            }
        }
    
        if(!is_null($secondary_host)) {
            if(!isset($host_data[$secondary_host])) {
                $host_data[$secondary_host] = node_load($secondary_host);
            }
    
            $host_record = $host_data[$secondary_host];
            $title = isset($host_record->field_dw_host_name['und']['0']['value']) ? $host_record->field_dw_host_name['und']['0']['value'] : 'Host';
            $email = isset($host_record->field_dw_host_email['und']['0']['value']) ? $host_record->field_dw_host_email['und']['0']['value'] : NULL;
    
            if(!is_null($title)) {
                $hosts[]    = '<span class="host"><a class="fb" href="/dw/host/contact/' . $secondary_host . '">' . $title . '</a></span>';
                $titles[]   = $title;
            }
        }
    }

    if($only_titles) {
       return $titles;
    }

    return $hosts;
}

function dw_campaigns_validate_email_domain($hostname) {

    $validhost = false;

    $result = getmxrr($hostname, $results);

    if($result) {
        foreach($results as $entry) {
            break;
        }
    
        if(is_null($entry) || $entry == '0.0.0.0') {
            return false;
        }
       
        $validhost = true;
    }
    
    if(!$result) {
        $new_addr = gethostbyname($hostname);
        if($new_addr != $hostname) {
            $validhost = true; // no MX - must be true
        }
    }
    
    return $validhost;

}

function dw_campaigns_report_show_clean_urls() {
    $result = db_query("select a.*,b.title from field_data_field_dw_campaign_location a left join node b on a.entity_id = b.nid left join dw_campaigns_campaigns_to_contribution_pages c on a.entity_id = c.nid where field_dw_campaign_location_value like '%, %' and c.active = 'Y' order by title, entity_id desc");
    
    $headers        = array(
        array(
            'data'  => t('Campaign Title'),
        ),
        array(
            'data'  => t('Location Name'),
        ),
        array(
            'data'  => t('Regular URL'),
        ),
        array(
            'data'  => t('Friendly URL'),
        ),
    );

    $rows   = array();
    foreach($result as $db_row) {
        $parts  = explode(", ", $db_row->field_dw_campaign_location_value, 2);
        $city   = urlencode(str_replace(" ", "-", strtolower($parts[0])));
        $state  = urlencode(str_replace(" ", "-", strtolower($parts[1])));
 
        $regular = sprintf("http://%s/dw/walking/location/%s", $_SERVER['SERVER_NAME'], $db_row->entity_id); 
        $friendly = sprintf("http://%s/%s/%s", $_SERVER['SERVER_NAME'], $state, $city); 

        $rows[] = array(
            'data' => array(
                array('data' => $db_row->title, 'class' => array('campaign-title')),
                array('data' => $db_row->field_dw_campaign_location_value, 'class' => array('location-name')),
                array('data' => "<a href=\"$regular\">$regular</a>", 'class' => array('regular-url')),
                array('data' => "<a href=\"$friendly\">$friendly</a>", 'class' => array('friendly-url')),
            )
        );
    }

    return theme('table', array('header' => $headers, 'rows' => $rows));
}

function dw_campaigns_checkout_pp_ipncatch() {

    global $dw_campaign_module_path;

    $other = $_POST;
    
    $other['cmd'] = '_notify-validate';
    $req = html_entity_decode(http_build_query($other));
    
    //$domain="https://www.sandbox.paypal.com/cgi-bin/webscr";
    $domain="https://www.paypal.com/cgi-bin/webscr";
    
    $curl = curl_init();
    
    curl_setopt($curl, CURLOPT_URL, $domain);
    curl_setopt($curl, CURLOPT_SSL_VERIFYPEER, false);
    curl_setopt($curl, CURLOPT_VERBOSE, 0);
    curl_setopt($curl, CURLOPT_HEADER, 0);
    curl_setopt($curl, CURLOPT_POST, 1);
    curl_setopt($curl, CURLOPT_RETURNTRANSFER, 1);
    curl_setopt($curl, CURLOPT_POSTFIELDS, $req);
    file_put_contents("/tmp/last_req", serialize($req));
    $results = curl_exec($curl);
    
    file_put_contents("/tmp/results", $results);
    
    if(!curl_errno($curl)) {
            $info = curl_getinfo($curl);
    } else {
            $info = 'Curl error: ' . curl_error($curl);
    }
    
    file_put_contents("/tmp/info", $info);
    
    curl_close($curl);
    
    
    
    if (strncmp($results, "VERIFIED", 8) == 0 || strpos($results, "An error occurred") !== FALSE) {
    
        $hash   = $_POST['custom'];
        $result = db_query("select * from {dw_campaigns_checkout_swap} where hash = :hash", array(':hash' => $hash));
        
        if($result->rowCount() == 0) {
			// FIXME - these should be saved somewhere better than
            file_put_contents($dw_campaign_module_path . "/failed_ipn/" . microtime(true), print_r($other, TRUE) . "\n\n" . serialize($other));
            // FIXME - this needs to be defined on an admin form
			mail("root@", "bad ipn", print_r($other, TRUE));
        } else {
            $row = $result->fetchObject();
    
            $values = unserialize($row->data);

            $table = 'dw_campaigns_offline_donation';
            if(!db_table_exists($table)) {
                dw_campaigns_create_offline_donation_table();
            }

            cache_clear_all($values['pcpId'] . '_CONVERT', 'cache');
            cache_clear_all($values['pcpId'] . '_DONT', 'cache');
    
            db_insert($table)->fields(array(
                'responsible_drupal_id' => -1,
                'remote_addr'           => '', 
                'currency'              => 'EUR',
                'first_name'            => $values['first_name'],
                'last_name'             => $values['last_name'],
                'email'                 => $values['email'],
                'address_1'             => $values['address_1'],
                'city'                  => $values['city'],
                'state'                 => $values['state'],
                'postal_code'           => $values['postal_code'],
                'country'               => $values['country'],
                'pcp_id'                => $values['pcpId'],
                'campaign'              => $values['campaign'],
                'include_in_honor_roll' => $values['include_in_honor_roll'],
                'donation_amount'       => $values['donation-amount'],
                'payment_instrument'    => 'cash',
                'payment_check_number'  => '',
                'trxn_id'               => $other['txn_id'],
                'receive_date'          => date("Y-m-d", strtotime(urldecode($other['payment_date'])))
            ))->execute(); 
    
    
        }
    } else {
		// FIXME - these should be saved somewhere better than /tmp, and having it write to two places is odd
        file_put_contents("/tmp/ipn_fraud.log", serialize($_POST) . "\n", FILE_APPEND);
        file_put_contents($dw_campaign_module_path . "/ipn_fraud.log", serialize($_POST) . "\n", FILE_APPEND);
        
		// FIXME - this needs to be defined on an admin form
		mail("root@", "failed ipn verification", print_r($_POST, TRUE) . "\n\n" . $results );
        break;
    }

    die;
}

function dw_campaigns_get_tag_id_by_name($name, $used_for = 'civicrm_contact') {
    $res = _dw_civicrm_tag_get(array('name' => $name));

    if(is_null($res->tag_id)) {
        $params = array(
            'name'        => $name,
            'description' => $name,
            'used_for'    => $used_for
        );
   
        $res = _dw_civicrm_tag_create($params); 
    }



    return $res->tag_id;
}

function dw_campaigns_tag_contact($tag_id = NULL, $contact_list = array()) {

    $tag_id = (int) $tag_id;

    // do additional validation
    if(!is_null($tag_id) && $tag_id > 0) {

        $params = array();
        $params['tag_id_holder'] = $tag_id;
        $params['entity_table']  = 'civicrm_contact';
 
        foreach($contact_list as $contact_id) {
                $params['entity_id_' . $contact_id] = $contact_id;
        }

        $res = _dw_civicrm_tag_entity_add($params);
    }

    watchdog('added tag', 'tag #' . $tag_id . ' to ' . implode(", ", $contact_list));
}

function dw_campaigns_relationship_tag_list() {
    $types = array(
        '9'  => t('Parent of someone with PWS'),
        '11' => t('Sibling of someone with PWS'),
        '14' => t('Relative of someone with PWS'),
        '33' => t('Friend'),
        '34' => t('Other')
    );

    return $types;
}

function dw_campaigns_user_host_manage_files_form($form, &$form_state, $extended_data) {
    global $user;

    $form = array();

    if(!is_object($extended_data)) {
        $extended_data = node_load($extended_data);    
    }


    $docs = array();


    for($i=0;$i<=4;$i++) {
        $orig_filename = isset($extended_data->field_dw_eventdetails_dl_documen['und'][$i]['filename']) ? $extended_data->field_dw_eventdetails_dl_documen['und'][$i]['filename'] : NULL;
        if(is_null($orig_filename)) {
            continue;
        }

        $uri = isset($extended_data->field_dw_eventdetails_dl_documen['und'][$i]['uri']) ? $extended_data->field_dw_eventdetails_dl_documen['und'][$i]['uri'] : NULL;
        $download_url = file_create_url($uri);

        if(!is_null($download_url)) {
            $download_title = isset($extended_data->field_dw_eventdetails_dl_documen['und'][$i]['description']) ? $extended_data->field_dw_eventdetails_dl_documen['und'][$i]['description'] : NULL;
    
            if(is_null($download_title) || empty($download_title)) {
                $download_title = t('Download');
            }
    
            $docs[] = array('title' => $download_title, 'url' => $download_url, 'filename' => $orig_filename);
        }
    }   

    // Add the current choices to the form. 
    for ($delta = 0; $delta < 5; $delta++) { 
        $doc = isset($docs[$delta]) ? $docs[$delta] : NULL;
        $form['event_files'][$delta] = dw_campaigns_user_host_manage_files_row($delta, $doc); 
    } 
 
    $form['extended_id'] = array(
        '#type'             => 'hidden',
        '#value'            => $extended_data->nid
    );


    $form['submit']         = array(
        '#type'             => 'submit',
        '#value'            => t('Submit'),
        '#attributes'       => array('class'=> array('btn'))
    );

    return $form;


}

function dw_campaigns_user_host_manage_files_row($delta, $doc) { 
 
    $form = array( 
        '#tree' => TRUE, 
    ); 

    if(is_null($doc)) {
        $description = '';
        $old_file    = '';
    } else {
        $description = $doc['title'];
        $old_file    = $doc['url'];
    }

    $attributes         = array('disabled' => TRUE);
    $old_file_markup    = '';

    if(!empty($old_file)) {
        $attributes         = array();
        $old_file_markup    = '<span class="old_file"><a href="' . $doc['url'] . '">' . $doc['filename'] . '</a></span>';
    }
     
    // We'll manually set the #parents property of these fields so that 
    // their values appear in the $form_state['values']['choice'] array. 
    $form['do_delete'] = array( 
        '#type'             => 'checkbox', 
        '#title'            => t('Delete This File'), 
        '#required'         => false, 
        '#prefix'           => '<div class="upload-row">',
        '#attributes'       => $attributes,
        '#parents'          => array('event_files', $delta, 'do_delete'), 
    );
 
    $form['old_file'] = array( 
        '#type'             => 'markup', 
        '#markup'           => $old_file_markup
    ); 

    $form['description'] = array( 
        '#type'             => 'textfield', 
        '#title'            => t('Description'), 
        '#default_value'    => $description, 
        '#required'         => false, 
        '#parents'          => array('event_files', $delta, 'description'), 
    ); 

     
    $form['new_file_' . $delta] = array( 
        '#type'             => 'file', 
        '#title'            => t('Upload File'), 
        '#required'         => false, 
        '#suffix'           => '</div>',
        '#parents'          => array('event_files_' . $delta, $delta, 'new_file'), 
    ); 
     
     
     
    return $form; 
}

function dw_campaigns_user_host_manage_files_form_validate(&$form, &$form_state) {
    global $user;

    $values = $form_state['values'];
}

function dw_campaigns_user_host_manage_files_form_submit(&$form, &$form_state) {
    global $user;

    $values = $form_state['values'];

    $directory = 'extended_' . $values['extended_id'];
        
    $uploadDir = "dw_campaigns/$directory";

    $validators = array(
        'file_validate_extensions' => array('jpg jpeg gif png pdf doc docx rtf txt')
    );

    dw_campaigns_user_host_manage_files_form_file_upload('event_files_0', $validators, $uploadDir);
    dw_campaigns_user_host_manage_files_form_file_upload('event_files_1', $validators, $uploadDir);
    dw_campaigns_user_host_manage_files_form_file_upload('event_files_2', $validators, $uploadDir);
    dw_campaigns_user_host_manage_files_form_file_upload('event_files_3', $validators, $uploadDir);
    dw_campaigns_user_host_manage_files_form_file_upload('event_files_4', $validators, $uploadDir);
 

var_dump($values);die;
}

function dw_campaigns_user_host_manage_files_form_file_upload($name, $validators, $uploadDir) {
    //this is a fresh file upload
    $targDir = $_SERVER['DOCUMENT_ROOT'] . base_path() . variable_get('file_public_path', conf_path() . '/files') . '/' . $uploadDir;
    @mkdir($targDir);

    $uploadDir = 'public://' . $uploadDir;
    if(isset($_FILES['files']['size'][$name]) && $_FILES['files']['size'][$name] > 0) {

        $file       = file_save_upload($name, $validators, $uploadDir);
    
        // this is critical - otherwise they'll be deleted when the cron runs
        $file->status |= FILE_STATUS_PERMANENT;
        file_save($file);
    
        //an error occured
        if($file->status == 0) {
            return FALSE;
        }

        return TRUE;
    }
}

function flickr_event_gallery_form( $form, &$form_state, $event_id ){
    global $user;

    $event = node_load( $event_id );


    $form = array();

    $form['flickr_user_name'] = array(
        '#type'         => 'textfield',
        '#title'        => 'User Id',
        '#description'  => 'Your Flickr User Name',
        '#default_value'=> $event->flickr_gallery_id['und'][0]['value'],
        '#required'     => false
    );

    $form['flickr_photoset_id'] = array(
        '#type'         => 'textfield',
        '#title'        => 'Photoset Id',
        '#description'  => 'The id number of the photoset you want to use for this event\'s gallery',
        '#default_value'=> $event->flickr_gallery_photoset_id['und'][0]['value'],
        '#required'     => false
    );

    $form['flickr_help_link'] = array(
        '#type'         => 'markup',
        '#markup'       => '<a href="/dw/help/tutorials/flickr_event_gallery">Need Help?</a><br><br>'
    );

    $form['submit']         = array(
        '#type'             => 'submit',
        '#value'            => t('Submit'),
        '#attributes'       => array('class'=> array('btn'))
    );


    return $form;
}

function flickr_event_gallery_form_validate($form, &$form_state){

}

function flickr_event_gallery_form_submit($form, &$form_state){
    global $user;

    $event_id = $form_state['build_info']['args'][0];

    $event = node_load( $event_id );

    $event->flickr_gallery_id['und'][0]['value'] = $form_state['input']['flickr_user_name'];
    $event->flickr_gallery_photoset_id['und'][0]['value'] = $form_state['input']['flickr_photoset_id'];

    node_save( $event );

}


function dw_campaigns_finallyfull() {
    return theme('dw_campaigns_finallyfull_page_theme');
}

function dw_campaigns_get_involved_page() {

    $form = drupal_get_form('dw_campaigns_walking_contact_block_form');
    return theme('dw_campaigns_get_involved', array('form' => $form));

}

function dw_campaigns_themometer_only() {
    return theme('dw_campaigns_thermometer_only', array());
}
