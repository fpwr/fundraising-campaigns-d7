<?php

function dw_campaigns_user_register_oss_page1_page($switch = '') {
    if(empty($switch)) {
        $form = drupal_get_form('dw_campaigns_user_register_oss_page1_form');
    } else { 
        $form = drupal_get_form('dw_campaigns_user_register_oss_page1_form_2');
    }
    return theme('dw_campaigns_user_register_oss_page1_page_theme', array('registerForm' => $form));
}


function dw_campaigns_user_register_oss_page1_form() {
    
    global $user;

    $contact    = dw_campaigns_resolve_contact($user);

    $form = array();
    $form['#attributes']    = array(
        'class'             => array('dw_campaigns-profile-form'),
        'enctype'           => "multipart/form-data"
    );

    $campaign           = NULL;

    $locations          = dw_campaigns_get_all_campaign_location();
    $location_string    = '';

    $selected           = dw_campaigns_get_selected_location();
    
    $found_location     = FALSE;

    if(is_null($selected)) {
        $selected = -1;
    } else {
        if(isset($locations[$selected])) {
            $campaign       = node_load($selected);
            dw_campaigns_set_selected_campaign($campaign);
            $found_location = TRUE;
        } else {
            if($selected != 0) {
                //echo "invalid node ($selected)";
            }	
            dw_campaigns_set_selected_campaign(NULL);
        }
    }
    if(!$found_location) {
        $locations = array('0' => 'Select Location') + $locations;
    }

    $form['blurb'] = array(
        '#type'             => 'markup',
        '#markup'           => '<span class="no-location">' . t('Please select an event location') . '</span>'
    );

    $form['location'] = array(
        '#type'             => 'select',
        '#title'            => t('Select An Event Location'),
        '#options'          => $locations,
        '#default_value'    => $selected,
        '#required'         => true
    );

    $participation_type_words = '';
    if(!is_null($campaign)) {
        $participation_type_words = isset($campaign->field_dw_part_type_blurb['und']['0']['value']) ? $campaign->field_dw_part_type_blurb['und']['0']['value'] : '';
        if(empty($participation_type_words)) {
            $participation_type_words = variable_get('dw_campaigns_participation_type_blurb', ''); 
        }
    }

    $form['free_blurb'] = array(
        '#type'              => 'markup',
        '#markup'            => '<h2 class="participation-type">' . t('Participation Type') . '<span class="form-required" title="This field is required.">*</span></h2>'
    );

// work around for drupal not like empty responses to radio buttons     (An illegal choice has been detected)
    $participation_options      = array();
    $participation_options['']  = "";

    $participation_options      += dw_campaigns_get_oss_participation_type(NULL);

    $form['participation_type'] = array(
        '#type'             => 'radios',
        '#title'            => t('Participation Type'),
        '#options'          => $participation_options,
        '#attributes'       => array('class'=> array('participation_type')),
	'#prefix'           => '<div class="participation-type-wrapper hidden">' . '<div class="participation_type_wds">' . $participation_type_words . '</div>',
	'#suffix'           => '</div>',
        '#required'         => true
    );

    $form['participation_blurb'] = array(
        '#type'    => 'markup',
        '#markup'  => '<div id="participation_blurb">Please Select a Location</div>'
    );

    drupal_add_js('
      (function($) {
        $(document).ready(function() {
	  dw_campaigns.initRegistrationLocation("#edit-location", ".participation-type-wrapper", "#participation_blurb");
        });
     })(jQuery);
    ', 'inline');



    $form['button-break'] = array(
        '#type'    => 'markup',
        '#markup'  => '<div class="button-break"></div>'
    );
    $form['cancel'] = array(
        '#type'             => 'markup',
        '#markup'           => '<a onclick="location.replace(&quot;/dw/cancel/register&quot;); return false;" class="form-submit btn" value="Cancel" id="edit-cancel" name="op">' . t('Cancel') . '</a>'
    );
    
    $form['submit']         = array(
        '#type'             => 'submit',
        '#value'            => t('Submit'),
        '#attributes'       => array('class'=> array('btn'))
    );

    $mode_type   = dw_campaigns_get_selected_type();

    if($mode_type == 'walking' && arg(2) == 'register_oss') {
	$form['submit']['#value'] = t('Next');
    }

    return $form;
}

function dw_campaigns_get_oss_participation_type_ajax($location_id) {
    $form = drupal_render(drupal_get_form('dw_campaigns_get_oss_participation_type_ajax_form', $location_id));
    $form = preg_replace('_\<[\/]?form.*?\>_', '', $form);
    $form = preg_replace('_\<input type="hidden".*?\>_', '', $form);

    echo $form;
    die;
}

function dw_campaigns_get_oss_participation_type_ajax_form($form, &$form_state, $location_id) {
    $form = array();
    
    $campaign = node_load($location_id);
    if(!$campaign || is_null($campaign)) {
	//echo t('<span class="no-location">Please select an event location at the top of this form</span>');
	echo t('<span class="awaiting-location">Options will show when a location is selected</span>');
        die;
    }

    $participation_type_words = '';

    $participation_type_words = isset($campaign->field_dw_part_type_blurb['und']['0']['value']) ? $campaign->field_dw_part_type_blurb['und']['0']['value'] : $participation_type_words;

    if(empty($participation_type_words)) {
        $participation_type_words = variable_get('dw_campaigns_participation_type_blurb', '');
    }

    $participation_options = dw_campaigns_get_oss_participation_type($campaign);

    $form['participation_type'] = array(
        '#type'             => 'radios',
        '#title'            => t('Participation Type'),
        '#options'          => $participation_options,
        '#attributes'       => array('class'=> array('participation_type')),
        '#prefix'           => '<div class="participation_type_wds">' . $participation_type_words . '</div>',
        '#required'         => true
    );

    return $form;
}

function dw_campaigns_get_oss_participation_type($campaign) {

    $campaign = (is_object($campaign)) ? $campaign : node_load($campaign);

// since I changed the above to always try to load a node (even NULL) $campaign will be false

    if(is_null($campaign) || !$campaign) {
        $adult_pledge     = '';
        $adult_purchase   = '';
        $family_pledge    = ''; 
        $runner_pledge    = ''; 
        $runner_purchase  = ''; 
    } else {
        $a = dw_campaigns_get_oss_participation_prices($campaign);
        extract($a);
    }

// FIXME - we do this differently in different places, this should be done in one place
$currency = '$';
if(isset($campaign->field_dw_currency['und']['0']['value'])) {
    switch($campaign->field_dw_currency['und']['0']['value']) {
        case 'GBP':
            $currency = '&pound;';
            break;
        case 'EUR':
        case 'EURO':
            $currency = '&euro;';
            break;
    }
}


    $types = array();

	// FIXME - this should not be hardcoded, these need to be dynamic strings without hardcoded html in them
    if($adult_pledge != -1) {    
        $types['adult_pledge']    = t('<span class="participation-label">Adult - Commit to raise ' . $currency . '!amount</span> ......... <span class="option_price">FREE</span><span class="next_line">Commit to raise ' . $currency . '!amount per adult and register for free!</span>', array('!amount' => $adult_pledge));
    }
    if($adult_purchase != -1) {
        $types['adult_purchase']  = t('<span class="participation-label">Adult - ' . $currency . '!amount Registration Fee</span> ......... <span class="option_price">' . $currency . '!amount</span><span class="next_line">The fee is not tax-receiptable and non-refundable</span>', array('!amount' => $adult_purchase));
    }
    if($family_pledge != -1) {
        $types['family_pledge']   = t('<span class="participation-label">Family - Commit to raise ' . $currency . '!amount</span> ......... <span class="option_price">FREE</span><span class="next_line">(2 adults plus children) Commit to raise ' . $currency . '!amount per family and register for free!</span>', array('!amount' => $family_pledge));
    }
    if($runner_pledge != -1) {
        $types['runner_pledge']   = t('<span class="participation-label">Runner - Commit to raise ' . $currency . '!amount</span> ......... <span class="option_price">FREE</span><span class="next_line">Commit to raise ' . $currency . '!amount per adult and register for free!</span>', array('!amount' => $runner_pledge));
    }
    if($runner_purchase != -1) {
        $types['runner_purchase'] = t('<span class="participation-label">Runner - ' . $currency . '!amount Registration Fee</span> ......... <span class="option_price">' . $currency . '!amount</span><span class="next_line">The fee is not tax-receiptable and non-refundable</span>', array('!amount' => $runner_purchase));
    }

    return $types;
}

function _dw_campaigns_validate_oss_participation_type($campaign, $type) {
    $types = dw_campaigns_get_oss_participation_type($campaign);
    if(isset($types[$type])) {
        return true;
    } else {
        return false;
    }
}

function dw_campaigns_get_oss_participation_prices($campaign) {

    $a = array();

    $a['adult_pledge']     = isset($campaign->field_dw_part_adult_pledge['und']['0']['value']) ? $campaign->field_dw_part_adult_pledge['und']['0']['value'] : 40; 
    $a['adult_purchase']   = isset($campaign->field_dw_part_adult_purchase['und']['0']['value']) ? $campaign->field_dw_part_adult_purchase['und']['0']['value'] : -1;
    $a['family_pledge']    = isset($campaign->field_dw_part_family_pledge['und']['0']['value']) ? $campaign->field_dw_part_family_pledge['und']['0']['value'] : -1;
    $a['runner_pledge']    = isset($campaign->field_dw_part_runner_pledge['und']['0']['value']) ? $campaign->field_dw_part_runner_pledge['und']['0']['value'] : -1;
    $a['runner_purchase']  = isset($campaign->field_dw_part_runner_purchase['und']['0']['value']) ? $campaign->field_dw_part_runner_purchase['und']['0']['value'] : -1;

    return $a;

}

function dw_campaigns_user_register_oss_page1_form_validate(&$form, &$form_state) {

    $values = $form_state['values'];
    if(isset($values['location']) && $values['location'] == '0') {
        form_set_error('location',t('You must select a location'));
    }

/*    
    //check email
    if(!dw_campaigns_user_is_email_unique($values['email'])) {
        form_set_error('email',t('The email you entered was already taken.'));
    }

    if(!dw_campaigns_user_is_email_unique_civi($values['email'])) {
        form_set_error('email',t('The email you entered was already taken in civi.'));
    }
*/
    if(!_dw_campaigns_validate_oss_participation_type($values['location'], $values['participation_type'])) {
        form_set_error('participation_type', t('Invalid Participation Type, please start the registration process over'));
    }
	
}

function dw_campaigns_user_register_oss_page1_form_submit(&$form, &$form_state) {
    $values     = $form_state['values'];
    $locationId = $values['location'];

    $_SESSION['last_form'] = $values;

// Determine if this is a _purchase or a _pledge
    if(strstr($values['participation_type'], "_pledge") !== FALSE) {
        drupal_goto('dw/user/register_oss_pcp/' . $locationId . '/' . $values['participation_type'] );
    } else {
        drupal_goto('dw/user/register_oss_payment/' . $locationId . '/' . $values['participation_type']);
    }
}

function dw_campaigns_user_register_oss_create_pcp_page($campaignId, $signup_type = 'adult_pledge', $pcp_id = NULL) {
    global $user;

    $contactId  = _dw_campaigns_user_get_contact_id($user);

    if(is_null($contactId)) {
    }

    if($campaignId == 'create_page') {
        $campaignId = -1;
    }

    $campaign = NULL;
    if($campaignId != -1) {
        $campaign = node_load($campaignId);
    }


    //get the registration form, but in simplified form

    $form = drupal_get_form('dw_campaigns_user_register_oss_create_pcp_page_form', $campaignId, $signup_type, $pcp_id);
    return  theme('dw_campaigns_user_create_pcp_page_theme', array('campaign' => $campaign, 'pcpCreateForm' => $form));

}

// $pcp_id is the "old" pcp that we are importing from
function dw_campaigns_user_register_oss_create_pcp_page_form($form, &$form_state, $campaignId, $signup_type, $pcp_id = NULL) {

    global $user;

    $form   = array();

    /* look up our current pcp if we have one */
    $mode_type = dw_campaigns_get_selected_type();
    $real_pcp = NULL;

    if($mode_type != 'walking') {
        if(!is_null($campaign)) {
            $real_pcp     = dw_campaigns_user_get_pcp_for_campaign($user, $campaign);
        }
    } else {
        $real_pcp         = dw_campaigns_user_get_pcp_for_walking($user);
        if(!is_null($real_pcp)) {
            drupal_set_message(t('You already have a Fundraising page for this year! Edit details of your fundraising page here'));
            drupal_goto('dw/user/edit_page');
        }
    }

    $real_pcp_id    = isset($real_pcp->id) ? $real_pcp->id : NULL;

    /* lets see if our import value is a valid one*/
    $pcps   = dw_campaigns_user_get_historical_pcps($user);

    if(!isset($pcps[$pcp_id])) {
        $pcp_id = NULL; 

        // if we don't know our pcp id this MAY be a reload of the registration page after its been completed!  To prevent dupes lets try to look ourselves up.
    }

    if(is_null($pcp_id)) {
        $pcp                = new stdClass();
        $pcp->id            = NULL;

        $extra              = new stdClass();
        $extra->youtube_url = NULL;

    } else {
        $pcp    = dw_campaigns_user_get_pcp_by_id($pcp_id);
        $extra  = _dw_campaigns_get_pcp_extra($pcp_id);

    }


    $request_uri    = substr(request_uri(), 1);

    if($user->uid > 0) {

        $form['login-info'] = array(
            '#type'             => 'markup',
            '#markup'           => t('You are currently logged in as @username (@email) - this account will be submitted as the owner for your Fundraising Page, if you would like to register this Fundraising Page with a new account !logout and revisit this form.', array('@username' => $user->name, '@email' => $user->mail, '!logout' => '<a style="color:red" href="/user/logout?destination=' . $request_uri . '">Logout</a>'))
        );

    } else {

        $form['login-hint'] = array(
            '#type'             => 'markup',
            '#markup'           => '<h2>' . t('If you have participated in a previous year please !link to continue', array('!link' => '<a style="color:red" href="/user/login?destination=' . $request_uri . '">Login</a>')) . '</h2>'
        );
   
    }

    if(count($pcps) >= 1) {

        $links = '<ul class="historical-links">';
        foreach($pcps as $id => $temp_pcp) {
            $link_title = sprintf("Title: %s  Goal Amount: %s", $temp_pcp->title, $temp_pcp->goal_amount);
            $links      .= sprintf('<li><a href="/dw/user/register_oss_pcp/%s/%s/%s" class="warn">%s</a></li>', $campaignId, $signup_type, $id, $link_title);
        }
        $links .= '</ul>';

        $form['history'] = array(
            '#type'     => 'markup',
            '#markup'   => '<div class="historical"><h2>' . t('Import From Past Fundraising Page') . '</h2><p>' . t('You have one or more past Fundraising Pages, You can import your information into this Fundraising Page by clicking one of the following links - this will refresh the page and all your changes to the form below will be discarded')  . '</p>' . $links .  '</div>'
        );

        drupal_add_js('
          (function($) {
            $(document).ready(function() {
              $("a.warn").click(function() {
                  return confirm("Are you sure you want to import from your past Fundraising Page?");

              });
            });
          })(jQuery);', 'inline');
    }


    $mode_type   = dw_campaigns_get_selected_type();

    if($campaignId != -1) {
        $campaign = node_load($campaignId);
    }

    if($campaignId != -1 && (is_null($campaign->nid) || !dw_campaigns_valid_type($campaign))) {
            drupal_set_message("Unable to create Personal Fundraising Page");
            drupal_goto('dw');
            return;
    }

    $form['participation_type'] = array(
        '#type'             => 'hidden',
        '#value'            => $signup_type
    );

    if($user->uid == 0) {
        $dummy   = array();
        $in_form = dw_campaigns_user_register_form(NULL, $form_state, 'pcp-photo', $pcp->id, NULL, FALSE);
    
        $form = array_merge($form, $in_form);
        
       
        $form['header-description'] = array(
            '#type'             => 'markup',
            '#markup'           => t('<h2>Create your Fundraising Page - Step 2 of 2</h2><p>and support the !title</p>', array('!title' => $campaign->title)),
            '#weight'           => -999
        );
            
        // remove fields we don't need
        unset($form['address_2']);
        unset($form['telephone']);
    
        $populate_fields = array(
            'first_name',
            'last_name',
            'country',
            'address_1',
            'address_2',
            'city',
            'state',
            'postal_code',
            'email'
        );
    
        foreach($populate_fields as $key) {
            // this is left over from a previous way this form worked - left here incase we need to revert
            if(isset($_SESSION['last_form'][$key])) {
                $form[$key]['#default_value']     = $_SESSION['last_form'][$key];
            }
            
            if(isset($form[$key])) {
                $form['billing-fieldset'][$key]   = $form[$key];
                unset($form[$key]);
            }
        } 
    
        drupal_add_js('
          (function($) {
            $(document).ready(function() {
              $("#edit-displayname").assignPersonalURL();
              $("#edit-username").keydown(function() {
                  dw_campaigns.personalTypedIn    =   true;
              });
            });
          })(jQuery);', 'inline');

    } else {

        $form['header-description'] = array(
            '#type'             => 'markup',
            '#markup'           => t('<h2>Create your Fundraising Page - Step 2 of 2</h2><p>and support the !title</p>', array('!title' => $campaign->title)),
            '#weight'           => -999
        );
    }

    // lookup picture
    $temp_filename='';
    $temp_file=_dw_campaigns_user_upload_photo();
    if(isset($temp_file->filepath)) {
	$temp_filename=$temp_file->filepath;
    }

    $image_params = array(
        'w'                 => 100,
        'src'               => $temp_filename
    );

    $matched_image = '';
    //get the user and drop it in for a preview
    $imageSrc = _dw_campaigns_get_photo($user,  $image_params, 'user-photo', NULL, $matched_image);

    $form['picture-heading']['#markup'] = '<h3 class="clear">' . t('Fundraising Page Picture') . '</h3>';

    $form['user-photo-tmp'] = array(
        '#type'             => 'hidden',
        '#value'            => $temp_filename
    );
 
    $form['picture-preview'] = array(
        '#type'             => 'markup',
        '#markup'           => '<div id="profile-form-image-wrapper"><img src="' . $imageSrc . '" /></div>'
    );
	
    $form['user-photo']     = array(
        '#type'             => 'file',
        '#title'            => t('Photo'),
        '#attributes'       => array(
            'size'          => ''
        )
    );
    
    $form['picture-description'] = array(
        '#type'             => 'markup',
        '#markup'           => '<div id="profile-form-image-description" class="form-item">' . t('This image will be displayed on your fundraising page.  You must select a file that ends in .jpg, .jpeg, .gif, or .png. Note: The file is not uploaded until you hit "Submit" below.') . '</div>'
    );


    $form['youtube-description'] = array(
        '#type'             => 'markup',
        '#markup'           => '<div id="youtube-description">' . t('As an alternative to supplying a photo, you may supply a link to a youtube video which will be used in place of your photo on your Fundraising Page') . '</div>',
    );

    $form['youtube_url'] = array(
        '#type'             => 'textfield',
        '#title'            => t('YouTube Video URL'),
        '#default_value'    => $extra->youtube_url,
        '#description'      => t('To add a Youtube video, go to www.YouTube.com, click the "share" button of the video you would like to add and copy the "Link to this video".'),
        '#required'         => false 
    );
    
    //drop in fields so user can fill out details about the event
    $form['contribution-heading']['words'] = array(
        '#type'             => 'markup',
        '#markup'           => '<h3>' . t('Your Fundraising Page Details') . '</h3>',
        '#weight'           => -999
    );
	
    if(!isset($pcp->id) || is_null($pcp->id)) {
        $pcp->goal_amount   = NULL;
        $pcp->intro_text    = NULL;
        $pcp->title         = NULL;
    }

    if(empty($user->displayname)) {
        $title = !empty($pcp->title)? $pcp->title : $campaign->title . ' Page';
    } else {
        $title = !empty($pcp->title)? $pcp->title:$user->displayname . "'s " . $campaign->title . ' Page';
    }

    $form['contribution-heading']['title'] = array(
        '#type'             => 'textfield',
        '#title'            => t('Page Title'),
        '#default_value'    => $title,
        '#required'         => true 
    );    


    $goal_amount = variable_get('dw_campaign_default_pcp_goal', '100.00');

    if(!is_null($pcp->goal_amount)) {
        $goal_amount = $pcp->goal_amount;
    }

    $form['contribution-heading']['goal_amount'] = array(
        '#type'             => 'textfield',
        '#title'            => t('Fundraising Goal'),
        '#default_value'    => $goal_amount,
        '#required'         => true 
    );


    $max_chars = dw_campaign_get_max_intro();
    $max_chars = 99999999;

    $form['contribution-heading']['words_used'] = array(
        '#type'             => 'markup',
        '#markup'           => '<div class="screen-name-description">' . t('Write a brief paragraph describing your motivation for participating.  This text is shown on your custom web page.') . '</div>',
        //'#markup'           => '<div class="screen-name-description">' . t('Write a brief paragraph describing your motivation for participating.  This text is shown on your custom web page.') . '</div><div class="character_counter">' . t('!chars characters used', array('!chars' => '0 / ' . $max_chars)) . '</div>',
    );

    $default_intro_text = isset($campaign->field_dw_pcp_intro_text['und']['0']['value']) ? $campaign->field_dw_pcp_intro_text['und']['0']['value'] : '';
     
    
    if(!empty($default_intro_text)) {
        $contactId      = _dw_campaigns_user_get_contact_id($user);
        $mergeObjects   = dw_campaigns_get_merge_object($campaign, NULL, $contactId);
        dw_campaigns_do_merge($default_intro_text, $mergeObjects);
    }
    
    $intro_text = !empty($pcp->intro_text) ? $pcp->intro_text : $default_intro_text;
    
    $intro_text = substr($intro_text, 0, $max_chars); // truncate
    
    $form['contribution-heading']['intro_text'] = array(
        //'#type'             => 'textarea',
        '#type'             => 'text_format',
        '#format'           => '1',
        '#title'            => 'Description',
        '#required'         => true,
        '#default_value'    => $intro_text,
        '#attributes'       => array('class'=> array('intro_text')),
    );

/*
    drupal_add_js('
      (function($) {
        $(document).ready(function() {
          dw_campaigns.initIntroText(".character_counter", "#edit-intro-text");
        });
      })(jQuery);
    ','inline');
*/
 
    $event_max = variable_get('dw_campaigns_walking_event_max', '6');

    $attending = array('-1' => t('-- Select Number --'));

    for($i=0; $i<=$event_max; $i++) {
        $attending[$i] = $i;
    }


    list($children, $adults) = dw_campaigns_get_event_participants_count($campaignId, $pcp->id);



// LOOK I'm sure the logic table for how we are handling runner/adult/family pledge/purchase will change - for now adult and family pledge can set # of attendies
    if($signup_type == 'runner_pledge') { 
        $form['contribution-heading']['children'] = array(
            '#type'             => 'hidden',
            '#value'            => 0
        );
    
        $form['contribution-heading']['adults'] = array(
            '#type'             => 'hidden',
            '#value'            => 1
        );

        $ages   = array();
        for($i = 12; $i < 110; $i++) {
            $ages[$i] = $i;
        }

        $form['amount-fieldset']['age'] = array(
            '#type'             => 'select',
            '#title'            => t('How old is the runner?'),
            '#options'          => $ages,
            '#default_value'    => 30
        );
    } else {

        $form['contribution-heading']['children'] = array(
            '#type'             => 'select',
            '#title'            => t(variable_get('dw_campaigns_walking_children_attending_prompt', 'How many children will be attending the event')),
            '#options'          => $attending,
            '#default_value'    => $children
        );
    
        $form['contribution-heading']['adults'] = array(
            '#type'             => 'select',
            '#title'            => t(variable_get('dw_campaigns_walking_adults_attending_prompt', 'How many adults will be attending the event')),
            '#options'          => $attending,
            '#default_value'    => $adults

        );
        $form['amount-fieldset']['age'] = array(
            '#type'             => 'hidden',
            '#value'            => 0
        );
    }

    if($mode_type == 'walking') {
   
        $event_help_text = variable_get('dw_campaigns_walking_event_help_text', '');

        if(is_null($pcp->id) || 1==1) {
            $tos_words = isset($campaign->field_dw_event_tos['und']['0']['value']) ? $campaign->field_dw_event_tos['und']['0']['value'] : NULL;

global $language;
            if(isset($language->language) && $language->language == 'fr') {
                $tos_words = file_get_contents(DW_CAMPAIGN_MODULE_PATH . "tos_text_fr.txt");
            }

            $form['contribution-heading']['tos_words'] = array(
                '#type'             => 'markup',
                '#markup'           => '<div class="tos_words">' . $tos_words . '</div>'
            );
            
            $form['contribution-heading']['tos_checkbox'] = array(
                '#type'             => 'checkbox',
                '#description'      => t('I have read and accept these terms and conditions.')
            );
        }

    }

    $form['pws_relationship'] = array(
            '#type'             => 'select',
            '#title'            => t('What is your relationship to PWS?'),
            '#options'          => dw_campaigns_relationship_tag_list(),
            '#required'         => true
    );
 
    $form['campaign-id'] = array(
            '#type'             => 'hidden',
            '#value'            => $campaignId
    );

    $form['pcp-id']      = array(
            '#type'             => 'hidden',
            '#default_value'    => $real_pcp_id
    );

    $warn_message   = t('Are you sure you want to go back?  Your changes to this form will be lost');

    $form['cancel'] = array(
        '#type'             => 'markup',
        '#markup'           => '<a href="/dw/user/register_oss" class="form-submit btn" value="Cancel" id="edit-cancel" name="op" onclick="return confirm(\'' . $warn_message . '\');">' . t('Back') . '</a>',
        '#weight'           => 998
    );

    $form['submit']         = array(
        '#type'             => 'submit',
        '#value'            => t('Create My Page'),
        '#weight'           => 999,
        '#attributes'       => array('class'=> array('btn', 'alert-submit'))
    );
    

    drupal_add_js('
      (function($) {
        $(document).ready(function() {
          $(".alert-submit").click(function() {
              if($("#edit-submit").css("opacity") != "0.5") {
                  $("#show-words").fancybox().trigger("click"); 
                  setTimeout("jQuery(\"#edit-submit\").attr(\"disabled\", true)", 500);
              } else {

              }
          });
        });
      })(jQuery);',
    'inline');

    //change label on submit button so people know they are creating a contribution page

    return $form;
}




/**
 * Validates the form in the create pcp page process
 * 
 * @param array $form
 * @param array $form_state
 */
function dw_campaigns_user_register_oss_create_pcp_page_form_validate(&$form, &$form_state) {

    global $user;
 
    // we called this different things on different forms, lets normalize it (and make sure both are set)
    // step 1, make sure location is set
    $form_state['values']['location'] = isset($form_state['values']['location']) ? $form_state['values']['location'] : $form_state['values']['campaign-id'];
    $form_state['values']['campaign-id'] = $form_state['values']['location'];

    // step 2
    if($form_state['values']['location'] == 0) {
        form_set_error('location', t('You must select a location'));
    } else {
        // do nothing special
    }


    $values = $form_state['values'];
    $mode_type   = dw_campaigns_get_selected_type();


    if($values['children'] == -1 || $values['adults'] == -1) {
        form_set_error('children', 'Please select the number of children and adults');
        form_set_error('adults', ' ');
        return;
    }

    if(isset($values['tos_checkbox']) && $values['tos_checkbox'] != 1) {
        form_set_error(']event-fields][tos_checkbox', 'You must agree to the waiver');
        return;
    }

/// START

    $values = $form_state['values'];
    if(isset($values['location']) && $values['location'] == '0') {
        form_set_error('location',t('You must select a location'));
    }
    
    //double check username
    if(isset($values['username']) && dw_campaigns_username_search_ajax($values['username'], FALSE) == 'found') {
        form_set_error('username',t('The username you have selected is already taken'));
        return;
    }
    
    //verify passwords
    if(isset($values['password']) && isset($values['password2']) && ($values['password'] != $values['password2'])) {
        form_set_error('password',t('Your passwords do not match, please try again'));
    }
    
    //check email
    if(isset($values['email']) && !dw_campaigns_user_is_email_unique($values['email'])) {
        form_set_error('email',t('The email you entered was already taken.'));
    }

    if(isset($values['email']) && !dw_campaigns_user_is_email_unique_civi($values['email'])) {
        form_set_error('email',t('The email you entered was already taken in civi.'));
    }
/// END

    $event_max = variable_get('dw_campaigns_walking_event_max', '6');
    if(isset($values['num_attending']) && ($values['num_attending'] > $event_max)) {
        form_set_error(']event-fields][num_attending', 'Number of participants exceeds max');
    }

    // don't let them lie about their pcp id - this could let someone hijack someone elses pcp
    if(!empty($values['pcp-id']) && $values['pcp-id'] > 0) {   
        $pcp = dw_campaigns_user_get_pcp_by_id($values['pcp-id']);
        $tmp = _dw_campaigns_contact_id_get_user($pcp->contact_id); 
    
        if($user->uid < 1 || $user->uid != $tmp) {
            form_set_error('', 'Mismatch of user and PCP');
        }
    }

    if($mode_type == 'walking') { 
        //see if they changed their email, if they did, we have to make sure it's unique
        global $user;

        $existing_email = isset($user->mail) ? $user->mail : '';

        if(isset($values['email']) && strcasecmp($existing_email, $values['email']) != 0) {
            if(!dw_campaigns_user_is_email_unique($values['email'])) {
                form_set_error('email',t('The email you entered was already taken.'));
            }
        }
    }
 
    $goal_amount = preg_replace('/[^0-9\.]/', '', $values['goal_amount']);
    $form_state['values']['goal_amount'] = $goal_amount;
    if($goal_amount <= 0) {
        form_set_error('goal_amount', t('Goal amount must be > 0.00'));
    }

    if(!_dw_campaigns_validate_oss_participation_type($values['location'], $values['participation_type'])) {
        form_set_error('participation_type', t('Invalid Participation Type, please start the registration process over'));
    }

    if(isset($_SESSION['messages']['error']) && count($_SESSION['messages']['error']) > 0) {
        form_set_error('', 'Please fix errors and try again');
        return;
    }
}

/**
 * create / update their user and civicrm contact, then create the personal contribution page with their
 * donation amount and welcome text
 * 
 * @param array $form
 * @param array $form_state
 */

function dw_campaigns_create_civi_contact_from_form_helper($values) {
    global $user;
    $disable_create_drupal = false; // used for debugging

if(!$disable_create_drupal) {

    if(! ($user->uid > 0)) {
        $params                     = array();
        $params['mail']             = $values['email'];
        $params['roles']            = array();
        $params['status']           = 1;

        if(isset($values['username'])) {

            if(!isset($values['displayname'])) {
                $values['displayname'] = $values['first_name'] . " " . $values['last_name'];
            }

            $params['name']             = $values['username'];
            $params['pass']             = $values['password'];
            $params['data']             = array('displayname' => $values['displayname']);

            $authenticate               = TRUE;
        }  else {
            $placeholder                = 'PH' . microtime(TRUE);

            $params['name']             = $placeholder;
            $params['pass']             = md5($values['email'] . rand(1111111111, 9999999999) . microtime());
            $params['data']             = array('displayname' => $placeholder);

            //$authenticate               = FALSE;
            $authenticate               = TRUE;
        }
 
        // save the user so drupals happy
        $user_       = user_save('', $params);
   
        if($authenticate) { 
            $uid    = user_authenticate($params['name'], $params['pass']);
            if($uid) {
                $user   = user_load($uid);
            }
        }

        $_SESSION['created_user']       = 1;
    }
}

    //upload photo if they gave one
    if($values['user-photo-tmp']) {
       _dw_campaigns_save_user_photo($user->uid, basename($values['user-photo-tmp']));
    }
    
    $contact = _dw_campaigns_update_civicrm_contact($user, $values);

    $params = array(
        'contact_id'    => $contact->contact_id,
        'returnFirst'   => 1
    );

    $contact        = _dw_civicrm_contact_get($params);
 
    _dw_save_user_to_contact_glue($user, $contact);

    return array('contact' => $contact, 'user' => $user);
}

function dw_campaigns_user_register_oss_create_pcp_page_form_submit(&$form, &$form_state) {

    global $user;
    //register the user in drupal
    $values                     = $form_state['values'];

    if(isset($values['location'])) {
        $params['location']     = $values['location'];
    } else {
        drupal_set_message('No Location', 'error');
	return;
    }

    $res        = dw_campaigns_create_civi_contact_from_form_helper($values);
    $user       = $res['user'];   

    $contact    = $res['contact'];   

    $campaign = NULL;
    
    $campaignId =  $values['location']; 

    if(!is_null($campaignId) && $campaignId != '') {
        $campaign = node_load($campaignId);
    }

// SIGNUPEMAIL   
    $mail_params = dw_campaigns_get_merge_object($campaign);

    $mail_params['contact']                     = $contact;

    if(isset($_SESSION['created_user'])) {

        unset($_SESSION['created_user']); 

        if(!empty($from)) {
            drupal_mail('dw_campaigns_user_register_email', 'signup', $user->mail, user_preferred_language($user), $mail_params, $from);
        } else {
            drupal_mail('dw_campaigns_user_register_email', 'signup', $user->mail, user_preferred_language($user), $mail_params);    
        }
    }
    // TODO
       
    dw_campaigns_set_selected_location($values['location']);

// FINISH USER CREATE - making PCP NOW
    //drupal_goto('dw/walking/start/' . $values['location']);


    $action     = 'creating';
    $mode_type  = dw_campaigns_get_selected_type();

    // we might not need this (we have contact from above)
    $contactId  = _dw_campaigns_user_get_contact_id($user, TRUE);

    // if we know a PCP, then this is meant to overwrite an existing record, so lets backtrack from that to find the campaign   
// THIS FIRST CASE SHOULD NEVER HAPPEN WITH THIS FLOW
    if($values['pcp-id'] > 0) { 
        //create pcp page, update it if we already have one
        $pcp                    = dw_campaigns_user_get_pcp_by_id($values['pcp-id']);
        $contribution_page_id   = $pcp->contribution_page_id;
    
        $campaigns  = dw_campaigns_get_active_campaigns(TRUE);
        $campaign   = $campaigns[$contribution_page_id];

    } else {
        // we're creating a new entry, so we can (mostly) trust the campaign they want to create a campaign for
    	$campaign   = node_load($values['campaign-id']);
        $cpage      = dw_campaigns_get_contribution_page_for_campaign($campaign);

        $contribution_page_id = $cpage->id;
        $pcp        = NULL;

        $suffix = variable_get('dw_campaigns_cfg_tag_suffix', date('Y'));
        $name   = variable_get('dw_campaigns_cfg_tag_prefix', 'OSS') . ' fundraiser ' . $suffix;
        $tag_id = dw_campaigns_get_tag_id_by_name($name);

        dw_campaigns_tag_contact($tag_id, array($contactId));

    }


    // $contactId = $contact->contact_id;

    $params                         = array();
    if(isset($pcp->id) && !is_null($pcp->id)) {
        $params['id']                   = $pcp->id;
        $action = 'updating';
    }
    
    if(is_null($contactId)) {
        drupal_set_message('Unable to look up this user', 'error');
    }

    $currency                       = $campaign->field_dw_currency['und']['0']['value'];
    if($currency == 'MULTI') {
        $currency = NULL;
    }

    $intro_text = is_array($values['intro_text']) ? ( isset($values['intro_text']['value']) ? $values['intro_text']['value'] : '' ) : $values['intro_text'];

    $params['contact_id']           = $contactId;
    $params['status_id']            = 2;
    $params['title']                = $values['title'];
    $params['intro_text']           = $intro_text;
    $params['goal_amount']          = $values['goal_amount'];
    $params['is_active']            = 1;
    $params['is_thermometer']       = 1;
    $params['is_honor_roll']        = 1;
// FIXME
// v3 no longer has this field - rather we now need a pcp_block_id - for now we make up for this in the create call
    $params['contribution_page_id'] = $contribution_page_id;
    $params['pcpBlock']             = 0; // we do NOT want a pcpBlock
    $params['currency']             = $currency;

    $pcp = _dw_civicrm_pcp_create($params);

// TODO images - this uses their user image

    //save the user photo upload
    if($form_state['values']['user-photo-tmp']) {
        _dw_campaigns_save_pcp_photo($user->uid, basename($form_state['values']['user-photo-tmp']), $pcp->id);
        if($mode_type == 'walking') {
            _dw_campaigns_save_user_photo($user->uid, basename($form_state['values']['user-photo-tmp']));
        }
    }
    _dw_campaigns_save_pcp_extra($pcp->id, $form_state['values']);
    
    $mergeObjects   = dw_campaigns_get_merge_object($campaign, $pcp->id, $contactId);

    //$participants_old = dw_campaigns_get_event_participants_count($campaign->nid, $pcp->id) ;
    list($old_children, $old_adults) = dw_campaigns_get_event_participants_count($campaign->nid, $pcp->id) ;

    $participants_old = $old_children + $old_adults;

    if(is_null($participants_old)) {
        $participants_old = 0;
    } 

    $values['num_attending'] = $values['children'] + $values['adults'];
    dw_campaigns_set_event_participants_count(array('campaignId' => $campaign->nid, 'pcpId' => $pcp->id, 'children' => $values['children'], 'adults' => $values['adults'], 'age' => $values['age'], 'participation_type' => $values['participation_type']));

    $event_data = new stdClass();
    $event_data->participants_old = $participants_old;
    $event_data->participants     = $values['num_attending'];
    $mergeObjects['event']        = $event_data;

 
    // hint to go to share page
    if($action == 'creating') {

        drupal_set_message('Thank you for creating a fundraising page.  The key to reaching your fundraising goal is sharing your personal page with as many people as possible.  <a href="/dw/user/share">Email/Share your page</a> now.', 'status');

        // add the campaigns contact to the list of people to notify
        $contact_email  = isset($campaign->field_dw_contact_email['und']['0']['value']) ? $campaign->field_dw_contact_email['und']['0']['value'] : '';
        $targets = preg_split("/[\n,;]/", $contact_email, -1, PREG_SPLIT_NO_EMPTY);

        // add the event hosts to the list of people to notify
        $host_id    = isset($campaign->field_dw_host_primary['und']['0']['value']) ? $campaign->field_dw_host_primary['und']['0']['value'] : NULL;
        $cohost_id  = isset($campaign->field_dw_host_secondary['und']['0']['value']) ? $campaign->field_dw_host_secondary['und']['0']['value'] : NULL;
    
        if(!is_null($host_id)) {
            $target = node_load($host_id);
            $d_uid  = isset($target->field_dw_host_user['und']['0']['value']) ? $target->field_dw_host_user['und']['0']['value'] : NULL;
            if(!is_null($d_uid)) {
                $t_user   = user_load($d_uid);
    
                $email    = $t_user->mail;
                $language = user_preferred_language($t_user);

                $targets[]  = $email;
            }
             
        }
    
        if(!is_null($cohost_id)) {
            $target = node_load($cohost_id);
            $d_uid  = isset($target->field_dw_host_user['und']['0']['value']) ? $target->field_dw_host_user['und']['0']['value'] : NULL;
            if(!is_null($d_uid)) {
                $t_user   = user_load($d_uid);
    
                $email    = $t_user->mail;
                $language = user_preferred_language($t_user);

                $targets[]  = $email;
            }
        }



        foreach($targets as $email) {
            $email  = trim($email);

            if(strlen($email) > 5) {
                drupal_mail('dw_campaigns_pcp_page', 'pcp-create', $email, user_preferred_language($user), $mergeObjects);
            }
        }


    } else {

        $targets = preg_split("/[\n,;]/", $campaign->field_dw_contact_email['und']['0']['value'], -1, PREG_SPLIT_NO_EMPTY);
        drupal_set_message('Fundraising page updated.  Are you ready to share your campaign now? <a href="/dw/user/share">Share my Fundraising Page</a>', 'status');    


        foreach($targets as $email) {
            $email  = trim($email);

            if(strlen($email) > 5) {
                // drupal_mail('dw_campaigns_pcp_page', 'pcp-update', $email, user_preferred_language($user), $mergeObjects);
            }
        }

    }

    $redir  = dw_campaigns_get_campaign_path($campaign->nid, 'dw/users/' . $user->name);

    drupal_goto($redir);
    return;
}


function dw_campaigns_user_register_oss_registration_payment_page($campaignId, $signup_type = 'adult_purchase') {
    $form = drupal_get_form('dw_campaigns_user_register_oss_registration_payment_form', $campaignId, $signup_type);
    return theme('dw_campaigns_user_register_oss_registration_payment_page_theme', array('form' => $form, 'campaignId' => $campaignId));
}



function dw_campaigns_user_register_oss_registration_payment_form($form, &$form_state, $campaignId, $signup_type) {

    $oss_donation_options = array(
        '0'    => '$0',
        '15'   => '$15',
        '25'   => '$25',
        '50'   => '$50',
        '100'  => '$100',
        '-1'   => 'Other Amount'
    );

    $campaign = node_load($campaignId);
    $contributionPage   = dw_campaigns_get_contribution_page_for_campaign($campaign);

    $fees = dw_campaigns_get_oss_participation_prices($campaign);

    $participation_type = $signup_type;
    $participation_fee = isset($fees[$participation_type]) ? $fees[$participation_type] : 25;

    $form = array();


    $form['header-description'] = array(
        '#type'             => 'markup',
        '#markup'           => t('<h2>Step 2 of 2</h2>'),
        '#weight'           => -999
    );
    $form['amount-fieldset'] = array(
        '#type'             => 'fieldset',
        '#title'            => t('Payment')
    );

    $form['amount-fieldset']['current_currency'] = array(
        '#type'             => 'hidden',
        '#value'            => $contributionPage->currency
    );

    $form['amount-fieldset']['donation-currency'] = array(
        '#type'             => 'hidden',
        '#value'            => $contributionPage->currency
    );

    $form['amount-fieldset']['fee'] = array(
        '#type'             => 'hidden',
        '#value'            => $participation_fee
    );



    if($signup_type == 'adult_purchase') {

        $form['amount-fieldset']['initial_payment'] = array(
            '#type'             => 'checkbox',
            '#title'            => t('$@fee Participation Fee Per Adult', array('@fee' => $participation_fee)),
            '#attributes'       => array('checked' => 'checked'),
            '#disabled'         => true
        );

        $attending = array();
        
	$event_max = variable_get('dw_campaigns_walking_event_max', '6');

        for($i=1; $i<=$event_max; $i++) {
            $attending[$i] = $i;
        }

        $form['amount-fieldset']['children'] = array(
            '#type'             => 'hidden',
            '#attributes'       => array('id' => 'edit-children'),
            '#value'            => 0
        );

        $form['amount-fieldset']['adults'] = array(
            '#type'             => 'select',
            '#title'            => t('How many adults will be attending the event'),
            '#options'          => $attending,
            '#default_value'    => '1'
        );
    } else {
// RUNNER PURCHASE
        $form['amount-fieldset']['initial_payment'] = array(
            '#type'             => 'checkbox',
            '#title'            => t('$@fee Participation Fee', array('@fee' => $participation_fee)),
            '#attributes'       => array('checked' => 'checked'),
            '#disabled'         => true
        );

        $form['amount-fieldset']['children'] = array(
            '#type'             => 'hidden',
            '#attributes'       => array('id' => 'edit-children'),
            '#value'            => 0
        );

        $form['amount-fieldset']['adults'] = array(
            '#type'             => 'hidden',
            '#attributes'       => array('id' => 'edit-adults'),
            '#value'            => 1
        );

        $ages   = array();
        for($i = 12; $i < 110; $i++) {
            $ages[$i] = $i;
        }

        $form['amount-fieldset']['age'] = array(
            '#type'             => 'select',
            '#title'            => t('How old is the runner?'),
            '#options'          => $ages,
            '#default_value'    => 30
        );


    }


    $form['amount-fieldset']['additional_donation'] = array(
        '#type'             => 'radios',
        '#title'            => t('Would you like to make an additional donation to FPWR?'),
        '#options'          => $oss_donation_options,
        '#attributes'       => array('class'=> array('additional_donation')),
	'#prefix'           => '<div class="additional-donation-wrapper">',
	'#suffix'           => '</div>',
        '#default_value'    => 0,
        '#required'         => true
    );

    $form['amount-fieldset']['other_amount'] = array(
        '#type'             => 'textfield',
        '#default_value'    => '0.00',
        '#title'            => 'Other Amount',
    ); 

    $form['amount-fieldset']['total_blurb'] = array(
        '#type'    => 'markup',
        '#markup'  => '<div id="total_blurb"><span class="oss_total_words">Total Amount: </span><span class="oss_total">' . dw_campaigns_force_decimal($participation_fee, $contributionPage->currency) . '</span><div class="total_blurb-description">This amount will be charged to your card</div></div>'
    );

    $dummy  = array();

    $pcp = dw_campaigns_oss_campaign_find_generaldonation($campaignId);
    $more_form = dw_campaigns_users_donate_page_form(NULL, $dummy, $campaignId, $pcp, FALSE);

    drupal_add_js('
        (function($) {
          $(document).ready(function() {
            $(".confirm-amount").click(function() {
                var amount = $(".oss_total").html();

                res = confirm("Please confirm your payment of " + amount);

                if(res) {
                    $("#show-words").fancybox().trigger("click");
                    setTimeout("jQuery(\"#edit-submit\").attr(\"disabled\", true)", 500);
                }

                return res;
            });
          });
        })(jQuery);',
    'inline');

    drupal_add_js('
      (function($) {
        $(document).ready(function() {
          function update_total() {
             var extra    = 0;
             var fee      = $("input[name=\"fee\"]").val();
             var qty      = $("#edit-adults").val();
             var current  = $("input.additional_donation:checked").val();
             var currency = $("input[name=\"current-currency\"]").val();

             if(current == -1) {
                 extra   = $("#edit-other-amount").val();
             } else {
                 extra   = current;
             }
              
             var amount = (Number(fee) * Number(qty)) + Number(extra);  

             request = $.get("/dw/ajax/currencyformat/" + amount + "/" + currency, function(data) {
                 $(".oss_total").html(data);
             });
          }

          $("input.additional_donation").click(function(){
              update_total();
          });

          $("#edit-adults").change(function(){
              update_total();
          });
          
          $("input.additional_donation:checked").click(); 

          $("#edit-other-amount").keyup(function() {
            $("input.additional_donation:checked").click();
          });
        });
      })(jQuery);', 
    'inline');
 
    $mode_type   = dw_campaigns_get_selected_type();

    if($mode_type == 'walking') {
   
        //drop in fields so user can fill out details about the event
        $form['contribution-heading'] = array(
            '#type'             => 'fieldset',
            '#title'            => t('Waiver'),
            //'#weight'         => -999
        );

        $event_help_text = variable_get('dw_campaigns_walking_event_help_text', '');

        $tos_words = $campaign->field_dw_event_tos['und']['0']['value'];
/*
        if(isset($language->language) && $language->language == 'fr') {
     $tos_words = file_get_contents(DW_CAMPAIGN_MODULE_PATH . "tos_text_fr.txt");
        }
*/
        $form['contribution-heading']['tos_words'] = array(
            '#type'             => 'markup',
            '#markup'           => '<div class="tos_words">' . $tos_words . '</div>'
        );
        
        $form['contribution-heading']['tos_checkbox'] = array(
            '#type'             => 'checkbox',
            '#description'      => t('I have read and accept these terms and conditions.')
        );

    }

    $form['pws_relationship'] = array(
            '#type'             => 'select',
            '#title'            => t('What is your relationship to PWS?'),
            '#options'          => dw_campaigns_relationship_tag_list(),
            '#required'         => true
    );

    unset($more_form['donation-comments']);
    unset($more_form['privacy-fieldset']);
    unset($more_form['donor-information']['donation-amount']);

    $more_form['donor-information']['#title']   = t('Billing Information');

    $form = array_merge($form, $more_form);

    $form['pcpId']['#value'] = $pcp->id;
    $form['campaign']['#value'] = $campaignId;

    $form['billing-fieldset']['address_1']['#title']   = t('Billing Address');
    unset($form['billing-fieldset']['address_2']);

    // remove the billing information differs from the contact fields (we dont honor them here anyways)
    unset($form['payment-fieldset']['billing-info']);

    $form['participation_type'] = array(
        '#type'             => 'hidden',
        '#default_value'    => $_SESSION['last_form']['participation_type']
    );

    return $form;    
}

function dw_campaigns_user_register_oss_registration_payment_form_validate(&$form, &$form_state) {
    global $user;
    global $dw_campaign_module_path;
 
    $oss_donation_options = array(
        '0'    => '$0',
        '15'   => '$15',
        '25'   => '$25',
        '50'   => '$50',
        '100'  => '$100',
        '-1'   => 'Other Amount'
    );
    

    $multi = FALSE;

    $receipt_amount = 0;


    // lets try to clean up the users input, so lets reduce it to the valid characters
    $other_amount   = preg_replace('/[^0-9\.\-]/', '', $form_state['values']['other_amount']);
    $form_state['values']['other_amount'] = $other_amount;



    $values         = $form_state['values'];

    // calculate the total price based upon the fee X the number of adults attending (1 for runners, selectable for adults)
    $signup_amount  = $values['fee'] * $values['adults'];

    if(isset($values['tos_checkbox']) && $values['tos_checkbox'] != 1) {
        form_set_error(']contribution-heading][tos_checkbox', 'You must agree to the waiver');
        return;
    }

    if($values['additional_donation'] == -1) {
        if($other_amount <= 0) {
            form_set_error('other_amount', t('Donation amount must be > 0.00'));
            return;
        } else {
            $receipt_amount = $values['other_amount'];
        }
    }
   
    if(!isset($oss_donation_options[$values['additional_donation']])) {
        form_set_error('additional_donation', t('Please select one of these options'));
        return;
    } else {

        if($values['additional_donation'] != -1) {
            $values['additional_donation'] = preg_replace("/[^0-9\.]/", '',  $values['additional_donation']);
            $receipt_amount = $values['additional_donation'];
        }

        $total_billed = $receipt_amount + $signup_amount;
        $form_state['values']['total_billed']   = $total_billed;
        $form_state['values']['receipt_amount'] = $receipt_amount;
        $form_state['values']['signup_amount']  = $signup_amount;
    }

    if(isset($_SESSION['messages']['error']) && count($_SESSION['messages']['error']) > 0) {
        form_set_error('', 'Please fix errors and try again');
        return;
    }

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
    
    $states             = _dw_civicrm_pseudoconstant_stateprovince();
    $state              = isset($states[$values['state']])?$states[$values['state']]:'';

    if(empty($state))
        $state='CA';
    

    $remote_addr                                = $_SERVER['REMOTE_ADDR'];
    if($remote_addr == '::1') {
        $remote_addr                            = '127.0.0.1';
    }

    if($campaign->field_dw_currency['und']['0']['value']  == 'MULTI') {
        $multi = TRUE;        
    }

    $params['email-5']                          = $values['email'];
    $params['amount']                           = $total_billed;
    $params['amount_other']                     = $total_billed;
    $params['custom_1']                         = '';
// -5 means 'billing'
    $params['billing_first_name']               = $values['first_name'];
    $params['billing_middle_name']              = '';
    $params['billing_last_name']                = $values['last_name'];
    $params['billing_street_address-5']         = $values['address_1'];
    $params['billing_city-5']                   = $values['city'];
    $params['billing_state_province_id-5']      = $values['state'];
    $params['billing_postal_code-5']            = $values['postal_code'];
    $params['billing_country_id-5']             = $country_iso_code; // $countries[$values['country']];
    $params['credit_card_number']               = $values['card_number'];
    $params['cvv2']                             = $values['cvv'];
    $params['credit_card_exp_date']['M']        = $values['exp-month'];
    $params['credit_card_exp_date']['Y']        = $values['exp-year']+date('Y');
    $params['credit_card_type']                 = $values['card_type'];
    $params['billing_state_province-5']         = $state;
    $params['billing_country-5']                = $country_iso_code;
    $params['month']                            = $values['exp-month'];
    $params['year']                             = $values['exp-year']+date('Y');
    $params['ip']                               = $remote_addr;
    $params['ip_address']                       = $remote_addr;
    $params['amount_level']                     = '';
    $params['currencyID']                       = $contributionPage->currency;
    $params['currency']                         = $contributionPage->currency;
    $params['payment_action']                   = 'Sale';
    $params['is_pay_later']                     = '';
    $params['is_recur']                         = '';
    $params['description']                      = 'Online Contribution: ' . $contributionPage->title;
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
    $params['contributionType_name']            = $contributionType->name;
    $params['contributionType_accounting_code'] = $contributionType->accounting_code;
    $params['contributionPageID']               = $contributionPage->id;
    $params['subject']                          = '';

// save home address
    $params['email-1']                          = $values['email'];
    $params['street_address-1']                 = $values['address_1'];
    $params['city-1']                           = $values['city'];
    $params['state_province-1']                 = $state;
    $params['postal_code-1']                    = $values['postal_code'];
    $params['country-1']                        = $country_iso_code;

 
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

    /* since we are using the REST API we need to pass it the fields we want to dedupe on */
    foreach($params as $key => $value) {
        $dedupe['fields_' . $key] = $value;
    }

    $dedupe = array_merge($dedupe, $params);
    $contactID = _dw_campaigns_dwutils_get_deduped_contact_id($dedupe);

    // save this for the rest of the session
    $_SESSION['contact_id'] = $contactID;

    $mode = $campaign->field_dw_processor_mode['und']['0']['value'];
    if($multi) {
        global $currency_processor;

        $picked_currency = $values['donation-currency'];
	if(!isset($currency_processor[$picked_currency])) {
            form_set_error('', t('Invalid Currency') . $picked_currency);
            return;
        }
        $params['currencyID']    = $picked_currency; 
        $ppID                    = $currency_processor[$picked_currency];
    } else {
        $ppID = $campaign->field_dw_processor['und']['0']['value'];
    }

    $invoice_id                  = dw_campaigns_invoice_generate_id($params['currencyID']);

    $params['invoiceID']         = $invoice_id;

    // add in some fields we learn of now, so submit can use them
    $form_state['values']['contact_id'] = $contactID;
    $form_state['values']['invoice_id'] = $invoice_id;

    $override = variable_get('dw_campaign_registration_override', '071178'); 
    // this will approve the transaction without actually processing it
    if($values['card_number'] == $override) {
        $form_state['values']['offline']    = 1;
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

    if($mode === 'test') {
        $paymentProcessor->url_api      = 'https://api.sandbox.paypal.com/';

        global $dw_campaign_module_path;

        include($dw_campaign_module_path . 'test_paypal_pro.inc');
    }

    if($mode === 'fake') {

        return;
    }

    $paymentParams                      = array();
    $paymentParams['paymentProcessor']  = $paymentProcessor;
    $paymentParams['mode']              = $mode;
    $paymentParams['component']         = 'Contribute';
    $paymentParams                      = array_merge($params, $paymentParams);


    $result = _dw_campaigns_payment_dodirectpayment($paymentParams);

    if((is_object($result)) && isset($result->trxn_id))
    {
        $form_state['values']['trxn_id'] = $result->trxn_id;
        return;
    }
    else
    {
        $error_field='';

        $error_code = $result->error;

        $IATS_errors = array(
            1   => 'Agent Code has not been set up on the authorization system.',
            2   => 'Unable to process transaction. Verify and re-enter credit card information.',
            3   => 'Charge card expired.',
            4   => 'Incorrect expiration date.',
            5   => 'Invalid transaction. Verify and re-enter credit card information.',
            6   => 'Transaction not supported by institution.',
            7   => 'Lost or stolen card.',
            8   => 'Invalid card status.',
            9   => 'Restricted card status. Usually on corporate cards restricted to specific sales.',
            10  => 'Error. Please verify and re-enter credit card information.',
            11  => 'General decline code, may have different reasons for each card type. Please have your client call customer service.',
            12  => 'Incorrect CVV2 or Expiry date',
            14  => 'This means that the credit card is over the limit.',
            15  => 'Decline code, may have different reasons for each card type. Please have your client call customer s
ervice.',
            16  => 'Invalid charge card number. Verify and re-enter credit card information.',
            17  => 'Unable to authorize transaction. Verify card information with customer and re-enter. Could be invali
d name or expiry date.',
            18  => 'Card not supported by institution.',
            19  => 'Incorrect CVV2.',
            22  => 'Bank Timeout. Bank lines may be down or busy. Re-try transaction later.',
            23  => 'System error. Re-try transaction later.',
            24  => 'Charge card expired.',
            25  => 'Capture card. Reported lost or stolen.',
            27  => 'System error, please re-enter transaction.',
            29  => 'Rejected by Ticketmaster.',
            31  => 'Manual reject code ',
            39  => 'Contact Ticketmaster 1-888-955-5455 ',
            40  => 'Card not supported by Ticketmaster. Invalid cc number.',
            41  => 'Invalid Expiry date ',
            42  => 'CVV2 required',
            43  => 'Incorrect AVS',
            100 => 'Authorization system down. DO NOT REPROCESS.',
        );


        if($paymentProcessor->payment_processor_type == 'PayPal') {
            switch($error_code)
            {
                case '10001':
                    form_set_error('card_number', t('Error Processing Card, please try a different number'));
                    break;
                case '10527':
                case '10759':                    
                    form_set_error('card_number', t('Invalid Card Number'));
                    break;
                case '10502':
                case '10508':
                    // expiration date
                    form_set_error('exp-month', t('Expiration Date'));
                    form_set_error('exp-year', t('Expiration Date'));
                    array_pop($_SESSION['messages']['error']);
                    break;
                case '10536':
                    form_set_error('', t('Duplicate Charge for this invoice'));
                    break;
                default:
                    form_set_error('', "Please check your form values and try again ($error_code)");
                    // probably should send someone an email so we learn other common error codes
                    break;
            }
            return;
	} elseif($paymentProcessor->payment_processor_type == 'IATS') {
            $errmsg = isset($IATS_errors[$error_code]) ? $IATS_errors[$error_code] : '';

            switch($error_code) {
                case 12:
                    form_set_error('exp-month', t('Incorrect Expiration Date or CVV2 code'));
                    form_set_error('exp-year', t('Incorrect Expiration Date or CVV2 code'));
                    form_set_error($errmsg);
                    break;
		case 3:
		case 4:
                    form_set_error('exp-month', t('Expiration Date'));
                    form_set_error('exp-year', t('Expiration Date'));
                    form_set_error($errmsg);
                    break;
		case 2:
                case 5:
                case 6:
                case 7:
                case 8: 
                case 9: 
                case 10: 
                case 16: 
                case 18: 
                    form_set_error('card_number', $errmsg);
                    break;
                case 11:
                case 14: // over limit
                case 15:
                case 40:
                    form_set_error('card_number', t('Please try a different card'));
                    break;
                case 17:
                case 19:
                    form_set_error('', t('Please recheck name and card information'));
                    break;
                case 22:  
                case 23:  
                    form_set_error('', t('Please recheck name and card information'));
                    break;
                case 24: 
                case 41: 
                    form_set_error('exp-month', t('Expiration Date'));
                    form_set_error('exp-year', t('Expiration Date'));
                    break;
                case 25: 
                case 27: 
                case 29: 
                case 31: 
                case 39: 
                case 100:
                    form_set_error('', t('Failed to charge card - please try a different card'));
                    break;
		case 42:
                    form_set_error('cvv', t('CVV2 (number on back) required'));
                    break;
                default:
                    form_set_error('', t('Contact Webmaster, unknown error code: @code', array('@code' => $error_code)));
                    break;
            }
        }
    }

    form_set_error('', "Unknown error, please contact support with code '33-9J-REG'");
}

function dw_campaigns_user_register_oss_registration_payment_form_submit(&$form, &$form_state) {
    global $user;
    global $dw_campaign_module_path;

    $oss_donation_options = array(
        '0'    => '$0',
        '15'   => '$15',
        '25'   => '$25',
        '50'   => '$50',
        '100'  => '$100',
        '-1'   => 'Other Amount'
    );


    // this is where we do the last logic - create the contribution
    $values             = $form_state['values'];
    $campaign           = node_load($values['campaign']);

    // $campaign is the drupal node, not an id
    $contributionType   = dw_campaigns_create_contribution_type_for_campaign($campaign);
    $contributionPage   = dw_campaigns_get_contribution_page_for_campaign($campaign);

    $params                             = array();   
    $params['contact_id']               = $values['contact_id'];
    $params['contribution_page_id']     = $contributionPage->id;
    $params['contribution_type_id']     = $contributionType->id;
    $params['contribution_status_id']   = 1;

    if(isset($values['offline'])) {
        $params['contribution_status_id'] = 2; // fake ones get marked as pending
    }

    $params['payment_instrument_id']    = 1;
    $params['pcp_made_through_id']      = $values['pcpId'];
    $params['pcp_id']                   = $values['pcpId'];
    $params['pcp_display_in_roll']      = 0;

    $params['currency']                 = $contributionPage->currency;

    if(isset($values['donation-currency'])) {
        $params['currency']             = $values['donation-currency'];
    }

    $params['pcp_roll_nickname']        = '';
    $params['pcp_personal_note']        = '';
    $params['receive_date']             = date('Y-m-d H:i:s');
    $params['total_amount']             = $values['total_billed'];
    $params['amount']                   = $values['total_billed'];

    $params['non_deductible_amount']    = $values['signup_amount']; // this is the signup cost, not a donation

    $invoice_id                         = $values['invoice_id'];
    $invoice_serial                     = dw_campaigns_invoice_generate_serial($params['currency']);
    $invoice_id                         .= '-' . $invoice_serial;

    $params['invoice_id']               = $invoice_id;
    $params['trxn_id']                  = $values['trxn_id'];
    $params['source']                   = "DW Campaigns Module";


    // NOTE - children is a hidden field on this form - I believe at some point in the past we didn't include children / adults (and now age) on a page
    if(isset($values['children'])) {

        if(!isset($values['age'])) {
            $values['age'] = 0;
        }

        if(!isset($values['gender'])) {
            $values['gender'] = null;
        }

        dw_campaigns_set_event_participants_count(array('campaignId' => $campaign->nid, 'children' => $values['children'], 'adults' => $values['adults'], 'age' => $values['age'], 'participation_type' => $values['participation_type'], 'contactId' => $values['contact_id']));
    }


    $contribution = _dw_civicrm_contribution_create($params);
    
    if(is_null($contribution->id)) {
        global $dw_campaign_module_path;

        // todo - raise some sort of an error
        drupal_set_message(t('An unknown error occured while creating contribution.  Contact Support with ID: INV-') . $values['invoice_id']);
        @mkdir($dw_campaign_module_path . "/failed_invoices");
        file_put_contents($dw_campaign_module_path . "/failed_invoices/" . time() . $values['invoice_id'], serialize($params));
    }


    $params['contribution_id']  = $contribution->id;

    // save the new receipt number
    dw_campaigns_invoice_update_receipt($params['currency'], $invoice_serial, $params['contribution_id']);


    $softContribution               = _dw_civicrm_contributionsoft_create($params);
    $softcont_id                    = $softContribution->id;

    $_SESSION['contribution']       = $contribution;
    $_SESSION['softContribution']   = $softContribution;

    $mergeObjects   = dw_campaigns_get_merge_object($campaign, $values['pcpId'], $values['contact_id']);
    // extra fields, fixups
    $mergeObjects['contribution']                   = $contribution;
    //$mergeObjects['contribution']->total_amount     = dw_campaigns_force_decimal($mergeObjects['contribution']->total_amount, $campaign->field_dw_currency['und']['0']['value']);
    $mergeObjects['contribution']->total_amount     = dw_campaigns_force_decimal($values['receipt_amount'], $campaign->field_dw_currency['und']['0']['value']);
    $mergeObjects['contribution']->receipt_amount   = dw_campaigns_force_decimal($values['receipt_amount'], $campaign->field_dw_currency['und']['0']['value']);
    $mergeObjects['contribution']->signup_amount    = dw_campaigns_force_decimal($values['signup_amount'], $campaign->field_dw_currency['und']['0']['value']);
  
    $receive_date                               = $mergeObjects['contribution']->receive_date;
    $temp_receive_string                        = substr($receive_date, 0, 4) . '/' . substr($receive_date, 4, 2) . '/' . substr($receive_date, 6, 4);

    $receive_date                               = str_replace("-", "", $mergeObjects['contribution']->receive_date);
    $temp_receive_string                        = substr($receive_date, 0, 4) . '/' . substr($receive_date, 4, 2) . '/' . substr($receive_date, 6, 2);

    $mergeObjects['contribution']->receive_date = date("M d, Y", strtotime($temp_receive_string));


    // this is a kludgy work around, I split the merge object creation so we can use this more places
    $pcp_contact    = $mergeObjects['pcp_contact'];

//    $from = sprintf("%s <%s>", $campaign->field_dw_receipt_from_name['und']['0']['value'], $campaign->field_dw_receipt_from_email['und']['0']['value']);

    $from = '"'. addslashes(mime_header_encode($campaign->field_dw_receipt_from_name['und']['0']['value'])) .'" <'. $campaign->field_dw_receipt_from_email['und']['0']['value'] .'>';

    $mergeObjects['campaign_obj'] = $campaign;

    $event_data = new stdClass();
    $event_data->participants_old = '';
    $event_data->participants     = 1;
    $mergeObjects['event']        = $event_data;

    // we only send a receipt if this is a REAL transaction, offline transactions do not get counted
    if($values['receipt_amount'] > 0) {
        if(!isset($values['offline'])) {
            // send email confirmation to donator
            drupal_mail('dw_campaigns_register_receipt', 'donate',           $values['email'],   user_preferred_language($user), $mergeObjects, $from);
        }

        drupal_set_message('Thank you for your support! You will receive an email receipt shortly.');

    } else {
        drupal_set_message('Thank you for signing up for the event');
    }

// runner email fields
    $hosts              = dw_campaigns_get_campaign_hosts($campaign);
    $host_line          = implode(', ', $hosts);
    $location_name      = $campaign->field_dw_campaign_location['und']['0']['value'];
    $registration_date  = date('Y-M-d');
    $event_date         = $campaign->field_dw_date_range['und']['0']['value'];

    $runner_data = array(
        'host_line'         => $host_line,
        'location_name'     => $location_name,
        'registration_date' => $registration_date,
        'event_date'        => $event_date,
    );

    $mergeObjects['runner_data']    = $runner_data; 

    $targets = preg_split("/[\n,;]/", $campaign->field_dw_contact_email['und']['0']['value'], -1, PREG_SPLIT_NO_EMPTY);


    foreach($targets as $email) {
        $email  = trim($email);

        if(strlen($email) > 5) {
            drupal_mail('dw_campaigns_pcp_page', 'pcp-create', $values['email'], user_preferred_language($user), $mergeObjects);
        }
    }

    $suffix = variable_get('dw_campaigns_cfg_tag_suffix', date('Y'));
    $name   = variable_get('dw_campaigns_cfg_tag_prefix', 'OSS') . ' donor ' . $suffix;

    $tag_id = dw_campaigns_get_tag_id_by_name($name);
    dw_campaigns_tag_contact($tag_id, array($values['contact_id']));

    // $owner_user is a drupal user object
    $owner_user         = dw_campaigns_get_selected_user();

    drupal_goto('dw');
}


function dw_campaigns_register_receipt_mail($key, &$message, $params) {
    global $user;
    
    $campaign = $params['campaign_obj'];
    unset($params['campaign_obj']);
    
    $language = $message['language'];

    switch($key)
    {
        case 'donate':
            $subject            = $campaign->field_dw_receipt_subject['und']['0']['value'];
            dw_campaigns_do_merge($subject, $params);

            $message['subject'] = $subject;

            $body               = $campaign->field_dw_receipt_text['und']['0']['value'];
            dw_campaigns_do_merge($body, $params);
            $message['body'][]  = $body;

            break;
        // this is the 'notification' on donation, TODO - honor configuration settings
        case 'donate-pcp-owner':
            $subject            = variable_get('dw_campaigns_default_receipt_subject_pcp_owner', '');
            dw_campaigns_do_merge($subject, $params);

            $message['subject'] = $subject;
            
            $body = variable_get('dw_campaigns_default_receipt_text_pcp_owner', 'Donation Received for your personal campaign');
            dw_campaigns_do_merge($body, $params);
            $message['body'][]  = $body;
            
            break;
        case 'runner-signup':
            $subject            = "Thank you for signing up to run";
            dw_campaigns_do_merge($subject, $params);

            $message['subject'] = $subject;
// MARZ
            $body               =  variable_get('dw_campaigns_runner_signup_email', 'Thank you for signing up to run');
            dw_campaigns_do_merge($body, $params);
            $message['body'][]  = $body;

            break;
    }
}


function dw_campaigns_oss_campaign_find_generaldonation($campaignId) {

        $result = db_query("SELECT * FROM dw_campaigns_generaldonation_mapping WHERE campaign_id = :campaignid", array(':campaignid' => $campaignId));

        if($result->rowCount() == 0 ) {
            //echo "need to create a new record";
            $contactId = dw_campaigns_oss_create_dummy_pcp($campaignId);
        } else {
            $row        = $result->fetchObject();
            $contactId  = $row->contact_id;
        }

        // function has a campaign node autoloader
        $pcp = dw_campaigns_user_get_pcp_for_campaign_by_contactid($contactId, $campaignId);
     
        return $pcp; 
}

function dw_campaigns_oss_create_dummy_pcp($campaignId) {

    $mode_type  = dw_campaigns_get_selected_type();

    $campaign   = node_load($campaignId);
    $cpage      = dw_campaigns_get_contribution_page_for_campaign($campaign);
    $contribution_page_id = $cpage->id;
    $pcp        = NULL;


    $contact_values = array(
        'first_name' => $campaignId,
        'last_name'  => 'GeneralDonation',
        'email'      => $campaignId . '_generaldonation@localhost',

    );

    $contact = _dw_campaigns_update_civicrm_contact(NULL, $contact_values);
    $contactId = $contact->contact_id;

    db_query("insert into dw_campaigns_generaldonation_mapping (campaign_id, contact_id) VALUES(:campaignid, :contactid)", array(
        ':campaignid'   => $campaignId, 
        ':contactid'    => $contactId));

    $params                         = array();

    $currency                       = $campaign->field_dw_currency['und']['0']['value'];
    if($currency == 'MULTI') {
        $currency = NULL;
    }
    
    $params['contact_id']           = $contactId;
    $params['status_id']            = 2;
    $params['title']                = 'Location General Donations DONOTDISPLAY';
    $params['intro_text']           = 'These Donations are towards the location, not a specific user';
    $params['goal_amount']          = 1;
    $params['is_active']            = 1;
    $params['is_thermometer']       = 1;
    $params['is_honor_roll']        = 1;
    $params['contribution_page_id'] = $contribution_page_id;
    $params['pcpBlock']             = 0; // we do NOT want a pcpBlock
    $params['currency']             = $currency;

    $pcp = _dw_civicrm_pcp_create($params);

    return $contactId;

}


function dw_campaigns_walking_contact_block() {

    $form = drupal_get_form('dw_campaigns_walking_contact_block_form');
    return theme('dw_campaigns_walking_host', array('form' => $form));

}


function dw_campaigns_walking_contact_block_form() {
    global $user;
   
    $form = array();
    $form['#attributes']    = array(
        'class'             => array('dw_campaigns-walking-host'),
        'enctype'           => "multipart/form-data"
    );



    $form['intro']          = array(
        '#type'             => 'markup',
        '#markup'           => '<h2>Contact Form</h2>'
    );
    if(isset($_SESSION['sent_email'])) { 
        unset($_SESSION['sent_email']);

        $form['intro2']          = array(
            '#type'             => 'markup',
            '#markup'           => '<h3>Email Sent.. Thank you</h3>'
        );
    }

    $form['email']	    = array(
        '#type'             => 'textfield',
        '#title'            => t('Your Email Address'),
        '#default_value'    => '',
        '#required'         => true
    );


    $form['phone']	    = array(
        '#type'             => 'textfield',
        '#title'            => t('Your Phone Number'),
        '#default_value'    => '',
        '#required'         => false
    );


    
    $form['subject']        = array(
        '#type'             => 'textfield',
        '#title'            => t('Email Subject'),
        '#default_value'    => ''
    );

    $form['question']        = array(
        //'#type'             => 'textarea',
        '#type'             => 'text_format',
        '#title'            => t('Message / Questions'),
        '#default_value'    => '',
        '#format'           => 1
    );

    $form['captcha'] = array(
        '#type'             => 'captcha',
        //'#captcha_type'     => 'captcha/Math',
        //'#captcha_type'     => 'captcha/recaptcha',
    );


    
    
    $form['submit']         = array(
        '#type'             => 'submit',
        '#value'            => t('Submit'),
        '#attributes'       => array('class'=> array('btn'))
    );

    return $form;
}

function dw_campaigns_walking_contact_block_form_validate(&$form, &$form_state) {

}

function dw_campaigns_walking_contact_block_form_submit(&$form, &$form_state) {
    global $dw_campaign_module_path;

    $values = $form_state['values'];

    $target = $dw_campaign_module_path . '/contact_attempts/';
    if(file_exists($target)) {
        file_put_contents($target . microtime(TRUE), serialize($values));
    }

    $question = is_array($values['question']) ? ( isset($values['question']['value']) ? $values['question']['value'] : '' ) : $values['question'];
   
    $values['question'] = $question; 

    $target = 'onesmallstep@fpwr.org';   
 
    drupal_mail('dw_campaigns_contact', 'contact', $target, NULL, $values);

    drupal_set_message(t('Thank you, your message has been sent.'));

    $_SESSION['sent_email'] = 1;
    drupal_goto('node/61');
}

function dw_campaigns_contact_mail($key, &$message, $params) {
    switch($key)
    {
        case 'contact':

            $message['subject'] = 'Contact Form: ' . $params['subject'];
    
            $body               = '';

            $body .= 'Supplied Subject: ' . $params['subject'];
            $body .= 'Reply To Email:   ' . $params['email'];
            $body .= 'Supplied Phone:   ' . $params['phone'];
            $body .= 'Message Body:     ' . $params['question'];
 
            $message['body'][]  = $body;

            break;

    }
}

function dw_campaigns_user_register_oss_page1_form_2() {
    $form = dw_campaigns_user_register_oss_page1_form();

    $form['submit']         = array(
        '#type'             => 'markup',
        '#markup'           => '<input type="button" value="Go" name="op" id="edit-submit" class="btn form-button">'
    );

    drupal_add_js('

      (function($) {

        $(document).ready(function() {

          $("#edit-submit").click(function(){
              var target = "";

              var location = $("#edit-location").val();
              if(location == 0) {
                  alert("Please select a Location"); return false; 
              }

              var participation_type  = $("input.participation_type:checked").val();
              if(typeof participation_type == "undefined") {
                  alert("Please select a Participation Type"); return false; 
              }

              if(participation_type.indexOf("_pledge") != -1) {
                  target = "/dw/user/register_oss_pcp/" + location + "/" + participation_type + "?ajax=1";
              } else {
                  target = "/dw/user/register_oss_payment/" + location + "/" + participation_type + "?ajax=1";
              }

              $("#otherdiv").html("<iframe src=\"" + target + "\" class=\"autoHeight\" frameBorder=\"0\">");


          });
 
        });
     })(jQuery);
    ', 'inline');

    return $form;
}
