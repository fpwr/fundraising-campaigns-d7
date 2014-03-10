<?php

//JFN todo - I really don't like the inconsistency here.  Consider consolidating related functionality into it's own set of include files, and possibly auto loading them at some point to clean up the routing system a bit-  That's going to be a considerable amount of work that no one will want to pay for, so consider it weekend practice when you get around to it.

function dw_campaigns_currency_config(){
    return array(
        'currency_service_provider' =>  array(
            '#title'            => 'Exchange Rate Data Provider',
            '#type'             => 'select',
            '#options'          => Array('openexchangerates.org'),
            '#description'      => t('Links your site to an external data provider for currency conversion rates. To ensure you have the most up to date currency values possible.'),
            '#maxlength'        => 250,
            '#default_value'    => 0
        ),
        'currency_service_provider_api_key' => array(
            '#title'            => 'Account API key',
            '#type'             => 'textfield',
            '#description'      => t('data providers usually require an api key, get one and enter it here!'),
            '#maxlength'        => 250,
            '#default_value'    => variable_get('currency_service_provider_api_key')
        ),
        'currency_service_provider_get_data' => array(
            '#type'             => 'submit',
            '#value'            => t('Sync Exchange Rates Now / Save Configuration'),
            '#submit'           => array('dw_campaigns_admin_grab_exchange_rate_data_submit')
        )
    );
}

function dw_campaigns_civicrm_config() {
    global $user;
    
    $form = array();


    $formats = filter_formats($user);

    $options = array(null => '-- USE Default Filter Behavior --');
    foreach ($formats as $format) {
        $options[$format->format] = $format->name;
    }

    $form['civi_settings']['dw_campaigns_cfg_default_filter'] = array(
        '#type' => 'select',
        '#title' => t('Default Text format (should be filtered html)'),
        '#options' => $options,
        '#default_value' => variable_get('dw_campaigns_cfg_default_filter', array_shift($options)),
        '#access' => count($formats) > 1,
    );

    
    $form['civi_settings']['dw_campaigns_cfg_rest_url'] =  array(
        '#title'            => 'Civicrm REST Interface URL',
        '#type'             => 'textfield',
        '#description'      => t('Civi REST Interface URL ( http://samplecivi.com/sites/all/modules/civicrm/extern/rest.php?q=civicrm )'),
        '#maxlength'        => 250,
        '#default_value'    => variable_get('dw_campaigns_cfg_rest_url', '')
    );
    
    $form['civi_settings']['dw_campaigns_cfg_sitekey'] = array(
        '#title'            => 'REST Interface Site Key',
        '#type'             => 'textfield',
        '#description'      => t('Civi REST Interface Site Key (CIVICRM_SITE_KEY from civicrm.settings.php'),
        '#maxlength'        => 250,
        '#default_value'    => variable_get('dw_campaigns_cfg_sitekey', '')           
    );

    $form['civi_settings']['dw_campaigns_cfg_apikey'] = array(
        '#title'            => 'REST API Key (is tied to a user)',
        '#type'             => 'textfield',
        '#description'      => t('Civi REST API Key'),
        '#maxlength'        => 250,
        '#default_value'    => variable_get('dw_campaigns_cfg_apikey', '')           
    );
    
    $form['civi_settings']['dw_campaigns_cfg_base_url'] =  array(
        '#title'            => 'Civicrm Public URL',
        '#type'             => 'textfield',
        '#description'      => t('Base URL to reach backends civicrm pages ( http://samplecivi.com )'),
        '#maxlength'        => 250,
        '#default_value'    => variable_get('dw_campaigns_cfg_base_url', '')
    );

    $form['civi_settings']['dw_campaigns_dedupe_callback_login'] =  array(
        '#title'            => 'Civicrm Dedupe Login',
        '#type'             => 'textfield',
        '#description'      => t('Login used by "civicrm merge catch" module'),
        '#maxlength'        => 250,
        '#default_value'    => variable_get('dw_campaigns_dedupe_callback_login', '')
    );

    $form['civi_settings']['dw_campaigns_dedupe_callback_pass'] =  array(
        '#title'            => 'Civicrm Dedupe Login',
        '#type'             => 'textfield',
        '#description'      => t('Login used by "civicrm merge catch" module'),
        '#maxlength'        => 250,
        '#default_value'    => variable_get('dw_campaigns_dedupe_callback_pass', '')
    );

    $form['civi_settings']['dw_campaigns_cfg_tag_prefix'] =  array(
        '#title'            => 'Civicrm Tag Prefix',
        '#type'             => 'textfield',
        '#description'      => t('Prefix to use when generating tags - eg: "OSS" would create "OSS fundraiser 2012"'),
        '#maxlength'        => 250,
        '#default_value'    => variable_get('dw_campaigns_cfg_tag_prefix', 'OSS')
    );

    $form['civi_settings']['dw_campaigns_cfg_tag_suffix'] =  array(
        '#title'            => 'Civicrm Tag Suffix',
        '#type'             => 'textfield',
        '#description'      => t('Prefix to use when generating tags - eg: "2013" would create "OSS fundraiser 2013"'),
        '#maxlength'        => 250,
        '#default_value'    => variable_get('dw_campaigns_cfg_tag_suffix', '2013')
    );

    $form['elasticemail_settings']['dw_campaigns_cfg_ee_username']  = array(
        '#title'            => 'Elastic Email Username',
        '#type'             => 'textfield',
        '#description'      => t('Prefix to use when generating tags - eg: "OSS" would create "OSS fundraiser 2012"'),
        '#maxlength'        => 250,
        '#default_value'    => variable_get('dw_campaigns_cfg_ee_username', '')
    );

    $form['elasticemail_settings']['dw_campaigns_cfg_ee_apikey']  = array(
        '#title'            => 'Elastic Email API Key',
        '#type'             => 'textfield',
        '#description'      => t('Elastic Email API Key - this is used to get information from EE'),
        '#maxlength'        => 250,
        '#default_value'    => variable_get('dw_campaigns_cfg_ee_apikey', '')
    );

    $form['elasticemail_settings']['dw_campaigns_cfg_ee_apikey']  = array(
        '#title'            => 'Elastic Email API Key',
        '#type'             => 'textfield',
        '#description'      => t('Elastic Email API Key - this is used to get information from EE'),
        '#maxlength'        => 250,
        '#default_value'    => variable_get('dw_campaigns_cfg_ee_apikey', '')
    );
    
    return system_settings_form($form);

}

function dw_campaigns_settings() {
    $form = array();

    
    $form['mode_settings'] = array(
        '#type'         => 'fieldset',
        '#title'        => t('DW Campaign Module Settings'),
        '#description'  => t('Configure Operating Settings')
    );
    
    $form['mode_settings']['dw_campaigns_operating_mode'] = array(
        '#type'             => 'select',
        '#options'          => array(
            'derby'         => 'Derby',
            'walking'       => 'Walking',
        ),
        '#default_value'    => variable_get('dw_campaigns_operating_mode', 'derby')
    );

    $form['mode_settings']['dw_campaigns_organization_name'] = array(
        '#type'             => 'textfield',
        '#title'            => t('Organization Name'),
        '#description'      => t('Global token available for configuring email messages.'),
        '#default_value'    => variable_get('dw_campaigns_organization_name', 'our cause')
    );
    
    
    $form['mode_settings']['dw_campaigns_organization_name_abbreviated'] = array(
        '#type'             => 'textfield',
        '#title'            => t('Organization Name Abbreviation'),
        '#description'      => t('Global token available for configuring email messages.'),
        '#default_value'    => variable_get('dw_campaigns_organization_name_abbreviated', '')
    );
    
    $form['mode_settings']['dw_campaigns_dw_node_title_prefix'] = array(
        '#type'             => 'textfield',
        '#title'            => t('Prefix for Event Content Types'),
        '#description'      => t('This title is the prefix for an event, on the location page. Eg. TheseWords New York'),
        '#default_value'    => variable_get('dw_campaigns_dw_node_title_prefix', '')
    );
    
    
    $homepage = variable_get('dw_campaigns_homepageurl', '');
    if(empty($homepage)) {
        $cur_host = $_SERVER['HTTP_HOST'];
        $homepage = 'http://' . $cur_host . '/dw/';
    }
    
    $form['mode_settings']['dw_campaigns_homepageurl'] = array(
        '#type'             => 'textfield',
        '#title'            => t('This sites homepage URL'),
        '#description'      => t('Campaign site homepage url available for configuring email messages.'),
        '#default_value'    => $homepage
    );

    
    $form['mode_settings']['dw_campaigns_organization_website'] = array(
        '#type'             => 'textfield',
        '#title'            => t('Your organizations website URL'),
        '#description'      => t('Organization homepage url available for configuring email messages.'),
        '#default_value'    => variable_get('dw_campaigns_organization_website', '')
    );

    $form['mode_settings']['dw_campaign_max_intro_text_chars'] = array(
        '#type'             => 'textfield',
        '#title'            => t('Maximum Number of Characters Allowed for Personal Campaign Intro'),
        '#description'      => t('User will not be able to type once this limit is reached'),
        '#default_value'    => variable_get('dw_campaign_max_intro_text_chars', 500)
    );

    $form['mode_settings']['dw_campaign_offline_transaction_override'] = array(
        '#type'             => 'textfield',
        '#title'            => t('Numeric code for offline transactions'),
        '#description'      => t('Enter this code into the Credit Card field to credit a personal campaign without charging a card'),
        '#default_value'    => variable_get('dw_campaign_offline_transaction_override', '1234588')
    );

    $form['mode_settings']['dw_campaign_default_pcp_goal'] = array(
        '#type'             => 'textfield',
        '#title'            => t('Default Personal Campaign Goal'),
        '#description'      => t('This is the initial value for a new Personal Campaign Goal'),
        '#default_value'    => variable_get('dw_campaign_default_pcp_goal', '100.00')
    )
;
    $form['mode_settings']['dw_campaigns_show_fireworks'] = array(
        '#type'             => 'select',
        '#title'            => t('Show fireworks effect when raised amount is higher than goal'),
        '#description'      => t('This is for all pages with a thermometer'),
        '#options'          => array('1' => 'Yes', '0' => 'No'),
        '#default_value'            => variable_get('dw_campaigns_show_fireworks', '1')
    );

    $form['mode_settings']['dw_campaigns_allow_youtube'] = array(
        '#type'             => 'select',
        '#title'            => t('Allow Youtube videos on Personal Campaign Page'),
        '#description'      => t('Allows user to paste in a http://youtube.com/ link to display instead of an image'),
        '#options'          => array('1' => 'Yes', '0' => 'No'),
        '#default_value'            => variable_get('dw_campaigns_allow_youtube', '1')
    );

    $form['mode_settings']['dw_campaign_auto_bcc']= array(
        '#type'             => 'textfield',
        '#title'            => t('Global Email Blind Carbon Copy (BCC)'),
        '#description'      => t('If defined all emails sent via drupal_mail will be bcc\'d to this address'),
        '#default_value'    => variable_get('dw_campaign_auto_bcc', '')
    );

    $form['mode_settings']['dw_campaign_contact_block_email']= array(
        '#type'             => 'textfield',
        '#title'            => t('Site Contact Email'),
        '#description'      => t('Address to email if the dw_campaigns contact form is used'),
        '#default_value'    => variable_get('dw_campaign_contact_block_email', '')
    );


    
    $form['derby_specific'] = array(
        '#type'         => 'fieldset',
        '#title'        => t('Derby Specific Settings'),
        '#description'  => t('Configure Derby Settings'),
        '#collapsed'        => TRUE,
        '#collapsible'      => TRUE,
    );
    $form['derby_specific']['dw_campaigns_derby_youtube_default'] = array(
        '#type'             => 'textfield',
        '#title'            => t('Default Youtube video'),
        '#default_value'    => variable_get('dw_campaigns_derby_youtube_default', '')
    );

    $form['derby_specific']['dw_campaigns_default_campaign'] = array(
        '#type'             => 'textfield',
        '#title'            => t('Node id of default campaign'),
        '#default_value'    => variable_get('dw_campaigns_default_campaign', '4')
    );

    $form['derby_specific']['dw_campaigns_derby_about_derby'] = array(
        '#title'            => 'Address for "About the Derby" link',
        '#type'             => 'textfield',
        '#description'      => t('URL for About the Derby link on the Banner Menu.'),
        '#maxlength'        => 250,
        '#default_value'    => variable_get('dw_campaigns_derby_about_derby', '')           
    );


    
    $form['walking_specific'] = array(
        '#type'         => 'fieldset',
        '#title'        => t('Walking Specific Settings'),
        '#description'  => t('Configure Walking Settings'),
        '#collapsed'        => TRUE,
        '#collapsible'      => TRUE,
    );
  
    $form['walking_specific']['dw_campaigns_walking_pws_relationship'] = array(
        '#title'            => 'Enable Prompt for "What is your relationship to PWS"',
        '#type'             => 'select',
        '#options'          => array(
            '0' => t('Disabled'),
            '1' => t('Enabled'),
        ),
        '#default_value'    => variable_get('dw_campaigns_walking_pws_relationship', 1)
    );
 
    $form['walking_specific']['dw_campaigns_walking_adults_attending_prompt_enabled'] = array(
        '#title'            => 'Enable Prompt for "How many adults are attending the event"',
        '#type'             => 'select',
        '#options'          => array(
            '0' => t('Disabled'),
            '1' => t('Enabled'),
        ),
        '#default_value'    => variable_get('dw_campaigns_walking_adults_attending_prompt_enabled', 1)
    );
 
    $form['walking_specific']['dw_campaigns_walking_adults_attending_prompt'] = array(
        '#title'            => 'Prompt for "How many adults are attending the event"',
        '#type'             => 'textfield',
        '#description'      => t('Please define this in English and translate to other languages with the translation system'),
        '#default_value'    => variable_get('dw_campaigns_walking_adults_attending_prompt', 'How many adults will be attending the event')           
    );

    $form['walking_specific']['dw_campaigns_walking_children_attending_prompt_enabled'] = array(
        '#title'            => 'Enable Prompt for "How many children are attending the event"',
        '#type'             => 'select',
        '#options'          => array(
            '0' => t('Disabled'),
            '1' => t('Enabled'),
        ),
        '#default_value'    => variable_get('dw_campaigns_walking_children_attending_prompt_enabled', 1)
    );

    $form['walking_specific']['dw_campaigns_walking_children_attending_prompt'] = array(
        '#title'            => 'Prompt for "How many children are attending the event"',
        '#type'             => 'textfield',
        '#description'      => t('Please define this in English and translate to other languages with the translation system'),
        '#default_value'    => variable_get('dw_campaigns_walking_children_attending_prompt', 'How many children will be attending the event')           
    );
    
    $form['walking_specific']['dw_campaigns_walking_distance_unit'] = array(
        '#title'            => 'Distance Measurement Unit',
        '#type'             => 'select',
        '#options'          => array(
            'M'             => 'Miles',
            'K'             => 'Kilometers',
        ),
        '#default_value'    => variable_get('dw_campaigns_walking_distance_unit', 'M')           
    );
    
    $form['walking_specific']['dw_campaigns_host_submit_email'] = array(
        '#title'            => 'Host Submission Email',
        '#type'             => 'textfield',
        '#description'      => t('This is the email that gets sent host form submissions'),
        '#default_value'    => variable_get('dw_campaigns_host_submit_email', '')
    );

    $form['walking_specific']['dw_campaigns_host_forward_database_table'] = array(
        '#title'            => 'Table to attempt to add host email "forward" to',
        '#type'             => 'textfield',
        '#description'      => t('FPWR assigns hosts fpwr.org emails which it forwards to themr primary - leave empty if you don\'t want/need this'),
        '#default_value'    => variable_get('dw_campaigns_host_forward_database_table', '')
    );

    $form['walking_specific']['dw_campaigns_host_forward_domain_name'] = array(
        '#title'            => 'first.last@[this domain] will be the alias',
        '#type'             => 'textfield',
        '#description'      => t('if empty HTTP_HOST will be used'),
        '#default_value'    => variable_get('dw_campaigns_host_forward_domain_name', '')
    );


    $form['walking_specific']['dw_campaigns_fundraising_goal_override'] = array(
        '#title'            => 'Fundraising Goal Override',
        '#type'             => 'textfield',
        '#description'      => t('If this value > 0 it will be used for the thermometer "goal"'),
        '#default_value'    => variable_get('dw_campaigns_fundraising_goal_override', '0')
    );
    
    $form['walking_specific']['dw_campaigns_fundraising_goal_start_value'] = array(
        '#title'            => 'Fundraising Goal Starting Amount',
        '#type'             => 'textfield',
        '#description'      => t('If this value > 0 it will be used as the starting amount towards the thermometer "goal"'),
        '#default_value'    => variable_get('dw_campaigns_fundraising_goal_start_value', '0')
    );
    
    $form['walking_specific']['dw_campaigns_walking_event_help_text'] = array(
        '#title'            => 'Event Registration Help Text',
        '#type'             => 'textarea',
        '#description'      => t('Displayed during Page creation '),
        '#default_value'    => variable_get('dw_campaigns_walking_event_help_text', '')
    );
    
    $form['walking_specific']['dw_campaigns_walking_event_max'] = array(
        '#title'            => 'Event Registration Max Participants',
        '#type'             => 'textfield',
        '#description'      => t('Maximum number of participants'),
        '#default_value'    => variable_get('dw_campaigns_walking_event_max', '6')
    );
    
    $form['walking_specific']['dw_campaigns_walking_event_tos'] = array(
        '#title'            => 'Event Registration Terms of Service Text',
        '#type'             => 'textarea',
        '#description'      => t('Displayed during Page creation '),
        '#default_value'    => variable_get('dw_campaigns_walking_event_tos', '')
    );
   
    $form['walking_specific']['dw_campaign_hoster_response_subject'] = array(
        '#title'            => 'Event Host Request Email Response Subject',
        '#type'             => 'textfield',
        '#description'      => t('Subject of auto reply email sent to Hostee (when host request is made)'),
        '#default_value'    => variable_get('dw_campaign_hoster_response_subject', 'Thank you for expressing interest in hosting an event')
    );

    $form['walking_specific']['dw_campaign_hoster_response_body'] = array(
        '#title'            => 'Event Host Request Email Response Body',
        //'#type'             => 'textarea',
        '#type'             => 'text_format',
        '#format'           => variable_get('dw_campaigns_cfg_default_filter', null),
        '#description'      => t('Body of auto reply email sent to Hostee (when host request is made)'),

        '#default_value'    => variable_get('dw_campaign_hoster_response_body', 'You will receive follow up communication soon.')
    );

    $form['walking_specific']['dw_campaign_host_contact_subject'] = array(
        '#title'            => 'Event Host "contact" email subject',
        '#type'             => 'textfield',
        '#description'      => t('Subject of email sent when a hosts name is clicked on the events page'),
        '#default_value'    => variable_get('dw_campaigns_host_contact_subject', 'Host Contact Form Email')
    );

    $form['walking_specific']['dw_campaigns_pcp_create_host_notify'] = array(
        '#title'            => 'Personal Campaign Creation Host Notify Email',
        '#type'             => 'textarea',
        '#description'      => t('Body of email sent to Host when a PCP is created (# of participants)'),
        '#default_value'    => variable_get('dw_campaigns_pcp_create_host_notify', 'Number of Walk Event Participants {{event}{participants}}')
    ); 

    $form['walking_specific']['dw_campaigns_participation_type_blurb'] = array(
        '#title'            => 'Participation Type Blurb',
        '#type'             => 'textarea',
        '#description'      => t('These words will show up on the register_oss page above the participation types'),
        '#default_value'    => variable_get('dw_campaigns_participation_type_blurb', 'Receive a free t-shirt when you fundraise or donate a minimum of $40 per adult')
    ); 
    
    $form['walking_specific']['dw_campaigns_host_event_accepted_email_subject'] = array(
        '#title'            => 'Host Event Accepted Subject',
        '#type'             => 'textfield',
        '#description'      => t('Email Subject sent to host and co-host when their event is accepted'),
        '#default_value'    => variable_get('dw_campaigns_host_event_accepted_email_subject', 'Your event has been approved')
    ); 
    
    
    $form['walking_specific']['dw_campaigns_host_event_accepted_email'] = array(
        '#title'            => 'Host Event Accepted',
        '#type'             => 'textarea',
        '#description'      => t('Email sent to host and co-host when their event is accepted'),
        '#default_value'    => variable_get('dw_campaigns_host_event_accepted_email', 'Your event has been accepted, please login to manage your event')
    );
 
    $form['walking_specific']['dw_campaign_host_panel_message'] = array(
        '#title'            => 'Host Panel Message',
        //'#type'             => 'textarea',
        '#type'             => 'text_format',
        '#format'           => variable_get('dw_campaigns_cfg_default_filter', null),
        '#description'      => t('Message shown on the host panel to all hosts - this is where you can include links or new information)'),
        '#default_value'    => variable_get('dw_campaign_host_panel_message', '')
    );
    
    
    $form['nav_bar'] = array(
        '#type'         => 'fieldset',
        '#title'        => t('Nav Bar Links Settings'),
        '#description'  => t('Configure Nav Bar Links')
    );
    
    $form['nav_bar']['dw_campaigns_give_enabled'] = array(
        '#type'             => 'checkbox',
        '#title'            => t('Show "Give" link in main navigation menu'),
        '#description'      => t('Enables / Disabled "Give" link in menu bar'),
        '#default_value'    => variable_get('dw_campaigns_give_enabled', 1)
    );
    
    $form['nav_bar']['dw_campaigns_derby_general_donation_link'] = array(
        '#title'            => 'Address for "General Donation" link',
        '#type'             => 'textfield',
        '#description'      => t('URL for GIVE link on Banner Menu (and Give a General Donation button in Derby)'),
        '#maxlength'        => 250,
        '#default_value'    => variable_get('dw_campaigns_derby_general_donation_link', '')           
    );
    $form['nav_bar']['dw_campaigns_derby_about_np_link'] = array(
        '#title'            => 'Address for "About Non-profit" link (About PWSACO / About Us)',
        '#type'             => 'textfield',
        '#description'      => t('URL for ABOUT US links on Banner menu and other ABOUT buttons.'),
        '#maxlength'        => 250,
        '#default_value'    => variable_get('dw_campaigns_derby_about_np_link', '')           
    );

// signup email tokens fieldset
    $form['signup_tokens_fieldset'] = array(
        '#type'		=> 'fieldset',
        '#title'	=> t('Signup Email Tokens'),
        //'#description'	=> t(''),
        '#collapsible'	=> TRUE,
        '#collapsed'	=> TRUE,
    );

    $form['signup_tokens_fieldset']['tokens'] = array(
        '#type'		=> 'fieldset',
        '#value'	=> getContactTokens() . getEmailTokens(),
    );


// signup email fieldset
    $form['signup'] = array(
        '#type'         => 'fieldset',
        '#title'        => t('Signup Email'),
        '#description'  => t('Signup Email')
    );

    $form['signup']['dw_campaigns_signup_from'] = array(
        '#title'            => 'Email From Address',
        '#type'             => 'textfield',
        '#description'      => 'From Email, use format of:  user@host.com  -OR- John Smith &lt;user@host.com&gt;',
        '#default_value'    => variable_get('dw_campaigns_signup_from', '')
    );


    $form['signup']['dw_campaigns_signup_subject'] = array(
        '#title'            => 'Email Subject',
        '#type'             => 'textfield',
        '#description'      => t('Subject for Email sent upon account creation'),
        '#maxlength'        => 250,
        '#default_value'    => variable_get('dw_campaigns_signup_subject', '')           
    );

    $form['signup']['dw_campaigns_signup_body'] = array(
        '#title'            => 'Email Body',
        //'#type'             => 'textarea',
        '#type'             => 'text_format',
        '#format'           => variable_get('dw_campaigns_cfg_default_filter', null),
        '#description'      => t('Body of Email sent upon account creation'),
        '#default_value'    => variable_get('dw_campaigns_signup_body', '')           
    );

    $form['signup']['dw_campaigns_signup_words_node'] = array(
        '#title'            => 'node number of drupal page with signup "thanks"',
        '#type'             => 'textfield',
        '#description'      => t('page to redirect to'),
        '#default_value'    => variable_get('dw_campaigns_signup_words_node', '')           
    );



// default value email tokens fieldset
    $form['default_tokens_fieldset'] = array(
	'#type'		=> 'fieldset',
	'#title'	=> t('Thank You / Receipt Tokens'),
	//'#description'	=> t(''),
	'#collapsible'	=> TRUE,
	'#collapsed'	=> TRUE,
    );

    $form['default_tokens_fieldset']['tokens'] = array(
        '#type'             => 'fieldset',
        '#value'            => getContactTokens() . getTokens(),
    );

// default values (Thank You / Receipt emails) fieldset
    $form['defaults'] = array(
        '#type'             => 'fieldset',
        '#title'            => t('Campaign Content Type Defaults'),
        '#description'      => t('Configure the default values used for new campaigns')
    );
    
    $form['defaults']['dw_campaigns_default_pcp_intro_text'] = array(
        '#title'            => 'Personal Campaign Intro Text',
        //'#type'             => 'textarea',
        '#type'             => 'text_format',
        '#format'           => variable_get('dw_campaigns_cfg_default_filter', null),
        '#description'      => t('Default Personal Campaign Intro Text (tokens available: fundraising_campaign, user, contact)'),
        '#default_value'    => variable_get('dw_campaigns_default_pcp_intro_text', '')                   
    );

    
    $form['defaults']['dw_campaigns_default_thankyou_title'] = array(
        '#title'            => 'Thank You Title',
        '#type'             => 'textfield',
        '#description'      => t('Default System Generated Thank You Title'),
        '#maxlength'        => 250,
        '#default_value'    => variable_get('dw_campaigns_default_thankyou_title', '')           
    );
    
    $form['defaults']['dw_campaigns_default_thankyou_text'] = array(
        '#title'            => 'Thank You Text',
        //'#type'             => 'textarea',
        '#type'             => 'text_format',
        '#format'           => variable_get('dw_campaigns_cfg_default_filter', null),
        '#description'      => t('Default System Generated Thank You Text'),
        '#default_value'    => variable_get('dw_campaigns_default_thankyou_text', '')           
    );
    
    $form['defaults']['dw_campaigns_default_thankyou_footer'] = array(
        '#title'            => 'Thank You Footer',
        '#type'             => 'textfield',
        '#description'      => t('Default System Generated Thank You Footer'),
        '#maxlength'        => 250,
        '#default_value'    => variable_get('dw_campaigns_default_thankyou_footer', '')           
    );
           
    
            
    $form['defaults']['dw_campaigns_default_receipt_from_name'] = array(
        '#title'            => 'Receipt From Name',
        '#type'             => 'textfield',
        '#description'      => t('Default Receipt From Name'),
        '#maxlength'        => 250,
        '#default_value'    => variable_get('dw_campaigns_default_receipt_from_name', '')           
    );
    
    $form['defaults']['dw_campaigns_default_receipt_from_email'] = array(
        '#title'            => 'Receipt From Email',
        '#type'             => 'textfield',
        '#description'      => t('Default Receipt From Email'),
        '#maxlength'        => 250,
        '#default_value'    => variable_get('dw_campaigns_default_receipt_from_email', '')           
    );


    $form['defaults']['dw_campaigns_default_cc_receipt'] = array(
        '#title'            => 'Receipt CCs',
        '#type'             => 'textfield',
        '#description'      => t('Default Receipt CCs (Carbon Copy) - comma seperated'),
        '#maxlength'        => 250,
        '#default_value'    => variable_get('dw_campaigns_default_cc_receipt', '')           
    );
    
    $form['defaults']['dw_campaigns_default_bcc_receipt'] = array(
        '#title'            => 'Receipt BCCs',
        '#type'             => 'textfield',
        '#description'      => t('Default Receipt BCCs (Blind Carbon Copy) - comma seperated'),
        '#maxlength'        => 250,
        '#default_value'    => variable_get('dw_campaigns_default_bcc_receipt', '')           
    );

    $form['defaults']['dw_campaigns_default_receipt_subject'] = array(
        '#title'            => 'Receipt Email Subject (donor)',
        '#type'             => 'textfield',
        '#description'      => t('Default Receipt Email Subject'),
        '#maxlength'        => 250,
        '#default_value'    => variable_get('dw_campaigns_default_receipt_subject', '')           
    );

    $form['defaults']['dw_campaigns_default_receipt_text'] = array(
        '#title'            => 'Receipt Text (donor)',
        //'#type'             => 'textarea',
        '#type'             => 'text_format',
        '#format'           => variable_get('dw_campaigns_cfg_default_filter', null),
        '#description'      => t('Default Receipt Text'),
        '#default_value'    => variable_get('dw_campaigns_default_receipt_text', '')           
    );

    
// donation settings fieldset
    $form['notifications'] = array(
        '#type'             => 'fieldset',
        '#title'            => t('PCP Owner Notifications'),
        '#description'      => t('Notificaiton Emails (these go to PCP owners per notify settings)')
    );
// notifications fieldset
    $form['notifications']['dw_campaigns_default_receipt_subject_pcp_owner'] = array(
        '#title'            => 'Receipt (Notification) Email Subject (pcp owner)',
        '#type'             => 'textfield',
        '#description'      => t('Default Receipt Email Subject'),
        '#maxlength'        => 250,
        '#default_value'    => variable_get('dw_campaigns_default_receipt_subject_pcp_owner', '')           
    );

    $form['notifications']['dw_campaigns_default_receipt_text_pcp_owner'] = array(
        '#title'            => 'Receipt (Notification) Text (pcp owner)',
        //'#type'             => 'textarea',
        '#type'             => 'text_format',
        '#format'           => variable_get('dw_campaigns_cfg_default_filter', null),
        '#description'      => t('Default Receipt Text'),
        '#default_value'    => variable_get('dw_campaigns_default_receipt_text_pcp_owner', '')           
    );
    

    $form['notifications']['dw_campaigns_default_notification_weekly_subject_pcp_owner'] = array(
        '#title'            => 'Weekly Notification Email Subject (pcp owner)',
        '#type'             => 'textfield',
        '#description'      => t('Email Subject'),
        '#maxlength'        => 250,
        '#default_value'    => variable_get('dw_campaigns_default_notification_weekly_subject_pcp_owner', '')           
    );

    $form['notifications']['dw_campaigns_default_notification_weekly_text_pcp_owner'] = array(
        '#title'            => 'Weekly Notification Text (pcp owner)',
        //'#type'             => 'textarea',
        '#type'             => 'text_format',
        '#format'           => variable_get('dw_campaigns_cfg_default_filter', null),
        '#description'      => t('Text'),
        '#default_value'    => variable_get('dw_campaigns_default_notification_weekly_text_pcp_owner', '')           
    );

    $form['notifications']['dw_campaigns_default_notification_progress_subject_pcp_owner'] = array(
        '#title'            => 'Progress Notification Email Subject (pcp owner)',
        '#type'             => 'textfield',
        '#description'      => t('Email Subject'),
        '#maxlength'        => 250,
        '#default_value'    => variable_get('dw_campaigns_default_notification_progress_subject_pcp_owner', '')           
    );

    $form['notifications']['dw_campaigns_default_notification_progress_text_pcp_owner'] = array(
        '#title'            => 'Progress Notification Text (pcp owner)',
        //'#type'             => 'textarea',
        '#type'             => 'text_format',
        '#format'           => variable_get('dw_campaigns_cfg_default_filter', null),
        '#description'      => t('Text'),
        '#default_value'    => variable_get('dw_campaigns_default_notification_progress_text_pcp_owner', '')           
    );

    $form['notifications']['dw_campaigns_runner_signup_email'] = array(
        '#title'            => 'Runner Signup Email',
        '#type'             => 'text_format',
        '#format'           => variable_get('dw_campaigns_cfg_default_filter', null),
        '#description'      => t('Email sent to runners confirming signup'),
        '#default_value'    => variable_get('dw_campaigns_runner_signup_email', '')           
    );

    $form['notifications']['dw_campaigns_canadian_receipt_email'] = array(
        '#title'            => 'Canadian (PDF) receipt Email',
        '#type'             => 'text_format',
        '#format'           => variable_get('dw_campaigns_cfg_default_filter', null),
        '#description'      => t('Email sent to Canadians with the PDF attached'),
        '#default_value'    => variable_get('dw_campaigns_canadian_receipt_email', '')           
    );


// donation settings fieldset
    $form['donation_settings_fieldset'] = array(
        '#type'             => 'fieldset',
        '#title'            => t('Default Contribution'),
        '#description'      => t('Default values for contribution related fields')
    );

    $form['donation_settings_fieldset']['default_contribution_amount'] = array(
        '#title'            => 'Suggested Contribution Amount',
        '#type'             => 'textfield',
        '#description'      => t('Default Contribution Amount'),
        '#maxlength'        => 250,
        '#default_value'    => variable_get('default_contribution_amount', '100.00')         
    );

/*
    $form['privacy_fieldset'] = array(
        '#type'             => 'fieldset',
        '#title'            => t('Default Privacy Settings'),
        '#description'      => t('When someone donates to a PCP these are the default privacy settings on the form')
    );

    $form['privacy_fieldset']['privacy_anonymous']    = array(
        '#type'             => 'select',
        '#title'            => t('Make this donation anonymous to the user'),
        '#options'          => array(
            '0'             => 'No',
            '1'             => 'Yes',
        ),
        '#default_value'    => 0,
        '#required'         => true
    );

    $form['privacy_fieldset']['privacy_contact']    = array(
        '#type'             => 'select',
        '#title'            => t('Allow the user to contact me'),
        '#options'          => array(
            '0'             => 'No',
            '1'             => 'Yes',
        ),
        '#default_value'    => 1,
        '#required'         => true
    );
*/
    /**
     * Invitation text
     */

// invitation tokens fieldset
    $form['invitation_tokens_fieldset'] = array(
	'#type'		=> 'fieldset',
	'#title'	=> t('Invitation Email / Social Sharing Tokens'),
	//'#description'	=> t(''),
	'#collapsible'	=> TRUE,
	'#collapsed'	=> TRUE,
    );

    $form['invitation_tokens_fieldset']['tokens'] = array(
        '#type'		=> 'fieldset',
        '#value'	=> getTokens(),
    );


// invitation fieldset
    $form['invitation_text_fieldset'] = array(
        '#type'             => 'fieldset',
        '#title'            => t('Invitation Text'),
        '#description'      => t('When someone creates their personal contribution page they will be asked to email friends and family. The text below is the default text that the invitiation email will have. The user will be able to edit this before they send it.')
    );
    
    
    $form['invitation_text_fieldset']['dw-campaigns-invitation-text-from-email-name'] = array(
        '#title'            => 'From "name" for invitation email',
        '#type'             => 'textfield',
        '#default_value'    => variable_get('dw-campaigns-invitation-text-from-email-name', '')
    );

    $form['invitation_text_fieldset']['dw-campaigns-invitation-text-from-email-type'] = array(
        '#title'            => 'From Email Address Type',
        '#type'             => 'select',
        '#options'          => array(
            'use-global-address' => t('Use "From Email Address" admin setting'),
            'use-user-address'   => t('Use sending users "From Email Address"'),
        ),
        '#description'      => t('This setting is used on the "Share" page when sending emails on behalf of someone'),
        '#default_value'    => variable_get('dw-campaigns-invitation-text-from-email-type', 'use-global-address')
    );
    
    $form['invitation_text_fieldset']['dw-campaigns-invitation-text-from-email-address'] = array(
        '#title'            => 'From Email Address for invitation email',
        '#type'             => 'textfield',
        '#default_value'    => variable_get('dw-campaigns-invitation-text-from-email-address', '')
    );

    $form['invitation_text_fieldset']['dw-campaigns-invitation-subject'] = array(
        '#title'            => 'Subject line',
        '#description'      => 'DOES NOT USE TEMPLATE!  Only variable is:  !email_from_name',
        '#type'             => 'textfield',
        '#default_value'    => variable_get('dw-campaigns-invitation-subject', '!email_from_name would like you to look at this fundraising campaign')
    );

    $form['invitation_text_fieldset']['dw-campaigns-invitation-text'] = array(
        '#title'            => 'Default Text for "sharing" email',
        //'#type'             => 'textarea',
        '#type'             => 'text_format',
        '#format'           => variable_get('dw_campaigns_cfg_default_filter', null),
        '#default_value'    => variable_get('dw-campaigns-invitation-text', '')
    );
    
    $form['sharing_fieldset'] = array(
        '#type'             => 'fieldset',
        '#title'            => t('Sharing Default Text'),
        '#description'      => t('Default text for sharing messages')
    );

    $form['sharing_fieldset']['dw-campaigns-facebook-text'] = array(
        '#title'            => 'Default Text for "sharing" on Facebook',
        '#type'             => 'textfield',
        '#maxlength'        => 400,
        '#default_value'    => variable_get('dw-campaigns-facebook-text', '')
    );
    
    $form['sharing_fieldset']['dw-campaigns-twitter-text'] = array(
        '#title'            => 'Default Text for "sharing" tweet',
        '#description'      => '(120 chars, so there is room for link)',
        '#type'             => 'textfield',
        '#maxlength'        => 120,
        '#default_value'    => variable_get('dw-campaigns-twitter-text', '')
    );

    $form['thankyou_fieldset'] = array(
        '#type'             => 'fieldset',
        '#title'            => t('User Thank Yous Default Text'),
        '#description'      => t('Default text for Thank You messages')
    );
    $form['thankyou_fieldset']['dw_campaigns_user_thankyou_from_email_name'] = array(
        '#title'            => 'From "name" for User Initiated Thank You email',
        '#type'             => 'textfield',
        '#default_value'    => variable_get('dw_campaigns_user_thankyou_from_email_name', '')
    );
    $form['thankyou_fieldset']['dw_campaigns_user_thankyou_from_email_address'] = array(
        '#title'            => 'From "address" for User Initiated Thank You email',
        '#type'             => 'textfield',
        '#default_value'    => variable_get('dw_campaigns_user_thankyou_from_email_address', '')
    );
    $form['thankyou_fieldset']['dw_campaigns_user_thankyou_title'] = array(
        '#title'            => 'Thank You Subject',
        '#type'             => 'textfield',
        '#description'      => t('Default User Inititated Thank You Email Subject'),
        '#maxlength'        => 250,
        '#default_value'    => variable_get('dw_campaigns_user_thankyou_title', '')           
    );
    
    $form['thankyou_fieldset']['dw_campaigns_user_thankyou_text'] = array(
        '#title'            => 'Thank You Text',
        //'#type'             => 'textarea',
        '#type'             => 'text_format',
        '#format'           => variable_get('dw_campaigns_cfg_default_filter', null),
        '#description'      => t('Default User Inititiated Thank You Text'),
        '#default_value'    => variable_get('dw_campaigns_user_thankyou_text', '')           
    );

    $form['host_to_fundraisers_fieldset'] = array(
        '#type'             => 'fieldset',
        '#title'            => t('Host to Fundraisers Email Settings'),
    );
    $form['host_to_fundraisers_fieldset']['dw_campaigns_user_host_contact_fundraisers_from_name'] = array(
        '#title'            => 'From "name" used in the "From" field on Host to Fundraisers emails',
        '#type'             => 'textfield',
        '#default_value'    => variable_get('dw_campaigns_user_host_contact_fundraisers_from_name', '')
    );
    $form['host_to_fundraisers_fieldset']['dw_campaigns_user_host_contact_fundraisers_from_email'] = array(
        '#title'            => 'From "address" used in the "From" field on Host to Fundraisers emails',
        '#type'             => 'textfield',
        '#default_value'    => variable_get('dw_campaigns_user_host_contact_fundraisers_from_email', '')
    );

    $form['admin_receipt_fieldset'] = array(
        '#type'             => 'fieldset',
        '#title'            => t('Host to Fundraisers Email Settings'),
    );

    $form['admin_receipt_fieldset']['dw_campaigns_admin_receipt_from_email'] = array(
        '#title'            => 'Admin Receipt Generation - From Email',
        '#type'             => 'textfield',
        '#description'      => t('Admin Receipt From Email'),
        '#maxlength'        => 250,
        '#default_value'    => variable_get('dw_campaigns_admin_receipt_from_email', '')           
    );

    $form['admin_receipt_fieldset']['dw_campaigns_admin_receipt_text'] = array(
        '#title'            => 'Admin Receipt Generation - Email Text',
        //'#type'             => 'textarea',
        '#type'             => 'text_format',
        '#format'           => variable_get('dw_campaigns_cfg_default_filter', null),
        '#description'      => t('Admin Receipt Text'),
        '#default_value'    => variable_get('dw_campaigns_admin_receipt_text', '')           
    );

    $form['default_cc_fieldset'] = array(
        '#type'             => 'fieldset',
        '#title'            => t('Host to Fundraisers Email Settings'),
    );

    $form['default_cc_fieldset']['dw_campaigns_host_contact_fundraisers_default_cc'] = array(
        '#title'            => 'Host Contact -> Fundtraisers default CC',
        '#type'             => 'textfield',
        '#description'      => t('Email'),
        '#maxlength'        => 250,
        '#default_value'    => variable_get('dw_campaigns_host_contact_fundraisers_default_cc', '')           
    );

    $form['default_cc_fieldset']['dw_campaigns_admin_contact_hosts_default_cc'] = array(
        '#title'            => 'Admin Contact -> Hosts default CC',
        '#type'             => 'textfield',
        '#description'      => t('Email'),
        '#maxlength'        => 250,
        '#default_value'    => variable_get('dw_campaigns_admin_contact_hosts_default_cc', '')           
    );

    $form['host_request_services_fieldset'] = array(
        '#type'             => 'fieldset',
        '#title'            => t('Host Request Services'),
    );

    $form['host_request_services_fieldset']['dw_campaigns_host_request_services_email'] = array(
        '#title'            => 'Host Request Services From Email',
        '#type'             => 'textfield',
        '#description'      => t('Host Request Services From Email'),
        '#maxlength'        => 250,
        '#default_value'    => variable_get('dw_campaigns_host_request_services_email', '')           
    );

    $form['livedocx_fieldset'] = array(
        '#type'             => 'fieldset',
        '#title'            => t('DocX '),
    );

    $form['livedocx_fieldset']['dw_campaigns_livedocx_login'] = array(
        '#title'            => 'Login for livedocx.com',
        '#type'             => 'textfield',
        '#description'      => t(''),
        '#maxlength'        => 250,
        '#default_value'    => variable_get('dw_campaigns_livedocx_login', '')           
    );

    $form['livedocx_fieldset']['dw_campaigns_livedocx_password'] = array(
        '#title'            => 'Password for livedocx.com',
        '#type'             => 'textfield',
        '#description'      => t(''),
        '#maxlength'        => 250,
        '#default_value'    => variable_get('dw_campaigns_livedocx_password', '')           
    );

    $form['donors_forms'] = array(
        '#type'             => 'fieldset',
        '#title'            => t('Donor Forms')
    );

        $form['donors_forms']['dw_campaigns_donors_attending'] = array(
            '#type'             => 'checkbox',
            '#title'            => 'enable attendance questions',
            '#default_value'    => variable_get('dw_campaigns_donors_attending', '')
        );



    $form['#submit'][] = 'dw_campaigns_system_settings_form_submit';

    return system_settings_form($form);

}

// all fields on the admin settings page which are the WYSIWYG need to be listed here
function dw_campaigns_system_settings_form_submit(&$form, &$form_state) {
    $values = $form_state['values'];

    $possibles = array(
        'dw_campaign_hoster_response_body',
        'dw_campaigns_signup_body',
        'dw_campaigns_default_pcp_intro_text',
        'dw_campaigns_default_thankyou_text',
        'dw_campaigns_default_receipt_text',
        'dw_campaigns_default_receipt_text_pcp_owner',
        'dw_campaigns_default_notification_weekly_text_pcp_owner',
        'dw_campaigns_default_notification_progress_text_pcp_owner',
        'dw-campaigns-invitation-text',
        'dw_campaigns_user_thankyou_text',
        'dw_campaigns_runner_signup_email',
        'dw_campaigns_canadian_receipt_email',
        'splashr_html',
        'dw_campaigns_admin_receipt_text'
    );

    foreach($possibles as $key) {
        if(isset($values[$key]['value'])) {
            $form_state['values'][$key] = $values[$key]['value'];
        }
    }
}

function dw_campaigns_groups_admin_page() {
    $form           = drupal_get_form('dw_campaigns_groups_admin_form');
    $groups_form    = drupal_get_form('dw_campaigns_groups_admin_campaigns_form');

    return theme('dw_campaigns_groups_admin_page', array('form' => $form, 'groups_form' => $groups_form));
}

function dw_campaigns_groups_admin_form($form, &$form_state) {
    $form   = array();

    
    $form['grouping_top'] = array(
        '#type'         => 'fieldset',
        '#title'        => t('Create New Grouping'),
        //'#description'  => t('')
        '#collapsible'  => TRUE,
        '#collapsed'    => TRUE
    );
    $form['grouping_top']['add_name'] = array(
        '#type'             => 'textfield',
        '#title'            => t('New Groupings Name'),
        '#description'      => t('Campaigns are assigned a single group.  Example Groupings would be "2011 season" "2012 season" or "2013 Fall"'),
        '#default_value'    => ''
    );

    $form['grouping_top']['submit']      = array(
        '#type'             => 'submit',
        '#value'            => t('Submit'),
        '#attributes'       => array('class'=> array('btn'))
    );
    return $form;
}

function dw_campaigns_groups_admin_form_submit(&$form, &$form_state) {
    $values = $form_state['values'];


    $add_name = $values['add_name'];
    if(strlen($add_name) > 0) {


        $params = array(
            'name' => $add_name,
            'description' => $add_name,
        );
        $res = _dw_civicrm_tag_create($params); 

        if(is_null($res->tag_id)) {

            drupal_set_message(t('Civi Failed Adding Grouping Tag named @add_name', array('@add_name' => $add_name)), 'error');

        } else {

            db_insert('dw_campaigns_groups')->fields(array('group_name' => $add_name, 'group_tag_id' => $res->tag_id))->execute();

            drupal_set_message(t('Added Grouping named @add_name', array('@add_name' => $add_name)), 'status');
        }

    } else {

        drupal_set_message(t('Must supply a name for the new grouping'), 'error');

    }
}

function dw_campaigns_groups_admin_campaigns_form($form, &$form_state) {

    $form   = array();
    $pairs  = array();

    $results = db_select('node', 'n')->fields('n')->condition('type', 'dw_campaigns_derby', '=')->execute();
    $options    = dw_campaigns_get_group_names();

    foreach($results as $node_reference) {
        $node = node_load($node_reference->nid);
        $key = 'dw_' . $node->nid;
        $form['campaigns'][$key]['name'] = array(
            '#markup'       => $node->title
        );

        $picked             = isset($node->field_dw_grouping['und']['0']['value']) ? $node->field_dw_grouping['und']['0']['value'] : 0;

        $pairs[$node->nid]  = $picked;

        $form['campaigns'][$key]['group_' . $key] = array(
            '#type'             => 'select',
            '#options'          => $options,
            '#default_value'    => $picked
        );

    }

    _dw_campaigns_old_group_values($pairs);

    $form['submit']      = array(
        '#type'             => 'submit',
        '#value'            => t('Submit'),
        '#attributes'       => array('class'=> array('btn'))
    );

    return $form;
}

function _dw_campaigns_old_group_values($input = NULL) {
    static $old = NULL;

    if(is_null($old)) {
        $old = $input;
    }

    return $old;
}

function dw_campaigns_groups_admin_campaigns_form_submit(&$form, &$form_state) {
    global $ignore_node_update;
    $ignore_node_update = TRUE; // suppress sync with civi, we are only changing a group which is not a field stored in the civi contribution page table
 

    $values = $form_state['values'];

    $old_values = _dw_campaigns_old_group_values();

    $remove = array();
    $add    = array();

    $count = 0;

    foreach($values as $key => $value) {
        if(strncmp($key, 'group_dw_', 9) == 0) {
            $idx = substr($key, 9);

            // we only update records that changed
            if(isset($old_values[$idx]) && $old_values[$idx] != $value) {

                $count++;

                $old = $old_values[$idx];

                $node = node_load($idx);
    
                // drupal doesn't always pass initialized values
                if(!isset($node->field_dw_grouping)) {
                    $node->field_dw_grouping = array('und' => array(0 => array('value' => 0)));
                }
    
                $node->field_dw_grouping['und']['0']['value'] = $value;
                node_save($node);

                $entity_id = dw_campaigns_campaign_id_to_contribution_page_id($idx);

                if(!isset($remove[$old])) {
                    $remove[$old] = array();
                }
                
                $remove[$old][] = $entity_id;


                if(!isset($add[$old])) {
                    $add[$value] = array();
                }
                
                $add[$value][] = $entity_id;

            }
        }
    }

    if($count > 0) {
        foreach($remove as $id => $ents) {
            $tag = dw_campaigns_group_to_tag_id($id);

            $params = array();
            $params['tag_id_holder'] = $tag;

            foreach($ents as $ent) {
                $params['entity_id_' . $ent] = $ent;
            }

            _dw_civicrm_tag_entity_remove($params);
        }

        foreach($add as $id => $ents) {
            $tag = dw_campaigns_group_to_tag_id($id);

            $params = array();
            $params['tag_id_holder'] = $tag;

            foreach($ents as $ent) {
                $params['entity_id_' . $ent] = $ent;
            }

            _dw_civicrm_tag_entity_add($params);
        }
     }

    drupal_set_message(t('Updated @count campaign groupings', array('@count' => $count)), 'status');

}

function template_preprocess_dw_campaigns_groups_admin_campaigns_form(&$variables) {
    $data = array();

    foreach(element_children($variables['form']['campaigns']) as $key) {
        $campaign_row = &$variables['form']['campaigns'][$key];

        $row = new stdClass;
        $row->name  = drupal_render($campaign_row['name']);
        $row->group = drupal_render($campaign_row['group_' . $key]);

        $data[$key] = $row;
    }

    $variables['group_data']    = $data;
    $variables['form_submit']   = drupal_render_children($variables['form']);
}

function getEmailTokens() {
    return nl2br(
'
USER - User that is logged in
{{user}{mail}}
{{user}{name}}
{{user}{pass}}
{{user}{displayname}}

ORG - Organization values (From admin module)
{{org}{name}}
{{org}{name_abbreviated}}
{{org}{homepageurl}}
{{org}{website}}
');
    
}

function getContactTokens() {
    return nl2br(
'CONTACT - information about the user registering, or donator donating
{{contact}{contact_id}}
{{contact}{sort_name}}
{{contact}{display_name}}
{{contact}{first_name}}
{{contact}{last_name}}
{{contact}{street_address}}
{{contact}{supplemental_address_1}}
{{contact}{city}}
{{contact}{postal_code}}
{{contact}{state_province_name}}
{{contact}{state_province}}
{{contact}{country}}
{{contact}{phone}}
{{contact}{email}}
{{contact}{world_region}}
');
   
}

function getTokens() {
    return nl2br(
'
FUNDRAISING_CAMPAIGN - these are the values from the derby / walk content
{{fundraising_campaign}{type}}
{{fundraising_campaign}{uid}}
{{fundraising_campaign}{status}}
{{fundraising_campaign}{created}}
{{fundraising_campaign}{changed}}
{{fundraising_campaign}{title}}
{{fundraising_campaign}{body}}
{{fundraising_campaign}{teaser}}
{{fundraising_campaign}{revision_timestamp}}
{{fundraising_campaign}{name}}
{{fundraising_campaign}{field_dw_fundraising_goal}}
{{fundraising_campaign}{fundraising_goal_formatted}}
{{fundraising_campaign}{raised_amount}}
{{fundraising_campaign}{field_dw_campaign_status}}
{{fundraising_campaign}{field_dw_event_date}}
{{fundraising_campaign}{field_dw_event_time}}
{{fundraising_campaign}{field_dw_event_location}}
{{fundraising_campaign}{field_dw_address}{city}}
{{fundraising_campaign}{field_dw_address}{street}}
{{fundraising_campaign}{field_dw_address}{postal_code}}

CONTRIBUTION - values from a donation
{{contribution}{id}}
{{contribution}{contact_id}}
{{contribution}{contribution_type_id}}
{{contribution}{contribution_page_id}}
{{contribution}{payment_instrument_id}}
{{contribution}{receive_date}}
{{contribution}{total_amount}}
{{contribution}{invoice_id}}
{{contribution}{currency}}
{{contribution}{source}}

USER - User that is logged in
{{user}{uid}}
{{user}{name}}
{{user}{pass}}
{{user}{mail}}
{{user}{displayname}}
{{user}{created}}
{{user}{access}}
{{user}{login}}
{{user}{status}}
{{user}{timezone}}
{{user}{hostname}}
{{user}{timestamp}}
{{pcp}{title}}
{{pcp}{intro_text}}
{{pcp}{page_text}}
{{pcp}{donate_link_text}}
{{pcp}{contribution_page_id}}
{{pcp}{goal_amount}}
{{pcp}{raised_amount}}
{{pcp}{donation_count}}
{{pcp}{url}}

PCP_CONTACT - fundraising page owner (civicrm info)
{{pcp_contact}{contact_id}}
{{pcp_contact}{contact_type}}
{{pcp_contact}{sort_name}}
{{pcp_contact}{display_name}}
{{pcp_contact}{first_name}}
{{pcp_contact}{last_name}}
{{pcp_contact}{street_address}}
{{pcp_contact}{city}}
{{pcp_contact}{postal_code}}
{{pcp_contact}{geo_code_1}}
{{pcp_contact}{geo_code_2}}
{{pcp_contact}{state_province_id}}
{{pcp_contact}{state_province_name}}
{{pcp_contact}{state_province}}
{{pcp_contact}{country}}
{{pcp_contact}{phone}}
{{pcp_contact}{email}}

PCP_USER = fundfaising page owners (drupal info; used for displayname)
{{pcp_user}{uid}}
{{pcp_user}{name}}
{{pcp_user}{pass}}
{{pcp_user}{mail}}
{{pcp_user}{displayname}}
{{pcp_user}{created}}
{{pcp_user}{access}}
{{pcp_user}{login}}
{{pcp_user}{status}}
{{pcp_user}{timezone}}
{{pcp_user}{hostname}}
{{pcp_user}{timestamp}}

ORG - Organization values (From admin module)
{{org}{name}}
{{org}{name_abbreviated}}
{{org}{homepageurl}}
{{org}{website}}
');
    
}


/**
 DW Campaigns Emails
 * admin/reports/dw_emails/
 * admin/reports/dw_emails/csv/

 */
function dw_campaigns_reports_email() {

    $all = array();

    $result = db_query("SELECT mail FROM users");

    foreach($result as $users) 
    {
      $clean_email = trim($users->mail);
      if(strstr($clean_email, "@") === FALSE) {
          continue;
      }

      $all[] = $clean_email;
    }

    $emails = implode("\n", $all);

    return '<a href="/admin/reports/dw_emails/csv">Download CSV</a><br><br>'.nl2br($emails);
}

function dw_campaigns_reports_email_csv(){

    $csvDataArray = array();

    $header = array(
        'userName',
        'emailAddress'
    );

    $csvDataArray[] = $header;

    $form = array();

    $result = db_query("SELECT * FROM users");
    foreach($result as $users){
        $clean_email = trim($users->mail);
        if(strstr($clean_email, "@") === FALSE) {
            continue;
        }

        $csvData = array(
            $users->name,
            $clean_email
        );

        $csvDataArray[] = $csvData;
    }

    return output_csv('user_email_addresses_'.date('Y_m_d').'.csv', $csvDataArray );

}




/**
 DW Active Campaigns
 *
*/
function dw_campaigns_active_campaigns() {

    $campaigns  = dw_campaigns_get_active_campaigns(FALSE);

    return theme('dw_campaigns_reports_active_campaign', array('campaigns' => $campaigns));
}

function dw_campaigns_active_campaigns_csv(){
    //prepare data for csv output.
    $campaigns = dw_campaigns_get_active_campaigns();
    $csvDataArray = array();

        //order matters! This is the order in which field data is expected.
    $header = array(
        'title',
        'location',
        'city',
        'state',
        'status',
        'dateFrom',
        'dateTo',
        'hostName',
        'hostEmail',
        'hostPhone',
        'hostAddress',
        'currency',
        'otherDonations',
        'goal'
    );

    $csvDataArray[] = $header;

    //and this is where we generate the field date for the csv, the order here should reflect the order of the headers above.
    foreach( $campaigns as $campaign ){
        $hostNodeId = $campaign->{'field_dw_host_primary'}['und'][0]['value'];

        //get the host's information for this campaign
        $hostData   = node_load( $hostNodeId );

        $hostName   = $hostData->{'field_dw_host_name'}['und'][0]['value'];
        $hostPhone  = $hostData->{'field_dw_host_phone'}['und'][0]['value'];
        $hostEmail  = $hostData->{'field_dw_host_email'}['und'][0]['value'];
        $hostPostal = $hostData->{'field_dw_host_postal'}['und'][0]['value'];
        $hostState  = $hostData->{'field_dw_host_province'}['und'][0]['value'];
        $hostCity   = $hostData->{'field_dw_host_city'}['und'][0]['value'];
        $hostStreet = $hostData->{'field_dw_host_street'}['und'][0]['value'];


        $csvData = array(
            $campaign->title,
            $campaign->{'field_dw_event_location'}['und'][0]['value'],
            $campaign->{'field_dw_address_city'}['und'][0]['value'],
            $campaign->{'field_dw_address_province'}['und'][0]['value'],
            $campaign->{'field_dw_campaign_status'}['und'][0]['value'],
            $campaign->{'field_dw_date_range'}['und'][0]['value'],
            $campaign->{'field_dw_date_range'}['und'][0]['value2'],
            $hostName,
            $hostEmail,
            $hostPhone,
            $hostStreet.' '.$hostCity.', '.$hostState.' '.$hostPostal,
            $campaign->{'field_dw_currency'}['und'][0]['value'],
            $campaign->{'field_dw_other_donations'}['und'][0]['value'],
            $campaign->{'field_dw_fundraising_goal'}['und'][0]['value'],
        );

        $csvDataArray[] = $csvData;
    }

    //send csv to browser as a download
    return output_csv('active_campaigns_'.date("Y_m_d").'.csv', $csvDataArray );
}





/**
 DW Offline Donations
 *  admin/reports/dw_offline
 *  admin/reports/dw_offline/%
 *  admin/reports/dw_offline/%/%
        -> dw_campaigns_offline_donations($selected = NULL, $mode = 'SCREEN')
            when selected == null, display a selection form ( dw_campaigns_offline_donations_select_form )
            which provides us a way to select an event, and an output format. (screen or csv)
*/

function dw_campaigns_offline_donations($selected = NULL, $mode = 'SCREEN') {

    $form               = NULL;
    $selected_campaign  = NULL;
    $campaign           = NULL;
    $campaigns          = dw_campaigns_get_active_campaigns(TRUE);
    $donations          = array();
    $found              = FALSE;
    $options            = array();

    $headers = array(
        array(
            'data' => t('select'),
        ),
        array(
            'data' => t('Donor First Name'),
        ),
        array(
            'data' => t('Donor Last Name'),
            'field' => 'last_name',
            'sort'  => 'asc'
        ),
        array(
            'data' => t('Email'),
        ),
/*
        array(
            'data' => t('Address'),
        ),
*/
        array(
            'data' => t('Street Address'),
        ),
        array(
            'data' => t('City'),
        ),
        array(
            'data' => t('State / Province'),
        ),
        array(
            'data' => t('Postal'),
        ),
        array(
            'data' => t('Check#'),
        ),
        array(
            'data' => t('Donation Amount'),
            'field' => 'donation_amount'
        ),
        array(
            'data' => t('PCP id'),
        ),
        array(
            'data' => t('Honor Roll'),
        ),
/*
        array(
            'data' => t('Status'),
        ),
*/
        array(
            'data' => t('Trxn ID'),
        ),
        array(
            'data' => t('Non Ded?'),
        ),
        array(
            'data' => t('Action')
        )
    );

    foreach($campaigns as $contribution_page => $campaign) {
        if(is_null($selected)) {
    
        }
    
        if($selected == $campaign->nid) {
            $found  = TRUE;
            $selected_campaign = $campaign;
        }
    
        $options[$campaign->nid] = $campaign->title;
    }

    if($selected == 'ALL') {
        $found = TRUE;
    }

    $donations      = array();
    $donation_ids   = array();

    if($found) {
        if($selected == 'ALL') {

            $results    = db_select('dw_campaigns_offline_donation', 'ol')->fields('ol')->condition('deleted', '0', '=')->condition('contribution_id', '-1', '=')->extend('TableSort')->orderByHeader($headers)->execute();

        } else {

            $results    = db_select('dw_campaigns_offline_donation', 'ol')->fields('ol')->condition('deleted', '0', '=')->condition('contribution_id', '-1', '=')->condition('campaign', $selected_campaign->nid, '=')->extend('TableSort')->orderByHeader($headers)->execute();

        }

        foreach($results as $result) {
            $donations[$result->offline_id] = $result;
            $donation_ids[$result->offline_id] = '';
        }
 
        $selform    = drupal_get_form('dw_campaigns_offline_donations_select_form', $options, $selected);
        $form       = drupal_get_form('dw_campaigns_reports_campaign_offline_donations_form', $donation_ids, $selected_campaign);

    } else {
        $campaign   = NULL;
        $form       = drupal_get_form('dw_campaigns_offline_donations_select_form', $options);
        return drupal_render($form);
    }





    //this provides the 'select all' functionality requested for importing offline donation data from this report.
    drupal_add_js('
        var all_selected = false;

        function select_all_donations(){
                    var contents = document.getElementsByClassName("offline-donations")[0].children[1].children[1];

                    all_selected = !all_selected;

                    for( var i = 0; i < contents.children.length; i++ ){

                        contents.children[i].children[0].children[0].children[0].checked = all_selected;
                    }
        }

        window.addEventListener("load", function(){
            var select_header = document.getElementsByClassName("offline-donations")[0].children[1].children[0].children[0].children[0]
                select_header.innerHTML = "<a href=\"#\" onclick=\"select_all_donations()\">ALL</a>";

    });','inline');






    return theme('dw_campaigns_reports_campaign_offline_donations', array('campaign' => $campaign, 'form' => $form, 'donations' => $donations, 'selform' => $selform, 'headers' => $headers, 'mode' => $mode));
}

function dw_campaigns_offline_donations_select_form($form, &$form_state, $options, $selected = NULL) {
    $form = array();

    $t = array();
    $t[''] = '-- Please Select a Location--';

    $t['ALL'] = 'All';

    asort($options);

    $form['options']     = array(
        '#type'             => 'select',
        '#title'            => t('Event Location'),
        '#options'          => $t + $options,
        '#default_value'    => $selected
    );

    $form['mode']        = array(
        '#type'             => 'select',
        '#title'            => t('Output Type'),
        '#options'          => array('SCREEN' => 'Table', 'CSV' => 'CSV'),
    );

    $form['submit']      = array(
        '#type'             => 'submit',
        '#value'            => t('Submit'),
        '#attributes'       => array('class'=> array('btn'))
    );

    return $form;
}

function dw_campaigns_offline_donations_select_form_validate(&$form, &$form_state) {

}

function dw_campaigns_offline_donations_select_form_submit(&$form, &$form_state) {
    $values = $form_state['values'];

    drupal_goto('admin/reports/dw_offline/' . $values['options'] . '/' . $values['mode']);
    die;
}

function dw_campaigns_reports_campaign_offline_donations_form($form, &$form_state, $donation_ids, $campaign) {
    $form   = array();

    $form['nid'] = array(
        '#type'             => 'hidden',
        '#value'            => $campaign->nid
    );

    $form['donations']  = array(
        '#type'             => 'checkboxes',
        '#options'          => $donation_ids
    );

    $form['actions']    = array(
        '#type'             => 'select',
        '#options'          => array('' => t('Choose an Action'), 'import' => t('Import Donation to Civi'), 'delete' => t('Delete'))
    );

    $form['submit']     = array(
        '#type'             => 'submit',
        '#value'            => t('Submit'),
        '#attributes'       => array('class'=> array('btn'))
    );

    return $form;
}

function dw_campaigns_reports_campaign_offline_donations_form_validate(&$form, &$form_state) {

    $valid_actions = array(
        'import' => '1',
        'delete' => '1',
    );


    $values = $form_state['values'];
    
    $action = $values['actions'];
    if(!isset($valid_actions[$action])) {
        form_set_error('actions', t('Must select an action!'));
    }

}

function dw_campaigns_reports_campaign_offline_donations_form_submit(&$form, &$form_state) {
    $values = $form_state['values'];

    $change = array();
    foreach($values['donations'] as $key => $value) {
        if($value!==0) {
            $change[$key]   = $value;
        }
    }

    $rows_changed = count($change);
    $id_list = array_keys($change);

    if($rows_changed > 0) {
        if($values['actions'] == 'delete') {

            db_update('dw_campaigns_offline_donation')->fields(array('deleted' => '1'))->condition('offline_id', $id_list, 'IN')->execute();
            drupal_set_message(t('Deleted @num offline donations', array('@num' => $rows_changed)));
        } 

        if ($values['actions'] == 'import') {
            $results = db_select('dw_campaigns_offline_donation','d')->fields('d')->condition('offline_id', $id_list, 'IN')->execute();
            foreach($results as $result) {

                $offline_id = $result->offline_id;

                $ids        = dw_campaigns_offline_donation_post($result);  
                db_update ('dw_campaigns_offline_donation')->fields(array('contribution_id' => $ids['contribution_id']))->condition('offline_id', $offline_id, '=')->execute();

            }

            drupal_set_message(t('Imported @num offline donations', array('@num' => $rows_changed)));
        }

    }

}

function dw_campaigns_offline_donation_post($row) {
    $values             = (array) $row;

    $campaign           = node_load($values['campaign']);

    // $campaign is the drupal node, not an id
    $contributionType   = dw_campaigns_create_contribution_type_for_campaign($campaign);
    $contributionPage   = dw_campaigns_get_contribution_page_for_campaign($campaign);

    $countries_longname = _dw_civicrm_pseudoconstant_country();
    // turn country 'id' 1228 (united states) into 'US'
    $countries          = _dw_civicrm_pseudoconstant_countryIsoCode();
    $countryID          = $values['country'];
    $country_iso_code   = isset($countries[$countryID])?$countries[$countryID]:'';

    $states             = _dw_civicrm_pseudoconstant_stateprovince();
    $state              = isset($states[$values['state']])?$states[$values['state']]:'';

    // we already did all our checks before saving
    $currency           = $values['currency'];
    $remote_addr        = $values['remote_addr'];
    
    $invoice_id         = dw_campaigns_invoice_generate_id($currency);

    $invoice_serial     = dw_campaigns_invoice_generate_serial($currency, 2); // mark it as "don't send / offline"
    $invoice_id         .= '-' . $invoice_serial;

    $values['invoice_serial'] = $invoice_serial;


    $params['amount']                           = $values['donation_amount'];
    $params['amount_other']                     = $values['donation_amount'];
    $params['custom_1']                         = '';
    $params['ip']                               = $remote_addr;
    $params['ip_address']                       = $remote_addr;
    $params['amount_level']                     = '';
    $params['currencyID']                       = $currency;
    $params['payment_action']                   = 'Sale';
    $params['invoiceID']                        = $invoice_id;
    $params['is_pay_later']                     = 0;
    $params['is_recur']                         = '';

    $params['first_name']                       = $values['first_name'];
    $params['middle_name']                      = '';
    $params['last_name']                        = $values['last_name'];
    $params['email']                            = $values['email'];
    $params['street_address']                   = $values['address_1'];
    $params['city']                             = $values['city'];
    $params['state_province']                   = $state;
    $params['postal_code']                      = $values['postal_code'];
    $params['country']                          = $country_iso_code;

// home address
    $params['email-1']                          = $values['email'];
    $params['street_address-1']                 = $values['address_1'];
    $params['city-1']                           = $values['city'];
    $params['state_province-1']                 = $state;
    $params['postal_code-1']                    = $values['postal_code'];
    $params['country-1']                        = $country_iso_code;

// billing address
    $params['email-5']                          = $values['email'];
    $params['street_address-5']                 = $values['address_1'];
    $params['city-5']                           = $values['city'];
    $params['state_province-5']                 = $state;
    $params['postal_code-5']                    = $values['postal_code'];
    $params['country-5']                        = $country_iso_code;

    $params['contributionType_name']            = $contributionType->name;
    $params['contributionType_accounting_code'] = $contributionType->accounting_code;
    $params['contributionPageID']               = $contributionPage->id;

    /* since we are using the REST API we need to pass it the fields we want to dedupe on */
    foreach($params as $key => $value) {
        $dedupe['fields_' . $key] = $value;
    }

    $dedupe = array_merge($dedupe, $params);
    $contactID = _dw_campaigns_dwutils_get_deduped_contact_id($dedupe);


    // add in some fields we learn of now, so submit can use them
    $values['contact_id'] = $contactID;
    $values['invoice_id'] = $invoice_id;

    // use the supplied display name OR do this if none is supplied
    $nickname           = ucfirst($values['first_name']) . ' ' . ucfirst($values['last_name']);
    if(!empty($values['displayname'])) {
        $nickname       = $values['displayname'];
    }

    $params                             = array();

    if($values['non_deductible'] == 1) {
        $params['non_deductible_amount'] = $values['donation_amount'];
    }

    $params['contact_id']               = $values['contact_id'];
    $params['contribution_page_id']     = $contributionPage->id;
    $params['contribution_type_id']     = $contributionType->id;

    // 1 = completed 2 = pending 3 = failed^M
    $params['contribution_status_id']   = 1;

    $params['payment_instrument_id']    = 1;
    $params['pcp_made_through_id']      = $values['pcp_id'];
    $params['pcp_id']                   = $values['pcp_id'];
    $params['pcp_display_in_roll']      = $values['include_in_honor_roll'];

    $params['currency']                 = $contributionPage->currency;

    if(isset($values['donation-currency'])) {
        $params['currency']             = $values['donation-currency'];
    }

    $params['pcp_roll_nickname']        = $nickname;
    $params['pcp_personal_note']        = isset($values['contribution_comments']) ? $values['contribution_comments'] : '';
    $params['receive_date']             = $values['receive_date'];
    $params['total_amount']             = $values['donation_amount'];
    $params['amount']                   = $values['donation_amount'];
    $params['invoice_id']               = $values['invoice_id'];
    $params['trxn_id']                  = $values['trxn_id'];
    $params['source']                   = "DW Campaigns Module (manual add)";
    $params['is_pay_later']             = 1;

    if($values['payment_instrument'] == 'check') {
        $params['payment_instrument_id'] = 4;
        $params['check_number']          = $values['payment_check_number'];
    }




    $contribution = _dw_civicrm_contribution_create($params);
    if(is_null($contribution->id)) {
        global $dw_campaign_module_path;

        drupal_set_message(t('An unknown error occured while creating contribution.  Contact Support with ID: INV-') . $values['invoice_id']);
        @mkdir($dw_campaign_module_path . "/failed_invoices");
        file_put_contents($dw_campaign_module_path . "/failed_invoices/" . $values['invoice_id'], serialize($params));
    }



    $params['contribution_id']  = $contribution->id;

    dw_campaigns_invoice_update_receipt($params['currency'], $values['invoice_serial'], $params['contribution_id']);

    $softContribution           = _dw_civicrm_contributionsoft_create($params);
    $softcont_id                = $softContribution->id;

    $ret = array('contribution_id' => $contribution->id,'softcontribution_id' => $softcont_id);
    
    return $ret;
}















function dw_campaigns_reports_event_summary($campaign_id = NULL, $mode = 'SCREEN') {
    $report_rows    = array();

    $campaigns                 = dw_campaigns_get_active_campaigns('nodeid');

    $campaigns_by_contribution_page_id  = dw_campaigns_get_active_campaigns(TRUE);

    $i = 0;

    foreach($campaigns as $campaign_id => $campaign) {
        $pcp_count                  = 0;
        $participant_count          = 0;
        $donations_count            = 0;
        $donations_average          = 0;
        $online_total               = 0; // 'online' below
        $offline_pending_total      = 0;
        $offline_total              = 0; // 'offline_already_imported + 'offline' (pending)
        $offline_pending_count      = 0; // 'offline_pending_count'
        $total_total                = 0; 

        $participant_count  = count((array)dw_campaign_get_event_participants_by_campaign($campaign_id));

        $res = _dw_campaigns_get_pcps_for_campaign($campaign);

        foreach($res as $pcp) {
                $pcp_count              += 1;

                $dummy                  = new stdClass;
                $dummy->id              = $pcp->id;

                $totals                 = dw_campaigns_get_contribution_total_for_pcp($dummy);
                $donations_count        += $totals['count'];
                $online_total           += $totals['online'];
                $offline_pending_total  += $totals['offline'];
                $offline_total          += $totals['offline'] + $totals['offline_already_imported'];
                $offline_pending_count  += $totals['offline_pending_count'];
                $total_total            += $totals['total'];
        }

        if($donations_count == 0) {
            $donations_count = 1;
        }

        $donations_average = $total_total / $donations_count;

        $hosts      = dw_campaigns_get_campaign_hosts($campaign, TRUE);
        $event_date = isset($campaign->field_dw_date_range['und']['0']['value']) ? $campaign->field_dw_date_range['und']['0']['value'] : 'None specified';

        $report_rows[] = array(
            'location'              => $campaign->title,
            'host'                  => implode(',', $hosts),
            'event_date'            => substr($event_date, 0, 10),
            'pcp_count'             => $pcp_count,
            'participant_count'     => $participant_count,
            'donations_count'       => $donations_count,
            'donations_average'     => sprintf("%.02f", $donations_average),
            'online_total'          => sprintf("%.02f", $online_total),
            'offline_total'         => sprintf("%.02f", $offline_total),
            'offline_pending_total' => sprintf("%.02f", $offline_pending_total),
            'offline_pending_count' => $offline_pending_count,
            'total_total'           => sprintf("%.02f", $total_total)
        );
        $i++;
if($i==50) {
}
    }

    $headers = array(
        array(
            'data' => t('Location'),
        ),
        array(
            'data' => t('Host'),
        ),
        array(
            'data' => t('Event Date'),
        ),
        array(
            'data' => t('#Pcps'),
        ),
        array(
            'data' => t('#Participants'),
        ),
        array(
            'data' => t('#Donations'),
        ),
        array(
            'data' => t('Avg Donation'),
        ),
        array(
            'data' => t('Total Raised Online'),
        ),
        array(
            'data' => t('Total Raised Offline'),
        ),
        array(
            'data' => t('# Offline Pending'),
        ),
        array(
            'data' => t('Total Raised'),
        ),

    );

    return theme('dw_campaigns_reports_event_summary', array('headers' => $headers, 'rows' => $report_rows));
    
}

function dw_campaigns_reports_event_registration($arg = NULL, $mode = 'SCREEN') {
    global $dw_campaign_module_path;


    $groups = dw_campaigns_get_group_names(FALSE);
    $active     = dw_campaigns_get_active_campaigns('nodeid');


// select box
    $participants_campaigns = dw_campaign_get_event_participants_campaigns();
    $options                = array();
    $activec                = array(); // this will only have the active campaigns listed
    $campaigns              = array();

    foreach($participants_campaigns as $campaign_id) {
        $node = node_load($campaign_id);

        if(empty($node->title)) {
            continue;
        }

        $group = isset($node->field_dw_grouping['und']['0']['value']) ? $node->field_dw_grouping['und']['0']['value'] : 0;

        $groupName = isset($groups[$group]) ? '_(' . $groups[$group] . ') ' : '';

        $campaigns[$campaign_id] = $node;
        $options[$campaign_id] = $groupName . $node->title;

        if(isset($active[$campaign_id])) {
            $activec[$campaign_id] = $groupName . $node->title;
        }
    }

    asort($options);

    $form           = drupal_get_form('dw_campaigns_reports_event_registration_form', $options, $arg, $mode);

drupal_add_css('#dw-campaigns-reports-event-registration-form .form-type-select { display:inline-block; } #dw-campaigns-reports-event-registration-form .form-item { border-width:0px; }', $options['type'] = 'inline');

    $list   = array();

    //$master_list = $options;
    $master_list = $activec;

    if(strcasecmp($arg, 'ALL') == 0) {
        $list = $master_list; 
    } else {
        // when we load the initital page we don't have an arg
        if(isset($master_list[$arg])) {
            $list[$arg] = $master_list[$arg];
        }
    }

    return dw_campaigns_reports_event_registration_from_list($list, $mode, $campaigns, $arg, $form);
}

function dw_campaigns_host_reports_event_registration($event_id, $mode) {
    $list = array();

    $node = node_load($event_id);

    $list[$event_id] = $node->title;
    $campaigns[$event_id] = $node;

    return dw_campaigns_reports_event_registration_from_list($list, $mode, $campaigns);
}

function dw_campaigns_reports_event_registration_from_list($list, $mode, $campaigns = array(), $arg ='', $form = NULL) {

    global $dw_campaign_module_path;


    $results = array();

    $headers = array('Location', 'Participant First Name', 'Participant Last Name', 'Participant Display Name', 'Email', 'Participant_Type', '# Children', '# Adults', 'Currency', 'Total Raised' , '$ Donations pending offline');

    if(empty($arg)) {
        $arg = implode('_', array_keys($list));
    }

    
    $file = $dw_campaign_module_path . "/civi_cache/report-event-registration-$arg-" . time() . ".csv";
    $fp = fopen($file, "w");

    fputcsv($fp, $headers);

    $campaigns_by_contribution_page_id  = dw_campaigns_get_active_campaigns(TRUE);


    foreach($list as $campaign_id => $location) {
        $data = dw_campaign_get_event_participants_by_campaign($campaign_id);

        if(isset($campaigns[$campaign_id])) {
            $campaign = $campaigns[$campaign_id];
        } else {
            $campaign = node_load($campaign_id);
        }

        foreach($data as $row) {

            $total_fee          = 0;
            $total_donation     = 0;

            if($row['row']->pcp_id > 0) {

                $dummy          = new stdClass;
                $dummy->id      = $row['row']->pcp_id;
                $totals         = dw_campaigns_get_contribution_total_for_pcp($dummy);
                $total_count    = $totals['count'];
                $total_donation = $totals['total'];
                $total_offline  = $totals['offline'];
                $total_fee      = 0;
            } else {

                $params = array();
                $params['contact_id']   = $row['row']->contact_id;
                $params['rowCount']     = 999999;
        
                $contributions = _dw_civicrm_contribution_get($params);

                $total_count    = 0;
                $total_offline  = 0;

                foreach($contributions as $donation) {
                    if(isset($campaigns_by_contribution_page_id[$donation->contribution_page_id])) {
                        if($campaigns_by_contribution_page_id[$donation->contribution_page_id]->nid == $campaign->nid) {
                            $total_fee      += $donation->non_deductible_amount;
                            $total_donation += ($donation->total_amount - $donation->non_deductible_amount);
                        }
                    }
                }
            }
          
            $row['row']->children = isset($row['row']->children) ? $row['row']->children : 0;
            $row['row']->adults   = isset($row['row']->adults) ? $row['row']->adults : 0;

            $parts = array(
                'location'              => $location,
                'first_name'            => $row['contact']->first_name,
                'last_name'             => $row['contact']->last_name,
                'display_name'          => $row['contact']->display_name,
                'email'                 => $row['contact']->email,
                'participant_type'      => $row['row']->participant_type,
                'participant_children'  => $row['row']->children,
                'participant_adults'    => $row['row']->adults,
                'currency'              => $campaign->field_dw_currency['und']['0']['value'],
                'total_donations'       => $total_donation,
                'total_offline'         => $total_offline,
            );

            $results[] = $parts; 

            $values = array_values($parts);
            fputcsv($fp, $values);
        }
    }


    if($mode == 'CSV') {

        $date = date("Y-m-d_His");
        $outfile = 'report-event-registration-' . $arg . '-' . $date . '.csv';
        $fsize   = filesize($file);

        header("Pragma: public"); // required
        header("Expires: 0");
        header("Cache-Control: must-revalidate, post-check=0, pre-check=0");
        header("Cache-Control: private",false); // required for certain browsers
        header("Content-Type: application/force-download"); 
        header("Content-Disposition: attachment; filename=\"$outfile\"");
        header("Content-Transfer-Encoding: binary");
        header("Content-Length: $fsize");
        header("Content-type: text/csv");   
        echo file_get_contents($file);
        die;
    } else {
        return theme('dw_campaigns_reports_event_registration', array('headers' => $headers, 'rows' => $results, 'form' => $form));
    }
}

function dw_campaigns_reports_event_registration_form($form, &$form_state, $options, $arg, $mode) {
    $form = array();

    $form['options']     = array(
        '#type'             => 'select',
        '#title'            => t('Event Location'),
        '#default_value'    => $arg,
        '#options'          => array('ALL' => 'All Active Campaigns') + $options
    );

    $form['mode']        = array(
        '#type'             => 'select',
        '#title'            => t('Output Type'),
        '#default_value'    => $mode,
        '#options'          => array('SCREEN' => 'Table', 'CSV' => 'CSV'),
    );

    $form['submit']      = array(
        '#type'             => 'submit',
        '#value'            => t('Submit'),
        '#attributes'       => array('class'=> array('btn'))
    );
 
    return $form;
}

function dw_campaigns_reports_event_registration_form_submit(&$form, &$form_state) {
    $values = $form_state['values'];

    drupal_goto('admin/reports/dw_event_registration/' . $values['options'] . '/' . $values['mode']);
    die;
}





/**
 DW Campaigns Emails
 * admin/reports/dw_event_emails
 * admin/reports/dw_event_emails/%
 * admin/reports/dw_event_emails/%/CSV
 */
function dw_campaigns_reports_event_emails($arg = '', $csvOutput = false) {

    global $dw_campaign_module_path;

// select box
    $allcampaigns           = dw_campaigns_get_all_campaigns();
    $active                 = dw_campaigns_get_active_campaigns('nodeid');

    $emails                 = array();

    $options                = array();
    $campaigns              = array();

    foreach($allcampaigns as $campaign) {
        $campaigns[$campaign->nid]  = $campaign;
        if(!isset($active[$campaign->nid])) {
            $options[$campaign->nid]    = '(Inactive) ' . $campaign->title;
        } else {
            $options[$campaign->nid]    = $campaign->title;
        }
    }

    asort($options);

    $form           = drupal_get_form('dw_campaigns_reports_event_emails_form', $options, $arg);

    //drupal_add_css(' #dw-campaigns-reports-event-registration-form .form-type-select { display:inline-block; } #dw-campaigns-reports-event-registration-form .form-item { border-width:0px; }', $options['type'] = 'inline');

    $list   = array();

    $master_list = $options;

    if(strcasecmp($arg, 'ALL') == 0) {
        $list = $master_list; 
    } else {
        $list[$arg] = $master_list[$arg];
    }

    foreach($list as $campaign_id => $location) {
        $res = _dw_campaigns_get_pcps_for_campaign($campaigns[$campaign_id]);

        foreach($res as $pcp) {

            $params = array(
                'contact_id'    => $pcp->contact_id,
                'returnFirst'   => 1
            );
            $pcp_contact        = _dw_civicrm_contact_get($params);

            $rows[] = array('first_name' => $pcp_contact->first_name, 'last_name' => $pcp_contact->last_name,  'email' => $pcp_contact->email);
        }

    } 

    if( $csvOutput ){
        //prepare date for csv output
        $csvDataArray = array();

        $header = array(
            'first_name',
            'last_name',
            'email'
        );

        $csvDataArray[] = $header;

        foreach($rows as $row){

            $csvData = array(
                $row['first_name'],
                $row['last_name'],
                $row['email']
            );

            $csvDataArray[] = $csvData;

        }

        return output_csv( 'event_emails_'.date('Y_m_d').'.csv', $csvDataArray );
    }

    return theme('dw_campaigns_reports_event_emails', array('headers' => $headers, 'rows' => $rows, 'form' => $form, 'selected' => $arg));

}

function dw_campaigns_reports_event_emails_form($form, &$form_state, $options, $arg) {
    $form = array();

    $t = array();
    if(empty($arg)) {
        $t[''] = '-- Please Select an Event --';
    }

    $t['ALL'] = 'All';

    $form['options']     = array(
        '#type'             => 'select',
        '#title'            => t('Event Location'),
        '#default_value'    => $arg,
        '#options'          => $t + $options
    );

    $form['submit']      = array(
        '#type'             => 'submit',
        '#value'            => t('Submit'),
        '#attributes'       => array('class'=> array('btn'))
    );
 
    return $form;
}

function dw_campaigns_reports_event_emails_form_submit(&$form, &$form_state) {
    $values = $form_state['values'];

    drupal_goto('admin/reports/dw_event_emails/' . $values['options']);
    die;
}

function dw_campaigns_reports_event_emails_csv($arg = ''){
    //this is a slightly round-about way of doing this, but it does save duplicate code, and the interface remains consistent enough to justify it.
    return dw_campaigns_reports_event_emails($arg, true);
}










function dw_campaigns_report_host_panel() {
    drupal_goto('dw/user/host');
}

/**
 DW Active PCPs
 * admin/reports/dw_activepages -> dw_campaigns_report_activepages_csv (no selection, will output a selection form [dw_campaigns_reports_activepages_select_form])
 * admin/reports/dw_activepages/% -> dw_campaigns_report_activepages (with selection)
 * admin/reports/dw_activepages/%/csv -> dw_campaigns_report_activepages_csv
 */
function dw_campaigns_reports_activepages_select_form($form, &$form_state, $options, $selected = NULL) {
    $form = array();

    $t = array();
    $t[''] = '-- Please Select a Location--';

    $t['ALL'] = 'All';

    asort($options);

    $form['options']     = array(
        '#type'             => 'select',
        '#title'            => t('Event Location'),
        '#options'          => $t + $options,
        '#default_value'    => $selected
    );

    $form['submit']      = array(
        '#type'             => 'submit',
        '#value'            => t('Submit'),
        '#attributes'       => array('class'=> array('btn'))
    );

    return $form;
}

function dw_campaigns_reports_activepages_select_form_validate(&$form, &$form_state) {

}

function dw_campaigns_reports_activepages_select_form_submit(&$form, &$form_state) {
    $values = $form_state['values'];

    drupal_goto('admin/reports/dw_activepages/' . $values['options']);
    die;
}

function dw_campaigns_reports_activepages($selected = NULL) {

    $form               = NULL;
    $selected_campaign  = NULL;
    $campaign           = NULL;
    $campaigns          = dw_campaigns_get_active_campaigns(TRUE);
    $donations          = array();
    $found              = FALSE;
    $options            = array();

    $data               = array();

    $headers = array(
        array(
            'data' => t('Page Title'),
        ),
        array(
            'data' => t('Location'),
        ),
    );

    foreach($campaigns as $contribution_page => $campaign) {
        if(is_null($selected)) {
    
        }
    
        if($selected == $campaign->nid) {
            $found  = TRUE;
            $selected_campaign = $campaign;
        }
    
        $options[$campaign->nid] = $campaign->title;
    }

    if($selected == 'ALL') {
        $found = TRUE;
    }

    $donations  = array();
        
    $selform    = drupal_get_form('dw_campaigns_reports_activepages_select_form', $options, $selected);

    if($found) {
        if($selected == ALL) {
            foreach($campaigns as $contribution_page => $campaign) {
                 $res = _dw_campaigns_get_pcps_for_campaign($campaign); 
                 $data += (array)$res;
            }
        } else {
            $data =  (array)_dw_campaigns_get_pcps_for_campaign($selected_campaign);
        }

    } else {
        return drupal_render($selform);
    }


    return theme('dw_campaigns_reports_activepages', array('selform' => $selform, 'headers' => $headers, 'campaigns' => $campaigns, 'data' => $data, 'selected' => $selected ));
}

function dw_campaigns_report_activepages_csv($selected = NULL ){
    //$selected come come as a nodeId, or 'ALL'.
    //ensure that we account for the 'ALL' flag appropriately!

    $campaigns = dw_campaigns_get_active_campaigns(TRUE);


    if( $selected !== 'ALL' ){
        //then we're looking for a single campaign in the list, lets extract it and overwrite the campaigns variable with it! :D
        foreach( $campaigns as $campaign ){
            if( $selected == $campaign->nid ){
                //this is the one and only campaign we need to export as a csv!
                $campaigns = array($campaign);
                continue;
            }
        }
    }

    $csvDataArray = array();

    //we've normalized our $campaigns variable now.. just prepare the csv data now. mmkay?
    $header = array(
        'title',
        'url',
        'contactName',
        'contactEmail',
        'contactAddress'
    );



    $csvDataArray[] = $header;

    foreach( $campaigns as $campaign ){
        $pcps            = _dw_campaigns_get_pcps_for_campaign($campaign);

        foreach( $pcps as $pcp ){
            $title          = $pcp->title;
            $pageId         = $pcp->contribution_page_id;

            $drupal_id      = _dw_campaigns_contact_id_get_user($pcp->contact_id);
            $fake_user      = dw_campaigns_cache_simple_user_data($drupal_id);
            $url            = 'http://'.$_SERVER['SERVER_NAME'].dw_campaigns_user_get_pcp_url($fake_user, $campaign);
            $contact        = _dw_civicrm_contact_get( array('contact_id' => $pcp->contact_id, 'returnFirst' => 1 ) );
            $contactName    = $contact->display_name;
            $contactAddress = $contact->street_address;
            $contactCity    = $contact->city;
            $contactState   = $contact->state_province;
            $contactPostal  = $contact->postal_code;
            $contactCountry = $contact->country;
            $contactEmail   = $contact->email;


            $csvData = array(
                $title,
                $url,
                $contactName,
                $contactEmail,
                $contactAddress.' '.$contactCity.', '.$contactState.' '.$contactPostal.' '.$contactCountry,

            );

            $csvDataArray[] = $csvData;
        }

    }


    return output_csv('active_pcps_'.date('Y_m_d').'.csv', $csvDataArray );
}





function dw_campaigns_reports_hosts() {
    $hosts  = dw_campaigns_get_hosts();


    return theme('dw_campaigns_reports_hosts', array('hosts' => $hosts));
}


/**
 DW Shirt Orders
 * admin/reports/dw_shirtorders/
 * admin/reports/dw_shirtorders/csv
 * admin/reports/dw_shirtorders/%
 * admin/reports/dw_shirtorders/%/%
 */
function dw_campaigns_reports_shirtorders($ref_nid = NULL, $option = NULL, $outputCsv = false) {
    $rows       = array();

    if(!is_null($ref_nid)) {
        db_query("update {dw_campaigns_request_services set shipping_status = '1' where nid=:nid", array(':nid' => $ref_nid));
        drupal_set_message(t("Shirt order marked as shipped!"));
        drupal_goto("admin/reports/dw_shirtorders");
    }

    $results    = db_query("select a.* from {dw_campaigns_request_services} a left join {dw_campaigns_campaigns_to_contribution_pages} b on a.nid = b.nid where shirt_quantity>0 or length(shirt_event_date) > 1 and b.active = 'Y'");

    foreach($results as $row) {
        $node               = node_load($row->nid);
        $row->node_title    = $node->title;
        $rows[]             = $row;
    }

    if( $outputCsv ){
        $csvDataArray = array();

        $header = array(
            'DeliveryBy',
            'Location',
            'EventDate',
            'ShippingAddress',
            'NumberOfShirtsNeeded',
            'SizeRequest',
            'Status'
        );

        $csvDataArray[] = $header;

        foreach( $rows as $row ){
            $row = get_object_vars($row);

            $csvData = array(
                $row['shirt_latest_delivery_date'],
                $row['node_title'],
                $row['shirt_event_date'],
                $row['shirt_shipping_address'],
                $row['shirt_quantity'],
                $row['shirt_special_requests'],
                $row['shipping_status'] ? 'Ordered' : 'Pending'
            );

            $csvDataArray[] = $csvData;
        }

        return output_csv('shirt_orders_'.date('Y_m_d').'.csv', $csvDataArray );
    }

    return theme('dw_campaigns_reports_shirtorders', array('rows' => $rows));
}

function dw_campaigns_reports_shirtorders_csv(){
    dw_campaigns_reports_shirtorders( null, null, true );
}





/**
 DW Event Insurance
 * admin/reports/dw_eventinsurance
 * admin/reports/dw_eventinsurance/csv
 */
function dw_campaigns_reports_eventinsurance($ref_nid = NULL, $option = NULL, $csvOutput = false) {
    $rows       = array();
/*
    if(!is_null($ref_nid)) {
        db_query("update {dw_campaigns_request_services set shipping_status = '1' where nid=:nid", array(':nid' => $ref_nid));
        drupal_set_message(t("Shirt order marked as shipped!"));
        drupal_goto("admin/reports/dw_shirtorders");
    }
*/
    $results    = db_query("select b.*, a.nid as real_nid from {dw_campaigns_campaigns_to_contribution_pages} a left join {dw_campaigns_request_services} b on a.nid = b.nid  where a.active = 'Y'");

    foreach($results as $row) {
        $node               = node_load($row->real_nid);

        if(!$node) {
            continue;
        }

        $has_record         = ($row->nid == $row->real_nid) ? TRUE : FALSE;

        $pcps               = _dw_campaigns_get_pcps_for_campaign($node);
        $row->pcp_count     = count((array)$pcps);

        $host_name          = '';
        $host_id            = isset($node->field_dw_host_primary['und']['0']['value']) ? $node->field_dw_host_primary['und']['0']['value'] : NULL;
        $cohost_id          = isset($node->field_dw_host_secondary['und']['0']['value']) ? $node->field_dw_host_secondary['und']['0']['value'] : NULL;

        if(!is_null($host_id)) {
            $target = node_load($host_id);
            $host_name .= isset($target->field_dw_host_name['und']['0']['value']) ? $target->field_dw_host_name['und']['0']['value'] : NULL;
        }

        if(!is_null($cohost_id)) {
            $target = node_load($host_id);
            $host_name .= ", " . isset($target->field_dw_host_name['und']['0']['value']) ? $target->field_dw_host_name['und']['0']['value'] : NULL;
        }

        $row->has_record    = $has_record;
        $row->host_names    = trim($host_name);
        $row->node_title    = $node->title;
        $rows[]             = $row;
    }

    if( $csvOutput ){
        //prepare data for csv output
        $csvDataArray = array();

        $header = array(
            'hostName',
            'location',
            'eventDate',
            'pcpCount',
            'additionalInsuredName',
            'additionalInsuredAddress',
            'status'
        );

        $csvDataArray[] = $header;

        foreach( $rows as $row ){
            $row = get_object_vars($row);

            $csvData = array(
                $row['host_names'],
                $row['ins_location'],
                $row['ins_event_date'],
                $row['pcp_count'],
                $row['ins_ai_name'],
                $row['ins_ai_address'],
                $row['ins_status']
            );

            $csvDataArray[] = $csvData;
        }

        return output_csv('event_insurance_'.date('Y_m_d').'.csv', $csvDataArray);
    }
    //Host Name, Location, Event Date, # PCPs, Name of Additional Insured, Address of Additional Insured, Status
    return theme('dw_campaigns_reports_eventinsurance', array('rows' => $rows));
}

function dw_campaigns_reports_eventinsurance_csv(){
    //calling the report function, with $outputCsv set to true.
    dw_campaigns_reports_eventinsurance(null, null, true);
}




/**
 DW Personal Page Fundraising Totals
 * admin/reports/dw_pp_fundraising_total
    -> dw_campaigns_pp_fundraising_total()

   @todo normalize!  I want consistency here, make it feel like all the others (focus on the csv output, for now)
 */
function dw_campaigns_pp_fundraising_total() {
    global $dw_campaign_module_path;

    $report_rows    = array();
    $campaigns      = dw_campaigns_get_active_campaigns('nodeid');

//    $campaigns_by_contribution_page_id  = dw_campaigns_get_active_campaigns(TRUE);

    $i = 0;

    $headers = array(
        array(
            'data' => t('Name'),
        ),
        array(
            'data' => t('Location'),
        ),
        array(
            'data' => t('Email'),
        ),
        array(
            'data' => t('Phone'),
        ),
        array(
            'data' => t('Tags'),
        ),
        array(
            'data' => t('Total Raised'),
        ),
        array(
            'data' => t('Confirmed Donations'),
        ),
    );

    $headers_values = array();
    foreach($headers as $row) {
        $headers_values[] = $row['data'];
    }

    $basename = "report-pp-fundraising-total-" . time() . ".csv";
    $file = $dw_campaign_module_path . "/civi_cache/" . $basename;
    $fp = fopen($file, "w");

    fputcsv($fp, array_values($headers_values));

    foreach($campaigns as $campaign_id => $campaign) {
        $res = _dw_campaigns_get_pcps_for_campaign($campaign);

        foreach($res as $pcp) {
            $i++;
            $pcp_count              += 1;
            $params = array(
                'contact_id'    => $pcp->contact_id,
                'returnFirst'   => 1
            );
            $contact        = _dw_civicrm_contact_get($params);

            $dummy                  = new stdClass;
            $dummy->id              = $pcp->id;
            $totals                 = dw_campaigns_get_contribution_total_for_pcp($dummy);

            $donations_count        = $totals['count'];
            //$online_total           = $totals['online'];
            $online_total           = $totals['online'] - $totals['offline_already_imported'];
            $offline_pending_total  = $totals['offline'];
            $offline_total          = $totals['offline'] + $totals['offline_already_imported'];
            $offline_pending_count  = $totals['offline_pending_count'];
            $total_total            = $totals['total'];
            //$confirmed_total        = $totals['online'] +  $totals['offline_already_imported'];
            $confirmed_total        = $totals['online'];

            $res = _dw_civicrm_tag_entity_display(array('entity_id' => $pcp->contact_id));
            $tags = isset($res->Result->result) ? $res->Result->result : '';
            if($tags == '0') {
                $tags = '';
            }

            $row = array(
                'name'                  => $contact->display_name,
                'location'              => $campaign->title,
                'email'                 => $contact->email,
                'phone'                 => $contact->phone,
                'tags'                  => $tags,
                'total_total'           => sprintf("%.02f", $total_total),
                'confirmed_total'       => sprintf("%.02f", $confirmed_total)
            );

            $values = array_values($row);
            fputcsv($fp, $values);

            $report_rows[] = $row;
        }

    }

    if($mode == 'CSV') {

        $date = date("Y-m-d_His");
        $fsize   = filesize($file);

        header("Pragma: public"); // required
        header("Expires: 0");
        header("Cache-Control: must-revalidate, post-check=0, pre-check=0");
        header("Cache-Control: private",false); // required for certain browsers
        header("Content-Type: application/force-download"); 
        header("Content-Disposition: attachment; filename=\"$outfile\"");
        header("Content-Transfer-Encoding: binary");
        header("Content-Length: $fsize");
        header("Content-type: text/csv");

        echo file_get_contents($file);

        die;
    } else {

        return theme('dw_campaigns_reports_pp_fundraising_total', array('headers' => $headers, 'rows' => $report_rows, 'filename' => $file));
    }
}
