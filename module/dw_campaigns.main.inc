<?php

$dwSelectedCampaign = NULL;
$dwSelectedPcp      = NULL;
$dwSelecterUser     = NULL;
$dwSelectedCampaign = NULL;
$dwSelectedLocation = NULL;
$dwLastSearch       = NULL;

$dwGoalData         = NULL;

global $dwHasTabs;
$dwHasTabs          = NULL;

function dw_campaigns_set_hastabs() {
    global $dwHasTabs;

    $dwHasTabs = ' sidebar-right';
}

function dw_campaigns_get_hastabs() {
    global $dwHasTabs;

    return $dwHasTabs;
}

function dw_campaigns_set_selected_location($campaign_id) {

    global $dwSelectedLocation;
    
    $dwSelectedLocation = $campaign_id;
}


function dw_campaigns_get_selected_location() {

    global $dwSelectedLocation;
    
    // we probably want to cascade down and see if they are logged in, and if so where their pcp location is
    if(is_null($dwSelectedLocation)) {
        $dwSelectedLocation = isset($_SESSION['last_location'])?$_SESSION['last_location']:NULL;
    }
    
    return $dwSelectedLocation;
}

/**
 *  Campaigns are drupal content nodes of type 'Derby' or (in expansion) Walking - we store the actual full node
 */
function dw_campaigns_set_selected_campaign($campaign) {

    global  $dwSelectedCampaign;
    if($campaign) {
        $dwSelectedCampaign = clone $campaign;
    }
}

/**
 *  Returns the Derby/Walking node object
 */
function dw_campaigns_get_selected_campaign() {

    global  $dwSelectedCampaign;
    
    return $dwSelectedCampaign;
}

function dw_campaigns_set_selected_type($type) {

    global  $dwSelectedType;
    
    $dwSelectedType = $type;
}

function dw_campaigns_get_selected_type() {

    global  $dwSelectedType;
    
    return $dwSelectedType;
}

function dw_campaigns_set_goal_results($goal) {
    global $dwGoalData;

    $dwGoalData = $goal;
}

function dw_campaigns_get_goal_results() {
    global $dwGoalData;

    return $dwGoalData;
}

/**
 *  PCPs are CiviCRM Personal Campaign Pages - we store a copy of the object
 */
function dw_campaigns_get_selected_pcp() {
    
    global  $dwSelectedPcp;
    
    return $dwSelectedPcp;
}

/**
 *  Returns the PCP Object
 */
function dw_campaigns_set_selected_pcp($pcp) {
    
    global  $dwSelectedPcp;
    
    $dwSelectedPcp = $pcp;
}

/**
 *  Users are drupal user objects (this is so we can get/set the 'owner' of a page)
 */
function dw_campaigns_get_selected_user() {
    
    global  $dwSelectedUser;
    
    return $dwSelectedUser;
}

/**
 *  Returns the Drupal user objects (this is so we can get/set the 'owner' of a page)
 */
function dw_campaigns_set_selected_user($user) {
    
    global  $dwSelectedUser;
    
    $dwSelectedUser = $user;
}

function dw_campaigns_set_last_search($search) {
    global $dwLastSearch;

    $dwLastSearch   = $search;
}

function dw_campaigns_get_last_search() {
    global $dwLastSearch;

    return $dwLastSearch;
}

function dw_campaigns_user_is_email_unique($email) {

    //check email
    $result 	= db_query("SELECT * FROM {users} u WHERE mail = :email", array(':email' => $email));
    $user       = $result->fetchObject();

    return !(isset($user->uid));
}

function dw_campaigns_user_is_email_unique_civi($email) {
    
    return true; // they didn't want this check
    
    $params = array(
        'email'    => $email,
        'returnFirst'   => 1
    );
    $res    = _dw_civicrm_contact_get($params);
    if(is_null($res->contact_id)) {
        return true;
    }
    return false;
}



/**
 * This function acts as glue between a drupal node and the civicrm backend.  When a new dw_campaign content is created in drupal
 * this function creates a contribution page in civi that corresponds with it
 */
function dw_campaigns_create_contribution_page_for_campaign($node) {
    $hasPage = false;
    
    $node = (is_object($node)) ? $node : node_load($node);

    //if we are creating a page, we have to create a contribution type first
    $contributionType   = dw_campaigns_create_contribution_type_for_campaign($node);
    
    //check if we are updating, id will come back blank if nothing was found
    $contributionPage   = dw_campaigns_get_contribution_page_for_campaign($node);
    if(isset($contributionPage->id) && !is_null($contributionPage->id) && !empty($contributionPage->id)) {
        $params['id']       = $contributionPage->id;
        $hasPage            = true;
    }

    $temp_status            = isset($node->field_dw_campaign_status['und']['0']['value']) ? $node->field_dw_campaign_status['und']['0']['value'] : NULL;

    //$node_enabled           = ($temp_status == 'enabled' || $temp_status == 'completed') ? '1' : '0'; 
    $node_enabled           = ($temp_status == 'enabled') ? '1' : '0'; 
    
    $params['title']                    = $node->title;
    $params['contribution_type_id']     = $contributionType->id;
    $params['intro_text']               = $node->body;
    $params['footer_text']              = '';
    $params['is_for_organization']      = '0';
    $params['for_organization']         = 'I am contributing on behalf of an organization.';
    $params['goal_amount']              = $node->field_dw_fundraising_goal['und']['0']['value'];
    $params['is_active']                = $node_enabled;
    $params['honor_block_title']        = '';
    $params['honor_block_text']         = '';
    $params['start_date']               = '';
    $params['start_date_time']          = '';
    $params['end_date']                 = '';
    $params['end_date_time']            = '';
    
    
    $params['created_date']             = date('YmdHis');
    
    $params['is_credit_card_only']      = '';
    $params['honor_block_is_active']    = '';
    $params['is_allow_other_amount']    = '1';
    $params['min_amount']               = '0';
    $params['max_amount']               = '99999';
    
    
    $params['amount_block_is_active']   = 1;
    $params['is_monetary']              = 1;
    $params['payment_processor_id']     = 1;

    $params['is_email_receipt']         = '1';    
    $params['thankyou_title']           = isset($node->field_dw_thankyou_title['und']['0']['value']) ? $node->field_dw_thankyou_title['und']['0']['value'] : '';
    $params['thankyou_text']            = isset($node->field_dw_thankyou_text['und']['0']['value']) ? $node->field_dw_thankyou_text['und']['0']['value'] : '';
    $params['thankyou_footer']          = isset($node->field_dw_thankyou_footer['und']['0']['value']) ? $node->field_dw_thankyou_footer['und']['0']['value'] : '';
    $params['receipt_from_name']        = isset($node->field_dw_receipt_from_name['und']['0']['value']) ? $node->field_dw_receipt_from_name['und']['0']['value'] : '';
    $params['receipt_from_email']       = isset($node->field_dw_receipt_from_email['und']['0']['value']) ? $node->field_dw_receipt_from_email['und']['0']['value'] : '';
    $params['receipt_text']             = isset($node->field_dw_receipt_text['und']['0']['value']) ? $node->field_dw_receipt_text['und']['0']['value'] : '';
    $params['cc_receipt']               = isset($node->field_dw_cc_receipt['und']['0']['value']) ? $node->field_dw_cc_receipt['und']['0']['value'] : '';
    $params['bcc_receipt']              = isset($node->field_dw_bcc_receipt['und']['0']['value']) ? $node->field_dw_bcc_receipt['und']['0']['value'] : '';

    $params['currency']                 = $node->field_dw_currency['und']['0']['value'];

    $page = _dw_civicrm_contributionpage_create($params);

    // this saves a record so we can relate node ids to civi ids (from both sides)    

    if($temp_status != 'completed') {
        _dw_save_node_to_civi_campaign_glue($node->nid, $page->id, $node_enabled);
    }
    
    // if this is updating an existing contribution page we need to update the titles in all the pcps (not the pcpblock) that are tied to that contribution page
    if($hasPage) {
        $params = array();
        $params['title']                = $node->title;
        $params['contribution_page_id'] = $page->id;
   
// Susan wanted the sync titles disabled 
//        _dw_civicrm_pcp_sync_titles($params);
    }

    //enable pcp's on this contribution page by inserting a new record into pcpblock
    //see if a pcpblock already exists so we can update it
    $params = array();
    $params['entity_id']    = $page->id;
    $params['pcpBlock'] = true;
    $params['returnFirst']  = 1;

    $pcpBlock = _dw_civicrm_pcp_get($params);

    // create or update the pcpBlock
    $params                          = array();
    if(isset($pcpBlock->id) && !is_null($pcpBlock->id) && !empty($pcpBlock->id)) {
        $params['id']                    = $pcpBlock->id;
    }
    $params['is_active']             = 1;
    $params['is_approval_needed']    = 1;
    $params['supporter_profile_id']  = 2;
    $params['is_tellfriend_enabled'] = 1;
    $params['tellfriend_limit']      = 9;
    $params['link_text']             = 'Donate Now';
    $params['notify_email']          = '';
    $params['entity_id']             = $page->id;
    $params['entity_table']          = 'civicrm_contribution_page';
    
    $params['pcpBlock']              = true;
    
    $pcpBlock = _dw_civicrm_pcp_create($params);
    // we don't return anything
    
    if($node->field_dw_create_event['und']['0']['value'] != 0 ) {
        // create the event also
        $event = dw_campaigns_create_event_registration_for_campaign($node, $contributionType->id);
        _dw_save_node_to_civi_event_glue($node->nid, $event->event_id);
    }

}

/**
 * Creates a contribution type from a campaign node
 * 
 * @param stdClass $node
 * @return CRM_Contribute_DAO_ContributionType the contribution type id
 */
function dw_campaigns_create_contribution_type_for_campaign($node) {
    $node = (is_object($node)) ? $node : node_load($node);

    //determine if we are updating by checking if a page already exists for the node
    //if one does, set the id of the type_id so it's saved
    $contributionPage   = dw_campaigns_get_contribution_page_for_campaign($node);

    $params = array();
    if(isset($contributionPage->contribution_type_id) && !is_null($contributionPage->contribution_type_id) && !empty($contributionPage->contribution_type_id)) {
        
        $params['id']               = $contributionPage->contribution_type_id; 
    }

    $params['name']             = "Campaign " . $node->nid . ": " . $node->title;
    $params['description']      = $node->body;
    $params['is_deductible'] 	= 1;
    $params['is_active'] 	= 1;


    $type = _dw_civicrm_contributiontype_create($params);
    return $type;
}



/**
 * Passing this function a node or node id will cause it to
 * search for any associated contribution page, if one is found, it is returned
 * 
 * @param stdObj $node
 * @return mixed either the page it found or null
 */
function dw_campaigns_get_contribution_page_for_campaign($node) {
    static $pages = array();
   
    $node = (is_object($node)) ? $node : node_load($node);
    
    $res = new stdClass;
  
    // invalid node 
    if(!isset($node->nid)) {
            return $res;
    }
 
    if(!isset($pages[$node->nid])) {
    
        $result  = db_query("SELECT contribution_page_id FROM {dw_campaigns_campaigns_to_contribution_pages} WHERE nid = :nid", array(':nid' => $node->nid));

        //if(count($result) == 0 ) {
        if($result->rowCount() == 0 ) {
            return $res;
        }
      
        $row        = $result->fetchObject();
        
        // this isn't necessary most of the time, since we don't care about anything BUT the page id :)
        $page       = dw_campaigns_get_contribution_page_from_id($row->contribution_page_id);
        
        $pages[$node->nid] = $page;
  
    }
    
    return $pages[$node->nid];
}

// TODO
function dw_campaigns_get_event_registration_for_campaign($node) {
    static $pages = array();

    $res = new stdClass;

    $node = (is_object($node)) ? $node : node_load($node);
   

    if(!isset($pages[$node->nid])) {
    
        $result  = db_query("SELECT event_page_id FROM {dw_campaigns_campaigns_to_contribution_pages} WHERE nid = :nid", array(':nid' => $node->nid));
        
        //if(count($result) == 0 ) {
        if($result->rowCount() == 0) {
            return $res;
        }
      
        $row        = $result->fetchObject();
        if(is_null($row->event_page_id)) {
            return $res;
        }

        // get event by event id
        $event      = _dw_civicrm_event_get(array('id' => $row->event_page_id));

        $pages[$node->nid] = $event;
    }
    
    return $pages[$node->nid];
}

function dw_campaigns_get_event_page_id_from_campaign($campaign) {
    //$row        = db_fetch_object($results);

    $results    = db_query("SELECT event_page_id FROM {dw_campaigns_campaigns_to_contribution_pages} WHERE nid = :nid", array(':nid' => $campaign->nid));
    $row        = $results->fetchObject();
    
    return $row->event_page_id;    
}

function dw_campaigns_get_event_participants_count($campaignId, $pcpId, $contactId = 0) {
    if($pcpId == 0 && $contactId == 0) {
        return NULL;
    }
    
    if($pcpId > 0) {

        $results    = db_query("SELECT children,adults FROM dw_campaigns_event_participants WHERE campaign_id = :campaignid and pcp_id = :pcpid", array(
            ':campaignid'   => $campaignId, 
            ':pcpid'        => $pcpId));
    } else {

        $results    = db_query("SELECT children,adults FROM dw_campaigns_event_participants WHERE campaign_id = :campaignid and contact_id = :contactid", array(
            ':campaignid' => $campaignId, 
            'contactid'   => $contactId));
    }

    $row        = $results->fetchObject();

    $children   = isset($row->children) ? $row->children : 0;
    $adults     = isset($row->adults) ? $row->adults : 0;

    return array($children, $adults);
}


// NOTE - SCHEMA CHANGE: ALTER TABLE `dw_campaigns_event_participants` ADD `gender` ENUM( 'M', 'F' ) NULL DEFAULT NULL COMMENT 'Null / Male / Female';

//function dw_campaigns_set_event_participants_count($campaignId, $pcpId, $children, $adults, $age = 0, $participation_type = 'adult_pledge', $contactId = 0) {
function dw_campaigns_set_event_participants_count($params) {

    global $dw_campaign_module_path;

    $defaults = array(
        'campaignId'         => null, 
        'pcpId'              => null,
        'age'                => 0,
        'gender'             => null,
        'children'           => 0,
        'adults'             => 0,
        'participation_type' => 'adult_pledge', 
        'contactId'          => 0
    );

    // set all the values we except as local variables to keep the code mostly the same
    foreach($defaults as $key => $value) {
        $$key = isset($params[$key]) ? $params[$key] : $value;
    }    

    if($age == '' || is_null($age)) {
        $age    = 0;
    }

    $gender = strtolower($gender);

    if($gender != 'm' && $gender != 'f') {
        $gender    = null;
    }

    $campaignId  = intval($campaignId);
    $pcpId       = intval($pcpId);
    $contactId   = intval($contactId);

    if($pcpId == 0 && $contactId == 0) {
        drupal_set_message(t('To add participants you must provide a personal campaign page id or a contact id'), 'error');
        return;
    }

// can't use db_merge because it goes off of unique keys   
    if($pcpId > 0) {

        $results    = db_query("DELETE from {dw_campaigns_event_participants} WHERE campaign_id = :campaignid and pcp_id = :pcpid", array(
        ':campaignid'   => $campaignId, 
        ':pcpid'        => $pcpId));

    } else {
        $results    = db_query("DELETE from dw_campaigns_event_participants WHERE campaign_id = :campaignid and contact_id = :contactid", array(':campaignid' => $campaignId, ':contactid' => $contactId));
    }
 

    $num_attending = $children + $adults;

    db_query("insert into dw_campaigns_event_participants (campaign_id, pcp_id, participants, participant_type, contact_id, children, adults, age, gender) VALUES(:campaignid, :pcpid, :participants, :participanttype, :contactid, :children, :adults, :age, :gender)", array(
        ':campaignid'       => $campaignId, 
        ':pcpid'            => $pcpId, 
        ':participants'     => $num_attending, 
        ':participanttype'  => $participation_type, 
        ':contactid'        => $contactId,
        ':children'         => $children,
        ':adults'           => $adults,
        ':age'              => $age,
        ':gender'           => $gender
    ));
}

function dw_campaigns_get_contribution_page_from_id($id) {
    
    static $pages =  NULL;
    
    if(is_null($pages)) {
        $pages                 = array();
        $params['rowCount']    = 999999;

        $raw_pages             = _dw_civicrm_contributionpage_get($params);

        foreach($raw_pages as $key => $page) {
            $pages[$page->id] = $page;
        }
    }
    
    return $pages[$id];
}

function _dw_campaigns_get_pcp_by_contact($contact_id, $return_first = FALSE) {

    $raw_pcps   = _dw_campaigns_get_all_pcps();

    $pcps       = new stdClass;

    foreach($raw_pcps as $pcp) {
        if($pcp->contact_id == $contact_id) {
            $one = clone $pcp;
            
            if($return_first) {
                return $one;
            }

            $id         = $pcp->id;

            $pcps->$id  = $one;
        }
    }

    return $pcps;
}

function _dw_campaigns_get_pcp_by_page_and_contact($page_id, $contact_id, $return_first = FALSE) {
    $count      = 0;

    $raw_pcps   = _dw_campaigns_get_all_pcps();

    $pcps       = new stdClass;

    foreach($raw_pcps as $pcp) {
        if($pcp->contribution_page_id == $page_id && $pcp->contact_id == $contact_id) {
            $one = clone $pcp;
            
            if($return_first) {
                return $one;
            }

            $id         = $pcp->id;

            $pcps->$id  = $one;
            $count++;
        }
    }

    if($count == 0) {
        return NULL;
    }

    return $pcps;
}

/**
 * Loads the personal campaign page for a user for a specific campaign
 * @param stdClass $user the user we are checking for
 * @param stdClass $campaign the campaign node or node id
 * @return stdClass
 */
function dw_campaigns_user_get_pcp_for_campaign($user, $campaign) {
    $campaign = (is_object($campaign)) ? $campaign : node_load($campaign);

    $params = array();
    $params['pcpBlock'] = 0;
    
    $page = dw_campaigns_get_contribution_page_for_campaign($campaign);

    if(is_null($page->id)) {
        return $page; // this is a failure to lookup!
    }

    $contactId                      = _dw_campaigns_user_get_contact_id($user);

    $pcp    = _dw_campaigns_get_pcp_by_page_and_contact($page->id, $contactId, TRUE);

    return $pcp;  
}

function dw_campaigns_user_get_pcp_for_campaign_by_contactid($contactId, $campaign) {
    $campaign = (is_object($campaign)) ? $campaign : node_load($campaign);

    $params = array();
    $params['pcpBlock'] = 0;
    
    $page = dw_campaigns_get_contribution_page_for_campaign($campaign);

    if(is_null($page->id)) {
        return $page; // this is a failure to lookup!
    }

    $pcp    = _dw_campaigns_get_pcp_by_page_and_contact($page->id, $contactId, TRUE);

    return $pcp;  
}



/**
 * Finds the first active PCP that is related to a user (for walking)
 * @param stdClass $user the user we are checking for
 * @return stdClass
 */
function dw_campaigns_user_get_pcp_for_walking($user) {
    static $pcp_result = array();

    if(!isset($pcp_result[$user->uid])) {
        $campaigns  = dw_campaigns_get_active_campaigns(TRUE);
        $campaign_values = array_keys($campaigns);
    
        $params = array();
        $params['pcpBlock'] = 0;
       
        $contactId  = _dw_campaigns_user_get_contact_id($user);
    
        $pcps       = _dw_campaigns_get_pcp_by_contact($contactId, FALSE);

        foreach($pcps as $pcp) {
            if(in_array($pcp->contribution_page_id, $campaign_values)) {
                $pcp_result[$user->uid] = $pcp;
                break;
            }
        }
    }

    return isset($pcp_result[$user->uid]) ? $pcp_result[$user->uid] : NULL;
}


/**
 * Finds the non-active PCPs that is related to a user (for walking)
 * @param stdClass $user the user we are checking for
 * @return array
 */
function dw_campaigns_user_get_historical_pcps($user) {
    static $found      = array();
    
    $campaigns       = dw_campaigns_get_active_campaigns(TRUE);

    if(is_null($campaigns) || count($campaigns) == 0) {
        return $found[$user->uid];
    }

    $campaign_values = array_keys($campaigns);

    $all_campaigns   = dw_campaigns_get_all_campaigns(TRUE);

    if(!isset($found[$user->uid])) {
        $found[$user->uid]  = array();

        $params             = array();
        $params['pcpBlock'] = 0;
       
        $contactId  = _dw_campaigns_user_get_contact_id($user);
    
        $pcps       = _dw_campaigns_get_pcp_by_contact($contactId, FALSE);
   
        foreach($pcps as $pcp) {
            if(!in_array($pcp->contribution_page_id, $campaign_values)) {
                $contribution_page_id   = $pcp->contribution_page_id;
                if(isset($all_campaigns[$contribution_page_id])) {
                    $found[$user->uid][$pcp->id] = $pcp;  
                }
            }
        }
    }
    return $found[$user->uid];
}

function dw_campaigns_user_get_historical_pcps_check() {
    global $user;
    $results = dw_campaigns_user_get_historical_pcps($user);
    return (count($results) > 0);
}

// takes pcp id, returns pcp (not PCPblock)
function dw_campaigns_user_get_pcp_by_id($pcpId, $reset = FALSE)
{
    $params = array();

    $raw_pcps   = _dw_campaigns_get_all_pcps($reset);

    if(isset($raw_pcps->$pcpId)) {
        $pcp    = clone $raw_pcps->$pcpId;
    } else {
        $pcp    = new stdClass;
    }
    
    return $pcp;
}

// takes pcpBlock id, returns pcpBlock
function dw_campaigns_user_get_pcpblock_by_id($pcpBlockId)
{
    $params = array();
    $params['pcpBlock']     = 1;
    $params['id']           = $pcpBlockId;
    $params['returnFirst']  = 1;

    $pcp = _dw_civicrm_pcp_get($params);
    
    return $pcp;
}

function dw_campaigns_get_campaign_from_contribution_page($contributionPage) {

    $results    = db_query("SELECT nid FROM {dw_campaigns_campaigns_to_contribution_pages} WHERE contribution_page_id = :contributionpage", array(':contributionpage' => $contributionPage->id));
    $row        = $results->fetchObject();

    if(!isset($row->nid)) {
        return NULL;
    }

    return node_load($row->nid);    
}

/**
 *   This returns only active campaigns
 */
function dw_campaigns_get_active_campaigns($keyedByContributionPage = FALSE) {

    static $campaigns = NULL;
    static $campaigns_cp = NULL;
    static $campaigns_ni = NULL;

    if(is_null($campaigns)) {

        $results    = db_query("SELECT nid, contribution_page_id FROM {dw_campaigns_campaigns_to_contribution_pages} where active='Y'");
    
        foreach($results as $row) {
            $node = node_load($row->nid);
            if(is_object($node)) {
                $campaigns[] = $node;
                $campaigns_cp[$row->contribution_page_id] = $node;
                $campaigns_ni[$row->nid] = $node;
            }
        }
    }
    if($keyedByContributionPage === TRUE) {
        return $campaigns_cp;
    } else {
        if($keyedByContributionPage === 'nodeid') {
            return $campaigns_ni;
        }
        return $campaigns;
    }
}
/**
 *   This will return all campaigns, even old ones
 */ 
function dw_campaigns_get_all_campaigns($keyedByContributionPage = FALSE) {

    static $campaigns = NULL;
    static $campaigns_cp = NULL;

    if(is_null($campaigns)) {

        $results    = db_query("SELECT nid, contribution_page_id FROM {dw_campaigns_campaigns_to_contribution_pages}");
    
        foreach($results as $row) {
            $node = node_load($row->nid);
            if(is_object($node)) {
                $campaigns[] = $node;
                $campaigns_cp[$row->contribution_page_id] = $node;
            }
        }
    }
    if($keyedByContributionPage) {
        return $campaigns_cp;
    } else {
        return $campaigns;
    }
}

function dw_campaigns_get_campaign_ids() {
    static $campaigns = NULL;
    if(is_null($campaigns)) {

        $results    = db_query("SELECT nid FROM {dw_campaigns_campaigns_to_contribution_pages} where active='Y'");
    
        foreach($results as $row) {
            $campaigns[] = $row->nid;    
        }
    }

    return $campaigns;
}

function dw_campaigns_get_inactive_campaign_ids() {
    static $campaigns = NULL;
    if(is_null($campaigns)) {

        $results    = db_query("SELECT nid FROM {dw_campaigns_campaigns_to_contribution_pages} where active='N'");
    
        foreach($results as $row) {
            $campaigns[] = $row->nid;    
        }
    }

    return $campaigns;
}

function dw_campaigns_get_all_campaign_ids() {
    static $campaigns = NULL;
    if(is_null($campaigns)) {

        $results    = db_query("SELECT nid FROM {dw_campaigns_campaigns_to_contribution_pages}");
    
        foreach($results as $row) {
            $campaigns[] = $row->nid;    
        }
    }

    return $campaigns;
}

function dw_campaigns_get_contribution_total_for_campaign($node) {

    $node = (is_object($node)) ? $node : node_load($node);
  
    $contributionPage   = dw_campaigns_get_contribution_page_for_campaign($node);

    $contributions      = dw_campaigns_get_contributions_by_pageid($contributionPage->id);   
    
    $total=0;

    foreach($contributions as $contribution)
    {
            $total += $contribution->total_amount;
    }


    return $total;

}

function dw_campaigns_get_contributions_by_pageid($contributionPageId, $keyed = FALSE) {

    static $pages = array();

    if(!isset($pages[$contributionPageId])) {

        $params                         = array();
        $params['contribution_page_id'] = $contributionPageId;
        $params['rowCount']             = 999999;
    
        $contributions                  = _dw_civicrm_contribution_get($params);

        $pages[$contributionPageId]     = $contributions;
    } 

    $contributions = $pages[$contributionPageId];

    if($keyed) {
        $temp           = array();
        foreach($contributions as $pre => $contribution) {
            $temp[$contribution->contribution_id]   = $contribution;
        }

        $contributions  = $temp;
    }

    return $contributions;
}


function dw_campaigns_get_contribution_by_id($contribution_id, $nocache = FALSE) {

    $contribution = NULL;

    $params                             = array();
    $params['contribution_id']          = $contribution_id;
    $params['rowCount']                 = 999999;
    if($nocache) {
        $params['nocache']              = time();
    }
    $contributions                      = _dw_civicrm_contribution_get($params);

    foreach($contributions as $contribution) {

    }

    return $contribution;
}

/**
 * Get the "soft" contributions to PCP pages
 * @param PCP_Object the pcp object
 * @return array the contributions if any were found
 */
function dw_campaigns_get_contributions_for_pcp($pcp, $by_id = FALSE) {

    static $contributions          = array();
    static $by_id_contributions    = array();
    static $grabbed_all            = FALSE;

    if(!isset($pcp->id)) {
        return NULL;
    }

    $pcp_id = $pcp->id;

    if(!$grabbed_all) {
        
        // REV2 - we should extend contributionsoft to handle a "in" clause so we don't look up pcps from other years?
        $grabbed_all        = TRUE;
        $params['rowCount'] = 999999;
        
        $softcontributions  = _dw_civicrm_contributionsoft_get($params);
        

        foreach($softcontributions as $softcontribution) {

            // some records don't have a pcp id, so lets only keep track if they do!
            if(isset($softcontribution->pcp_id)) {
                $t_pcp_id = $softcontribution->pcp_id;
                if(!isset($contributions[$t_pcp_id])) {
                    $contributions[$t_pcp_id] = array();
                }

                $contributions[$t_pcp_id][]                 = $softcontribution;

                $cont_id                                    = $softcontribution->contribution_id;
                $by_id_contributions[$t_pcp_id][$cont_id]   = $softcontribution;
            }
        }
    }


    if(!isset($contributions[$pcp_id])) {
        $contributions[$pcp_id] = array();
        //file_put_contents("/tmp/pcp_miss", $pcp_id . "\n", FILE_APPEND);
    }

    if($by_id) {
        return $by_id_contributions[$pcp_id];
    }

    return $contributions[$pcp_id];
}


/**
 * Tells you how much has been dontated for this pcp
 */
function dw_campaigns_get_contribution_total_for_pcp_OLD($pcp, $convertNONUSD = FALSE) {
    $softcontributions  = dw_campaigns_get_contributions_for_pcp($pcp);
    
    $total          = 0;
    $count          = 0;
    $offline        = 0;

    // lookup regular contributions
    $params                             = array();
    $params['contribution_page_id']     = $pcp->contribution_page_id;
    $params['rowCount']                 = 999999;
    $params['contribution_status_id']   = 2; // pending only
    $contributions                      = _dw_civicrm_contribution_get($params);


    foreach($softcontributions as $softcontribution) {
        $key = '_' . $softcontribution->contribution_id;
        $contribution = isset($contributions->$key) ? $contributions->$key : NULL;

        if(!is_null($contribution)) {
            $offline += $softcontribution->amount;
        }

        $amount =  $softcontribution->amount;
        if($convertNONUSD) {
            $amount = dw_campaigns_convert_to_usd($softcontribution->currency, $amount);
        }
        $total += $amount;
        $count++;
    }

    // save the total number of donations
    dw_campaigns_set_contribution_count_for_pcp($pcp, $count);

    return array('total' => $total, 'count' => $count, 'offline' => $offline);

}


function dw_campaigns_get_contribution_total_for_pcp($pcp, $convertNONUSD = FALSE) {
 
    static $pcp_totals = array();
    static $pending_contribution = NULL;

    if(!isset($pcp->id)) {
        return array('total' => 0, 'count' => 0, 'offline' => 0);
    }

    $cache_key = $pcp->id;
    if($convertNONUSD) {
        $cache_key .= "_CONVERT";   
    } else {
        $cache_key .= "_DONT";   
    }

    if(!isset($pcp_totals[$cache_key])) {

        $cache = cache_get($cache_key);

        if(!$cache) {
            $softcontributions  = dw_campaigns_get_contributions_for_pcp($pcp);
     
            $count          = 0;

            $total          = 0; // total is a combination of offline_total AND online_total
            $offline_total  = 0;
            $online_total   = 0;
    
            $offline_pending_count      = 0;
            $offline_already_imported   = 0;
    
            // unused, and sometimes we only have a pcp->id and not a full true PCP here
            // $page_id = $pcp->contribution_page_id;
    
    /*
    // this doesn't have a purpose in 2012
            if(is_null($pending_contribution)) { 
                // lookup regular contributions
                $params                             = array();
                $params['rowCount']                 = 999999;
                $params['contribution_status_id']   = 2; // pending only
                $contributions                      = _dw_civicrm_contribution_get($params);
        
                $pending_contribution               = $contributions;
    
            } else {
                $contributions                      = $pending_contribution;
            } 
    */ 
            foreach($softcontributions as $softcontribution) {
    
                $key            = '_' . $softcontribution->contribution_id;
                $contribution   = isset($contributions->$key) ? $contributions->$key : NULL;
     
                if(!is_null($contribution)) {
                    //$offline_total += $softcontribution->amount;
                }
     
                $amount     =  $softcontribution->amount;
    
                if($convertNONUSD) {
                    $amount = dw_campaigns_convert_to_usd($softcontribution->currency, $amount);
                }
    
                $online_total   += $amount;
                $total          += $amount;
                $count++;
            }
    
            $offline_donations  = dw_campaigns_get_offline_donations_for_pcp($pcp->id);
    
            foreach($offline_donations as $offline_donation) {
                $amount         = $offline_donation->donation_amount;
    
                if($convertNONUSD) {
                    $amount     = dw_campaigns_convert_to_usd($offline_donation->currency, $amount);
                }
    
                if($offline_donation->contribution_id != -1) {
                    $offline_already_imported   += $amount;
                    continue;
                }
    
                $offline_pending_count  += 1;
    
                $offline_total  += $amount;
                $total          += $amount;
                $count++;
            }
     
            $results                    = array('total' => $total, 'count' => $count, 'offline' => $offline_total, 'online' => $online_total, 'offline_already_imported' => $offline_already_imported, 'offline_pending_count' => $offline_pending_count);

            cache_set($cache_key, $results, 'cache', time() + (60*60));
        } else {
            $results = $cache->data;
        }

        $pcp_totals[$cache_key]     = $results;
 
    } else {
        $results            = $pcp_totals[$cache_key];
    }

    // save the total number of donations
    dw_campaigns_set_contribution_count_for_pcp($pcp, $results['count']);
 
    return $results;
}

/**
 * This function is named poorly, we can both get and set with it
 */
function dw_campaigns_set_contribution_count_for_pcp($pcp, $count = 0) {
    return dw_campaigns_get_contribution_count_for_pcp($pcp, $count);
}

function dw_campaigns_get_contribution_count_for_pcp($pcp, $count = NULL) {
    static $counts = array();
    if(!is_null($count)) {
        $counts[$pcp->id] = $count;
    }
    
    if(!isset($counts[$pcp->id])) {
        $res   = dw_campaigns_get_contribution_total_for_pcp($pcp);
        $total = $res['total'];
        $count = $res['count'];
        $counts[$pcp->id] = $count;
    }
    
    return $counts[$pcp->id];
}

function dw_campaigns_valid_type($campaign) {
    if($campaign->type == 'dw_campaigns_derby' || $campaign->type == 'dw_campaigns_walking') {
        return TRUE;
    }
    
    return FALSE;
}

/**
 * Looks up a users CiviCRM contact id
 */
function _dw_campaigns_user_get_contact_id($user, $refresh = false) {
    static $contacts = NULL;

    $uid = is_object($user) ? $user->uid : $user;
    
    if(is_null($contacts) || $refresh) {
        $contacts = array();

        $result    = db_query("SELECT drupal_id, civi_contact_id from {dw_campaigns_drupal_civi_contact_mapping}");
        foreach($result as $row) {
            if(!is_null($row)) {
                $contacts[$row->drupal_id] = $row->civi_contact_id;
            }
        }
    }
 
    if(isset($contacts[$uid])) {
        return $contacts[$uid];
    }

    return NULL;

}

/**
 * Looks up a CiviCRM contact's drupal id
 */
function _dw_campaigns_contact_id_get_user($contact_id) {
    static $contacts = NULL;
    
    if(is_null($contacts)) {
        $contacts = array();

        $result    = db_query("SELECT drupal_id,civi_contact_id from {dw_campaigns_drupal_civi_contact_mapping}");
        foreach($result as $row) {
            if(!is_null($row)) {
                $contacts[$row->civi_contact_id] = $row->drupal_id;
            }
        }
    }
 
    if(isset($contacts[$contact_id])) {
        return $contacts[$contact_id];
    }

    return NULL;
    
}

function _dw_campaigns_contact_id_get_user_old($contact_id) {
    static $contacts = array();
    
    if(!isset($contacts[$contact_id])) {
        // TODO - lookup contact id from user id

        $result     = db_query("SELECT drupal_id from {dw_campaigns_drupal_civi_contact_mapping} where civi_contact_id = :contactid", array(':contactid' => $contact_id));
        $thisUser   = $result->fetchObject();
        
        if(!$thisUser) {
            $contacts[$contact_id] = NULL;
        } else {
            $contacts[$contact_id] = $thisUser->drupal_id;
        }
    }
    
    return $contacts[$contact_id];
}

/**
 * Used for verifying a username is taken or not
 * 
 * @param boolean $echoResults
 * @return string 'found' | 'not-found'
 */
function dw_campaigns_username_search_ajax($username = NULL, $echoResults = TRUE) {
    
    global $user;

    $post_username  = !is_null($_POST['username']) ? $_POST['username'] : NULL;
    $username       = !is_null($username) ? $username : $post_username;
    
    //they are logged in and they entered their own username which we want to allow
    if($user->uid > 0 && $user->name == $username) {
        echo 'not-found';exit;
    }
        
    $result 	= db_query("SELECT * FROM {users} u WHERE name = :username", array(':username' => $username));
    $thisUser   = $result->fetchObject();

    // $thisUser will be false if no lines return;
    $results = ($thisUser) ? 'found' : 'not-found';

    if($echoResults) {
        
        echo $results;
        echo '|';
        echo dw_campaigns_make_url($username);

        exit;
    } 
    
    return $results;
}

function dw_campaigns_campaign_member_search_autocomplete($campaign_id, $username) {

	$data = dw_campaigns_campaign_search_pcp_search($campaign_id, $username);
	
	$return = array();

	foreach($data as $key => $value) {
		$return[$value->sort_name] = $value->display_name;
	}

	return drupal_json($return);
}

function dw_campaigns_ajax_position($pcp_id, $amount) {
    $pcp    = dw_campaigns_user_get_pcp_by_id($pcp_id);
    
    echo theme('dw_campaigns_ajax_position', array('pcp' => $pcp, 'possible' => $amount));
    die;
}

function dw_campaigns_ajax_currencyformat($amount = '0.00', $currency = 'USD') {
    echo dw_campaigns_force_decimal($amount, $currency);
    die;
}


function dw_campaigns_create_event_registration_for_campaign($node, $contribution_type_id = 4) {

    //check if we are updating, id will come back blank if nothing was found
    $event   = dw_campaigns_get_event_registration_for_campaign($node);

    if(isset($event->id) && !is_null($event->id) && !empty($event->id)) {
        $params['id']       = $event->id;        
    }
   
    $ts = strtotime($node->field_dw_date_range['und']['0']['value']);
    if($ts > mktime(0,0,0,1,1,date('Y'))) {
        $start_date = date("Y-m-d", $ts);
        $end_date   = date("Y-m-d", strtotime($node->field_dw_date_range['und']['0']['value2']));
    }
 
    $params['title']                            = $node->title;
    $params['event_type_id']                    = '3'; // this is hardcoded for FPWR 'Derby' style
    $params['description']                      = $node->body;
    $params['summary']                          = $node->body;
// TODO - evaluate which of these should be configurable, some of these settings are from 2010 FPWR annual conference
    $params['participant_listing_id']           = NULL; // ???
    $params['is_public']                        = 1;
    $params['start_date']                       = $start_date;
    $params['end_date']                         = $end_date;   
    $params['is_online_registration']           = 1;
    $params['registration_link_text']           = 'Register Now';
    $params['registration_start_date']          = '';
    $params['registration_end_date']            = '';
    $params['event_full_text']                  = 'This event is currently full.';
    // taken from FPWR 'running for FPWR and Ayden Jane'
    $params['is_monetary']                      = 1;
    $params['contribution_type_id']             = $contribution_type_id;
    $params['payment_processor_id']             = 1;
    $params['is_map']                           = 1;
    $params['is_active']                        = 1;
    $params['fee_label']                        = 'Event Fee(s)';
    
    $params['is_show_location']                 = 0; // this requires the creation of a 'location' which adds complexity here, for now if someone needs it they can just edit the event we insert
    $params['loc_block_id']                     = '';
    
    $params['default_role_id']                  = 1;
    $params['intro_text']                       = '';
    $params['footer_text']                      = '';
    $params['confirm_title']                    = 'Confirm Your Registration Information';
    $params['confirm_text']                     = 'Please confirm your registration information.';
    $params['confirm_footer_text']              = '';
    $params['is_email_confirm']                 = 1;
    $params['confirm_email_text']               = 'Thank you for your registration for the ' . $params['title'];
    $params['confirm_from_name']                = '';
    $params['confirm_from_email']               = '';
    $params['bcc_confirm']                      = '';
    $params['thankyou_title']                   = 'Thank you for registering';
    $params['thankyou_text']                    = 'Thank you for your registration for the ' . $params['title'];
    $params['thankyou_footer_text']             = '';
    $params['is_pay_later']                     = 0;
    $params['pay_later_text']                   = 'I will send payment by check';
    $params['pay_later_receipt']                = '';
    $params['is_multiple_registrations']        = 0; // TODO - maybe let this be set
    $params['allow_same_participant_emails']    = 0; // TODO maybe let this be set
    $params['has_waitlist']                     = 0;
    $params['requires_approval']                = NULL;
    $params['expiration_time']                  = NULL;
    $params['waitlist_text']                    = '';
    $params['approval_req_text']                = '';
    $params['is_template']                      = 0;
    $params['currency']                         = $node->field_dw_currency['und']['0']['value'];
    
    // address information
    $params['location_type_id']          = 1;
    $params['is_primary']                = 1;
    /*
    $params['street_address']            = $node->field_dw_address['und']['0']['street'];
    $params['city']                      = $node->field_dw_address['und']['0']['city'];
    $params['postal_code']               = $node->field_dw_address['und']['0']['postal_code'];
    */
// LOOK / NOTE - maybe we just shouldn't send them if they aren't defined...
    $params['street_address']            = isset($node->field_dw_address_street['und']['0']['value']) ? $node->field_dw_address_street['und']['0']['value'] : '';
    $params['city']                      = isset($node->field_dw_address_city['und']['0']['value']) ? $node->field_dw_address_city['und']['0']['value'] : '';
    $params['postal_code']               = isset($node->field_dw_address_postal_code['und']['0']['value']) ? $node->field_dw_address_postal_code['und']['0']['value'] : '';
    $params['supplemental_address_2']    = '';
    $params['postal_code_suffix']        = '';
    $params['geo_code_1']                = ''; 
    $params['geo_code_2']                = '';
    
    $event = _dw_civicrm_event_create($params);

    return $event;
}

function dw_campaigns_create_event_participant($user, $event = NULL, $otherParams) {

    if(is_null($event)) {
        if(!isset($otherParams['id'])) {
            drupal_set_message(t('you _MUST_ provide event_id to add a participant'), 'error');
            return;
        } else {
            $event_id   = $otherParams['id'];
            $event      = _dw_civicrm_event_get(array('id' => $event_id));
        }
    } else {
        $event_id = $event->id;
    }
    
    if(is_null($event_id)) {
        echo "failed to locate event - contact tech support";
        die;
    }
    //print_r($event);die;
    // I have no idea what I'm doing in here..
    /*
    [contact_id] => 1308
    [contact_type] => Individual
    [sort_name] => (removed)
    [display_name] => (removed)
    [event_id] => 8
    [event_title] => Go the Extra Mile for PWS
    [event_start_date] => 2010-05-15 08:30:00
    [event_end_date] => 2010-05-15 14:00:00
    [participant_id] => 6
    [participant_fee_level] => ,Race Registration,
    [participant_fee_amount] => 18.00
    [participant_fee_currency] => USD
    [event_type] => Fundraiser
    [participant_status_id] => 1
    [participant_status] => Registered
    [participant_role_id] => 1
    [participant_register_date] => 2010-03-30 11:04:42
    [participant_source] => Online Event Registration: Go the Extra Mile for PWS
    [participant_is_pay_later] => 0
    [participant_is_test] => 0
    */
    $contactId  = NULL;
    
    // this will add the current logged in user to the event specified
    if(isset($otherParams['addLoggedIn']) && $otherParams['addLoggedIn']) {
        $contactId      = _dw_campaigns_user_get_contact_id($user);
    } else {
        // I think we just add them as contacts and hope civi sorts it out?
        $params     = array();
        /*
        $params['first_name']       = _temp_rand_string(5); //$values['first_name'];
        $params['last_name']        = _temp_rand_string(8); //$values['last_name'];
        $params['email']            = _temp_rand_string(20); //$values['email'];
        */
        $params['first_name']       = 'liz_first'; //$values['first_name'];
        $params['last_name']        = 'liz_last'; //$values['last_name'];
        $params['email']            = 'liz_email'; //$values['email'];
        $params['contact_type']     = 'Individual';
        $params['returnFirst']      = 1;

        $contact_temp   = _dw_civicrm_contact_get($params);

        if(!is_null($contact_temp->contact_id)) {
            echo "did not insert";
            $contact        = $contact_temp;
        } else {
            unset($params['returnFirst']);
            echo "did insert";
            $contact        = _dw_civicrm_contact_create($params);

        }
        $contactId      = $contact->contact_id;
    }

    if(is_null($contactId)) {
        drupal_set_message(t('We were unable to create or find a contact record - so adding to an event is impossible'), 'error');
        return;
    }

    echo "Searching contact $contactId<br>";
    $searchParams               = array();
    $searchParams['contact_id'] = $contactId;
    $searchParams['event_id']   = $event_id;  
    $participant                = _dw_civicrm_participant_get($searchParams);
    
    if(is_null($participant->participant_id)) {
        drupal_set_message("user does not exist, adding", 'error');
        // this person does not exist yet
        if(!is_null($contactId)) {
            drupal_set_message("we have no contact id", 'error');
            $participant->contact_id                    = $contactId;
        }
        $participant->event_id                      = $event_id;
        $participant->participant_fee_level         = '';
        $participant->participant_fee_amount        = '';
        $participant->participant_fee_currency      = '';
        $participant->participant_status_id         = 1;
        $participant->participant_status            = 'Registered';
        $participant->participant_role_id           = 1;
        $participant->participant_register_date     = date('Y-m-d H:i:s');
        $participant->participant_source            = 'Online Event Registration: ' . $event->title;
        $participant->participant_is_pay_later      = 0;
        $participant->participant_is_test           = 0;
    } else {
        // I don't know how we're handling updating..
        drupal_set_message("This person is already a participant in this event", 'notice');
    }
    
    $params = get_object_vars($participant);
    
    $res = _dw_civicrm_participant_create($params);
    print_r($res);
    drupal_set_message("participant added", 'status');
    return $res->participant_id;
}

function _temp_rand_string($strlen = 6) {
    $accum = '';
    for ($i=0; $i<$strlen; $i++) { 
        $d=rand(1,30)%2; 
        $accum .= $d ? chr(rand(65,90)) : chr(rand(48,57)); 
    }
    return $accum;
}

function dw_campaigns_display_event_details($node) {
    $node = (is_object($node)) ? $node : node_load($node);
    
    
}

function _dw_campaigns_get_pcp_by_drupal_id($user) {
    static $pcp_list = array();

    // in some situations we don't have the actual campaign object, just its id
    $contact = _dw_civicrm_contact_get_from_user($user);
    if(!isset($contact->contact_id)) {
        return array();
    }
    
    if(!isset($pcp_list[$contact->contact_id])) {    

        $pcps   = _dw_campaigns_get_pcp_by_contact($contact->contact_id, FALSE);

        $pcp_list[$contact->contact_id] = $pcps;
    }
    
    return $pcp_list[$contact->contact_id];
}

function _dw_campaigns_get_all_pcps($reset = FALSE) {
    static $pcps = NULL;

    if($reset) {
        $pcps   = NULL;
    }

    if(is_null($pcps)) {

        $params                         = array();
        $params['rowCount']             = 999999;
        $params['pcpBlock']             = 0;
        $pcps                           = _dw_civicrm_pcp_get($params);
    
    }

    return $pcps;
}


function _dw_campaigns_get_pcps_for_campaign($campaign) {
    static $pcp_list = array();

    // in some situations we don't have the actual campaign object, just its id
    $nid        = (is_object($campaign)) ? $campaign->nid : $campaign;
    $campaign   = node_load($nid);

    if(!isset($pcp_list[$nid])) {    
        // dw_campaigns_get_contribution_page_for_campaign understands both campaigns and campaid_ids
        $contribution_page              = dw_campaigns_get_contribution_page_for_campaign($campaign);
        if(empty($contribution_page->id))  {
            return array();
        }


        $raw_pcps = _dw_campaigns_get_all_pcps();

        $pcps = new stdClass;

        foreach($raw_pcps as $pcp) {
            if($pcp->contribution_page_id == $contribution_page->id) {
                $id = $pcp->id;
                $pcps->$id = clone $pcp;
            }
        }

        $pcp_list[$nid]       = $pcps;
    }
    
    return $pcp_list[$nid];
}

function _dw_campaigns_get_pcps_for_campaign_old($campaign) {
    static $pcp_list = array();

    // in some situations we don't have the actual campaign object, just its id
    $nid = (is_object($campaign)) ? $campaign->nid : $campaign;

    if(!isset($pcp_list[$nid])) {    
        // dw_campaigns_get_contribution_page_for_campaign understands both campaigns and campaid_ids
        $contribution_page              = dw_campaigns_get_contribution_page_for_campaign($campaign);
        if(empty($contribution_page->id))  {
            return array();
        }

        $params = array();
        $params['rowCount']             = 999999;
        $params['pcpBlock']             = 0;
        $params['contribution_page_id'] = $contribution_page->id;
        $pcps                           = _dw_civicrm_pcp_get($params);

        $pcp_list[$nid]       = $pcps;
    }
    
    return $pcp_list[$nid];
}

function _dw_campaigns_campaign_total($campaign) {
    static $campaign_total         = array();  

    $campaign   = is_object($campaign) ? $campaign : node_load($campaign);
    $nid        = $campaign->nid;

    if(!isset($campaign_total[$nid])) {
     
        $contribution_page              = dw_campaigns_get_contribution_page_for_campaign($campaign);
    
        $goal                           = $contribution_page->goal_amount;
        $total_raised                   = 0;
        $offline_total                  = 0;
        $online_total                   = 0;
        $pcp_count                      = 0;
        $pcp_withd_count                = 0;
        $donation_count                 = 0;
        $sponsor                        = 0;
      
        $pcps                           = _dw_campaigns_get_pcps_for_campaign($campaign);

        $currency                       = $campaign->field_dw_currency['und']['0']['value'] ;
        $convert                        = ($currency == 'MULTI') ? TRUE : FALSE;
        
        foreach($pcps as $pcp) {
            $res                        = dw_campaigns_get_contribution_total_for_pcp($pcp, $convert);
            extract($res);

            $total_raised               += $total;
            $offline_total              += $offline;
            $online_total               += $online;

            if(strstr($pcp->title, "General Donation")) {
        
            } else {
                $pcp_count              += 1;
                if($total > 0) {
                    $pcp_withd_count    += 1;
                }
            }

            $donation_count             += $count; 
        }

        if(isset($campaign->field_dw_other_donations['und']['0']['value'])) {
            $sponsor       = $campaign->field_dw_other_donations['und']['0']['value'];
            $total_raised += $sponsor;
        }
    
        $campaign_total[$nid] = array('goal' => $goal, 'raised' => $total_raised, 'pcp_count' => $pcp_count, 'sponsor' => $sponsor, 'online' => $online_total, 'offline' => $offline_total, 'count' => $donation_count, 'pcp_withd_count' => $pcp_withd_count);
    
    } 

    return $campaign_total[$nid];
}

function  _dw_campaigns_campaigns_total() {
    $goal             = 0;
    $total_campaigns  = 0;
    $total_raised     = 0;
    $total_usd        = 0;
    $total_pcps       = 0;
    $total_pcps_withd = 0;
  
    $cache = cache_get('dw_campaigns_campaigns_total');
    if(!$cache) {
    $campaigns  = dw_campaigns_get_active_campaigns();
    if(count($campaigns) > 0 ) {
        foreach($campaigns as $campaign) {
            $res            = _dw_campaigns_campaign_total($campaign);
            $amount         = $res['raised'];
            $goal           += $res['goal'];
            $total_raised   += $amount;
            $total_usd      += dw_campaigns_convert_to_usd($campaign->field_dw_currency['und']['0']['value'], $amount);
            $total_campaigns +=1;
            $total_pcps     += $res['pcp_count'];

            $total_pcps_withd   += $res['pcp_withd_count'];
        }
    }
    
    $res = array('goal' => $goal, 'raised' => $total_raised, 'raised_usd' => $total_usd, 'total_campaigns' => $total_campaigns, 'total_pcps' => $total_pcps, 'total_pcps_withd' => $total_pcps_withd);
    cache_set('dw_campaigns_campaigns_total', $res, 'cache', time() + 3600);
    } else {
        $res = $cache->data;
    }

    return $res;
}


function  _dw_campaigns_campaigns_campaign_total() {
    $totals         = array();
    $usd_totals     = array();
    $pcp_counts     = array();

    $campaigns      = dw_campaigns_get_active_campaigns();
    $campaigns_l    = array(); 
   
    if(count($campaigns) > 0) {
        foreach($campaigns as $campaign) {
            $nid                = $campaign->nid;
    
            $res                = _dw_campaigns_campaign_total($campaign);
    
            $total              = $res['raised'];
            $totals[$nid]       = $total;

            $usd_total          = dw_campaigns_convert_to_usd($campaign->field_dw_currency['und']['0']['value'], $res['raised']);
            $usd_totals[$nid]   = $usd_total;
    
            $pcp_count          = $res['pcp_count'];
            $pcp_counts[$nid]   = $pcp_count;
    
            $campaigns_l[$nid]  = $campaign;
        }
    }
    arsort($totals, SORT_NUMERIC);
    arsort($usd_totals, SORT_NUMERIC);
    
    return array('totals' => $totals, 'usd_totals' => $usd_totals, 'campaigns' => $campaigns_l, 'pcp_counts' => $pcp_counts);
}


/**
 * Returns a list of all pcps for a specific campaign ordered by total
 */
function _dw_campaigns_campaign_leaders_list($campaign) {
    static $leaders = array();

    if(!isset($campaign->nid)) {
        drupal_set_message(t('We can not generate a list of campaign leaders if no campaign is specified'), 'error');
        return $leaders;
    }

    if(!isset($leaders[$campaign->nid])) {
        
        $pcps                           = _dw_campaigns_get_pcps_for_campaign($campaign);
        $totals                         = array();
        $counts                         = array();

        $currency                       = $campaign->field_dw_currency['und']['0']['value'] ;
        $convert                        = ($currency == 'MULTI') ? TRUE : FALSE;
        
        foreach($pcps as $pcp) {

            $res                        = dw_campaigns_get_contribution_total_for_pcp($pcp, $convert);

            if(isset($totals[$pcp->contact_id])) {
               $mode_type  = dw_campaigns_get_selected_type();
                // mail('', 'Dupe PCP: ' . $mode_type, "{$pcp->id} - {$pcp->contact_id}<br>" . serialize($res));
            } else  {
                $totals[$pcp->contact_id]   = $res['total'];
                $counts[$pcp->contact_id]   = $res['count'];
            }
        }
        
        arsort($totals, SORT_NUMERIC);
        arsort($counts, SORT_NUMERIC);
        
        $leaders[$campaign->nid]    = array('totals' => $totals, 'counts' => $counts);
    }
    return $leaders[$campaign->nid];
}


function _dw_campaigns_campaign_leaders_list_fake($campaign, $pcp_id, $amount) {

    $currency                       = $campaign->field_dw_currency['und']['0']['value'] ;
    $convert                        = ($currency == 'MULTI') ? TRUE : FALSE;

    $pcps                               = _dw_campaigns_get_pcps_for_campaign($campaign);
    
    foreach($pcps as $pcp) {
        $res                            = dw_campaigns_get_contribution_total_for_pcp($pcp, $convert);

        if($pcp_id == $pcp->id) {
            $totals[$pcp->contact_id]   = $amount;
            $counts[$pcp->contact_id]   = $res['count']+1;
        } else {
            $totals[$pcp->contact_id]   = $res['total'];
            $counts[$pcp->contact_id]   = $res['count'];
        }
    }
    
    arsort($totals, SORT_NUMERIC);
    arsort($counts, SORT_NUMERIC);
    
    return array('totals' => $totals, 'counts' => $counts);
}


/**
 * returns 'top X' leaders for a given campaign
 * returns name (displayname), drupal id, address to users page, total amount of money raised, and total number of donations
 */
function _dw_campaigns_campaign_leaders($campaign = NULL, $count = 3) {
    $rows   = array();

    $res    = _dw_campaigns_campaign_leaders_list($campaign);
    extract($res);

    $campaign_location  = isset($campaign->field_dw_campaign_location['und']['0']['value']) ? $campaign->field_dw_campaign_location['und']['0']['value'] : '<!-- no location -->';

    $image_match    = '';
    $image_params   = array(
        'w'                 => 45,
        'contribution'      => true,
    );

    foreach($totals as $contact_id => $total)
    {
        if($count==0)
                break;

        if(dw_campaigns_hide_contact($contact_id)) {
            continue; 
        }

        $count--;
        
        $drupal_id      = _dw_campaigns_contact_id_get_user($contact_id);
echo "<!-- HI: $contact_id => $drupal_id -->\n";
        //$fake_user      = user_load(array('uid'=>$drupal_id));
        $fake_user      = dw_campaigns_cache_simple_user_data($drupal_id);
        $url            = dw_campaigns_user_get_pcp_url($fake_user, $campaign);
        $image          = _dw_campaigns_get_photo($fake_user, $image_params, 'user-photo', NULL, $image_match);

        $donations      = $counts[$contact_id];
        
        $params = array(
            'contact_id'    => $contact_id,
            'returnFirst'   => 1
        );

        $contact        = _dw_civicrm_contact_get($params);
if(!isset($fake_user->displayname)) {
//var_dump($fake_user);
}
        $displayname    = isset($fake_user->displayname) ? $fake_user->displayname : '(temporarily unavail)'; 
 
        $rows[]		    = array('name' => $displayname, 'drupal_id' => $drupal_id, 'image' => $image, 'url' => $url, 'total' => $total, 'donations' => $donations, 'contact' => $contact, 'campaign_location' => $campaign_location, 'campaign_id' => $campaign->nid);
    }

    return $rows;
}

function _dw_campaigns_all_leaders($count = 3, $noImage = FALSE, $allCampaigns = FALSE) {

    $row            = array();

    if($allCampaigns) {
        if($allCampaigns == 'PREVIOUS_YEARS') {
            $campaignList   = dw_campaigns_get_inactive_campaign_ids();
        } else {
            $campaignList   = dw_campaigns_get_all_campaign_ids();
        }
    } else {
        $campaignList   = dw_campaigns_get_campaign_ids();
    }

    $res            = _dw_campaigns_all_leaders_list($campaignList);

    if(is_null($res)) {
        return NULL;
    }

    extract($res);

    $image_match    = '';
    $image_params   = array(
        'w'                 => 45,
        'contribution'      => true,
    );

    $campaigns = array();

    if(count($totals) <= 0 || $totals == 0) {
        return NULL;
    }

    

    foreach($totals as $idx => $total)
    {
        list($campaign_id, $contact_id)     = preg_split('/-/', $idx);

        if(dw_campaigns_hide_contact($contact_id)) {
            continue; 
        }
        
        // load the campaign if we havn't seen it before
        if(!isset($campaigns[$campaign_id])) {
            $campaigns[$campaign_id]        = node_load($campaign_id);
        }
        
        $campaign                           = $campaigns[$campaign_id];

        if($count==0)
            break;

        $count--;
        
        $drupal_id      = _dw_campaigns_contact_id_get_user($contact_id);
echo "<!-- HI: $contact_id => $drupal_id -->\n";

        $fake_user      = dw_campaigns_cache_simple_user_data($drupal_id);

        //$fake_user      = user_load(array('uid'=>$drupal_id));
        $url            = dw_campaigns_user_get_pcp_url($fake_user, $campaign);

        if(!$noImage) {
            $image      = _dw_campaigns_get_photo($fake_user, $image_params, 'user-photo', NULL, $image_match);
        } else {
            $image      = NULL;
        }

        $donations      = $counts[$idx];
        
        $params = array(
            'contact_id'    => $contact_id,
            'returnFirst'   => 1
        );
        $contact        = _dw_civicrm_contact_get($params);
        
        $campaign_location  = $campaign->field_dw_campaign_location['und']['0']['value'];
        
        $rows[]		= array('name' => $fake_user->displayname, 'drupal_id' => $drupal_id, 'image' => $image, 'url' => $url, 'total' => $total, 'donations' => $donations, 'contact' => $contact, 'campaign_location' => $campaign_location, 'campaign_id' => $campaign_id);
    }

    return $rows;

}

// LOOK - this doesn't seem to do what we want it to, leaders should probably be an array?
function _dw_campaigns_all_leaders_list($campaignList) {
    static $leaders = NULL;
    
    if(is_null($leaders)) {
        if(count($campaignList) > 0) {

            $totals = array();   
            $counts = array();   
            $in_usd = array();   

            foreach($campaignList as $campaign_id) {
                $campaign = node_load($campaign_id);
                if(!$campaign) {
                    continue;
                }
                $currency                       = $campaign->field_dw_currency['und']['0']['value'] ;
                $convert                        = ($currency == 'MULTI') ? TRUE : FALSE;

                $pcps   = _dw_campaigns_get_pcps_for_campaign($campaign_id);
 
                foreach($pcps as $pcp) {
                    $idx    = $campaign_id . '-' . $pcp->contact_id;
    
 
                    $res            = dw_campaigns_get_contribution_total_for_pcp($pcp, $convert);
                    $totals[$idx]   = $res['total'];
                    $counts[$idx]   = $res['count'];
                    $in_usd[$idx]   = dw_campaigns_convert_to_usd($currency, $res['total']);

                }
                
            }

            arsort($in_usd, SORT_NUMERIC);
            arsort($counts, SORT_NUMERIC);
            arsort($totals, SORT_NUMERIC);

            $totals_sorted = array();
            foreach($in_usd as $idx => $junk) {
                $totals_sorted[$idx] = $totals[$idx]; 
            } 
                
            $leaders                        = array('totals' => $totals, 'counts' => $counts);
            $leaders                        = array('totals' => $totals_sorted, 'counts' => $counts);
        } else {
            $leaders                            = array('totals' => 0, 'counts' => 0);
        }
    }
    return $leaders;
}



/**
 * supply a campaign and a user and it will tell you which position they are ranked based on total donations to PCP
 */
function _dw_campaigns_campaign_find_position_of_user($campaign = NULL, $user) {

    $res                    = _dw_campaigns_campaign_leaders_list($campaign);
    extract($res);

    $our_contact_id         = _dw_campaigns_user_get_contact_id($user);
    
    $high                   = 0;
    $pos                    = 1;
    foreach($totals as $contact_id => $total)
    {
        if($total > $high) {
            $high           = $total;
        }

        if($contact_id == $our_contact_id) {
            break;
        }
        
        $pos++;
    }

    if($high == 0){
        $pos = 1;
    }
    
    return array('position' => $pos, 'total' => $total);
}


/**
 * looks up and returns the
 */
function _dw_campaigns_campaign_get_position($campaign, $pos) {
    $res                    = _dw_campaigns_campaign_leaders_list($campaign);
    extract($res);
    
    foreach($totals as $contact_id => $total) {
    
        $pos--;
        
        if($pos == 0) {
            if(dw_campaigns_hide_contact($contact_id)) {
                continue;
            }

            $drupal_id      = _dw_campaigns_contact_id_get_user($contact_id);
            //$fake_user      = user_load(array('uid'=>$drupal_id));
            $fake_user      = dw_campaigns_cache_simple_user_data($drupal_id);
            $url            = dw_campaigns_user_get_pcp_url($fake_user, $campaign);
            
            $donations      = $counts[$contact_id];
            
            $params = array(
                'contact_id'    => $contact_id,
                'returnFirst'   => 1
            );
            
            $contact        = _dw_civicrm_contact_get($params);
            $row = array('name' => $fake_user->displayname, 'drupal_id' => $drupal_id, 'contact_id' => $contact_id, 'url' => $url, 'total' => $total);
            return $row;
        }
    }
    
}

function position_to_name($pos) {
    if($pos == 0) {
        return 'N/A';
    }
    
    $str = "$pos";
    
    $t = $pos > 9 ? substr($str,-2,1) : 0;
    $u = substr($str,-1);

    if ($t==1) {
        return $str . 'th';
    } else {
        switch ($u) {
            case 1:
                return $str . 'st';
            case 2:
                return $str . 'nd';
            case 3:
                return $str . 'rd';
            default:
                return $str . 'th';
        }
    }
}

function _dw_campaigns_campaign_find_position_of_amount($campaign = NULL, $amount) {

    $res                    = _dw_campaigns_campaign_leaders_list($campaign);
    extract($res);
    
    $high                   = 0;
    $pos                    = 1;
    foreach($totals as $contact_id => $total)
    {
        if($total > $high) {
            $high           = $total;
        }
        if($total <= $amount) {
            break;
       	} 
        $pos++;
    }

    if($high == 0){
        $pos = 1;
    }
    
    return array('position' => $pos, 'total' => $amount);
}


function dw_campaigns_resolve_contact($user) {
    
    // defaults
    $contact = array();
    $contact['email']                   = NULL;
    $contact['phone']                   = NULL;
    $contact['postal_code']             = NULL;
    $contact['state_province_id']       = NULL;
    $contact['country_id']              = NULL;
    $contact['city']                    = NULL;
    $contact['supplemental_address_1']  = NULL;
    $contact['street_address']          = NULL;
    $contact['last_name']               = NULL;
    $contact['first_name']              = NULL;
    
    //load the users civicrm contact if they are logged in to populate default values in form
    if($user->uid > 0) {
        
        if(arg(2) == 'register') {
            drupal_goto('dw');
            return;
        }
            
        $contact = _dw_civicrm_contact_get_from_user($user);
        $contact = get_object_vars($contact); // convert stdObj to an array
    }
    
    return $contact;
}

/**
 * Gets the landing page for a users pcp - personal campaign page
 */
function dw_campaigns_user_get_pcp_url($forUser = NULL, $campaign = NULL, $includeFqd = FALSE) {

	if(is_null($forUser)) {
	    global $user;
	    $forUser = $user;
	}

        if($forUser->uid == 0) {
            return FALSE;
        }	

        $name = is_object($forUser) && isset($forUser->name) ? $forUser->name : $forUser; // assume its just a string for name


	if(is_null($campaign)) {
	    $return_data = "/dw/users/" . $name;
	} else {

            $path = drupal_lookup_path('alias',"node/".$campaign->nid) ;

            $path_alias = str_replace('content/', '', $path);

	    $return_data = "/dw/users/" . $name . "/" . $path_alias;
	}

	$return_data = preg_replace("_//*_", '/', $return_data);

	if($includeFqd) {
            $cur_host    = $_SERVER['HTTP_HOST'];
	    $return_data = "http://" . $cur_host . $return_data;
	}

	return $return_data;
}
/**
 * takes '2011-kentucky-derby' and converts to node id (4)
 */
function dw_campaigns_campaign_get_id_from_url($campaignPath) {
    $campaignId         = NULL;

    $count = preg_match('/^node-(\d*)$/', $campaignPath, $matches);

    if($count > 0) {
        if(isset($matches[1])) {
            return $matches[1];
        }
    }

    // I know some people don't use a prefix (fpwr did not) - or they may use more complicated paths
    $path               = drupal_lookup_path('source', $campaignPath, 'en');
   
    if(!$path) {
        // we use content/whatever as our prefix
        $path           = drupal_lookup_path('source', 'content/' . $campaignPath, 'en');
    }
    
    if($path) {
        list($junk, $campaignId)  = explode('/', $path);
    }

    return $campaignId;

}

/**
 * takes '4' and returns $prefix/2011-kentucky-derby
 */
function dw_campaigns_get_campaign_path($campaignId, $prefix = '/dw/campaign', $suffix = '') {
    $path_alias = drupal_get_path_alias("node/" . $campaignId);

    // this now makes us have a problem if a place is named node- - that's okay for now
    $path_alias = str_replace('node/', 'node-', $path_alias);

    // remove the content/ prefix
    $path_alias = str_replace('content/', '', $path_alias);
    $path = $prefix . '/' . $path_alias . '/' . $suffix;
    // remove the dupes
    $path = preg_replace('/\/\/*/', '/', $path);
    // remove a trailing /
    $path = preg_replace('/\/$/', '', $path);
    return $path;
}

/**
 * pass either a campaign id (nid) or a campaign object
 */

function dw_campaigns_get_donate_url($user, $campaign) {

    $campaign = (is_object($campaign)) ? $campaign : node_load($campaign);
    
    $prefix = 'dw/users/' . $user->name;
    
    return dw_campaigns_get_campaign_path($campaign->nid, $prefix, 'donate');
}


function dw_campaigns_campaign_search_pcp_search($campaign, $in_query) {

    $matches            = array();

    $params             = array();
    $params['search']   = $in_query;

    $civi_contacts      = _dw_civicrm_pcp_search_owner($params);

    if($campaign === '0') {
        $pcps           =  _dw_campaigns_get_all_pcps_keyed_by_contact_ids();
    } else {
        $pcps           = _dw_campaigns_get_pcps_for_campaign_keyed_by_contact_ids($campaign);
    }

    foreach($civi_contacts as $contact_id => $civi_contact)
    {
        if(isset($pcps[$contact_id]))
        {
            $pcp                        = $pcps[$contact_id];
            $_userId                    = _dw_campaigns_contact_id_get_user($contact_id);
            $_user                      = user_load($_userId);
            $civi_contact->user         = $_user;
            $civi_contact->pcp_title    = $pcp->title; 
            $matches[$contact_id]       = $civi_contact;
        }
    }

    return $matches;
}


function dw_campaigns_search_form($form, &$form_state, $in_query = NULL) {

    $form = array();
    $mode           = dw_campaigns_get_selected_type();
    if($mode == 'walking') {
        $form['markup']      = array(
            '#type'             => 'markup',
            '#markup'           => '<div class="search-header"><h2>Search for a walker</h2><div class="word">' . t('Enter at least two characters of the first or last name in the search box to find a fundraising page.') . '</div></div>'
        );

        $form['query']       = array(
            '#type'             => 'textfield',
            '#title'            => t('Walkers Name'),
            '#required'         => true,
            '#default_value'    => t('Enter Walker\'s Name'),
            '#prefix'           => '<div class="search-box-form">',
            '#attributes'       => array(
                'class'         => array('text', 'has-default-text')
            )
        );

    } else {
        $form['markup']      = array(
            '#type'             => 'markup',
            '#markup'           => '<div class="search-header"><h2>Search for a fundraiser</h2><div class="word">' . t('Enter at least two characters of the first or last name in the search box above to find a fundraising page.') . '</div></div>'
        );

        $form['query']       = array(
            '#type'             => 'textfield',
            '#title'            => 'Fundraisers Name',
            '#required'         => true,
            '#default_value'    => 'Enter Fundraisers\'s Name',
            '#prefix'           => '<div class="search-box-form">',
            '#attributes'       => array(
                'class'         => array('text', 'has-default-text')
            )
        );

        if(!is_null($in_query)) {
            $offset = array_search('has-default-text', $form['query']['#attributes']['class']);
            unset($form['query']['#attributes']['class'][$offset]);
            $form['query']['#default_value']    = $in_query;
        }

    }



    $form['submit'] = array(
        '#type'             => 'submit',
        '#suffix'           => '</div>',
        '#value'            => t('Search'),
        '#weight'           => 2,
    );

    return $form;

}

function dw_campaigns_search_form_submit(&$form, &$form_state) {
    if(isset($form_state['values']['query'])) { 
        $_SESSION['last_query'] = $form_state['values']['query'];
    }
}

function dw_campaigns_campaign_member_search($campaignPath, $in_query = NULL) {

    if(is_null($in_query)) {
        if(isset($_SESSION['last_query'])) {
            $_REQUEST['query'] = $_SESSION['last_query'];
            unset($_SESSION['last_query']);
        }
        $in_query	=	isset($_REQUEST['query'])?$_REQUEST['query']:'';
    }

    if($in_query === 'Name' || $in_query == 'search for a fundraiser')
    {
    	$in_query = '';
    }

    dw_campaigns_set_last_search($in_query);

    $form           = drupal_get_form('dw_campaigns_search_form', $in_query);
    $return         = drupal_render($form);

  
    $mode           = dw_campaigns_get_selected_type();

    if($campaignPath == 'current')  {
         $campaign = '0';
    } else {   
        $campaignId     = dw_campaigns_campaign_get_id_from_url($campaignPath);
        $campaign       = node_load($campaignId);
    
        dw_campaigns_set_selected_campaign($campaign);
    
        if($campaign->nid == 0) {
    	    drupal_goto("dw");
    	    return;
        }
    }



    if(empty($in_query)) {
        return $return;
    } else {
 
        $return         .= '<h2 class="search-results">Search Results</h2>';

        if(strlen($in_query)<2) {
            return $return . 'Minimum search length is 2 characters';        
        }
    
        $results = dw_campaigns_campaign_search_pcp_search($campaign, $in_query);
    
        if($mode == 'walking') {
            $headers    = array('','','', '');
        } else {
            $headers    = array('','','');
        }
        $rows       = array();
        
        foreach($results as $result) {
            $prefix = '/dw/users/' . $result->user->name;
            $dummy  = '';
    
            // this means its for a specific campaign
            if($campaignPath != 'current') {
                $path   = dw_campaigns_get_campaign_path($campaign->nid, $prefix, '');
    
                $row    = array();
                $row[]  = '<img src="' . _dw_campaigns_get_photo($result->user, array('w' => 50), 'user-photo', NULL, $dummy) . '" />';
                $row[]  = $result->display_name . ' (' . $result->pcp_title . ')';
                if($mode == 'walking') {
                    $row[]  = $campaign->field_dw_campaign_location['und']['0']['value'];
                }
                $row[]  = '<a href="' . $path . '" class="btn">' . t('Fundraising Page') . '</a>';
                $rows[] = $row;
    
            } else {
                $pcp_data  = _dw_campaigns_get_pcp_by_drupal_id($result->user);
                $campaigns = dw_campaigns_get_active_campaigns(true); // these come back indexed by contribution page
    
                foreach($pcp_data as $pcp_id => $pcp) {
    
                    $contribution_page_id = $pcp->contribution_page_id;
    
    		if(isset($campaigns[$contribution_page_id])) {
                        $campaign = $campaigns[$contribution_page_id];
                    } else {
                        continue;
                    }
                    
                    $path   = dw_campaigns_get_campaign_path($campaign->nid, $prefix, '');
        
                    $row    = array();
                    $row[]  = '<img src="' . _dw_campaigns_get_photo($result->user, array('w' => 50), 'user-photo', NULL, $dummy) . '" />';
                    $row[]  = $result->display_name . ' (' . $result->pcp_title . ')';
    
                    if($mode == 'walking') {
                        $row[]  = $campaign->field_dw_campaign_location['und']['0']['value'];
                    }
    
                    $row[]  = '<a href="' . $path . '" class="btn">' . t('Fundraising Page') . '</a>';
                    $rows[] = $row;
    
                }
            }
        }
    
        if(count($rows) == 0) {
            return $return . 'No matches found for (' . htmlentities($in_query) . ')';   
        }
         
        return $return . theme('table', array('headers' => $headers, 'rows' => $rows));
    }
}


function _dw_campaigns_get_pcps_for_campaign_keyed_by_contact_ids($campaign) {

    $contacts = array();
    
    $res = _dw_campaigns_get_pcps_for_campaign($campaign);

    foreach($res as $pcp) {
        $contacts[$pcp->contact_id] = $pcp;
    }
    
    return $contacts;
}

function _dw_campaigns_get_all_pcps_keyed_by_contact_ids() {

    $contacts = array();
   
    $campaigns = dw_campaigns_get_active_campaigns(); 
    if(count($campaigns) > 0) {
        foreach($campaigns as $campaign) {
            $res = _dw_campaigns_get_pcps_for_campaign($campaign);
        
            foreach($res as $pcp) {
                $contacts[$pcp->contact_id] = $pcp;
            }
        } 
    }
    return $contacts;
}


function dw_campaigns_pcp_get_supporters($pcp) {

    $res    = dw_campaigns_get_contributions_for_pcp($pcp);
    

    $offline_donations  = dw_campaigns_get_offline_donations_for_pcp($pcp->id);


    foreach($offline_donations as $offline_donation) {
        if($offline_donation->contribution_id == -1) {
            $res[] = $offline_donation;
        }
    }

    return $res;
}

function dw_campaigns_get_default_campaign() {
    return variable_get('dw_campaigns_default_campaign', '4');
}

function dw_campaigns_force_decimal($in, $currency = 'USD', $always_show_usd = false) {
    $prefix     = '$';
    $extra      = '';

    $decimal    = '.';
    $thousands  = ',';

    switch($currency) {
        case 'AUD':
            $prefix     = 'AU$';
            break;
        case 'CAD':
            $prefix     = 'CA$';
            break;
        case 'EUR':
            $prefix     = 'EUR ';
            $decimal    = ',';
            $thousands  = '.';
            break;
        case 'GBP':
            $prefix     = 'GBP ';
            break;
        case 'NZD':
            $prefix     = 'NZ$ ';
            break;

    }

    if($always_show_usd) {
	$in_usd = dw_campaigns_convert_to_usd($currency, $in);

        $usd     = dw_campaigns_force_decimal($in_usd, 'USD');
        $extra   = " ($usd) ";
    }

    $fmt         = sprintf("%s%s%s", $prefix, number_format($in, 2, $decimal, $thousands), $extra);

    return $fmt;
}

//dw_campaigns_admin_grab_exchange_rate_data();
//exit;
//dw_campaigns_convert_to_usd("AUD", 10.00);

function dw_campaigns_convert_to_usd($currency, $in) {

    $exchangeRates = variable_get("exchangeRates");

    if(is_null($exchangeRates)) {
        drupal_set_message(t('You must set the exchange rates!'));
        return $in;
    }

    $rate = 1/$exchangeRates->{ $currency };

    if( !$rate ){
        //@todo we should write a log, and redirect to a fail page.
    }

    $converted = $in * $rate;

    return $converted;

    //JFN - january 24 2014 0746 - [#todo "below is the old static code for currency exchange. above is the new currency exchange stuff. remove the old stuff in february 2014"]
    // prices according to xe.com  Live rates at 2011.04.01 18:47:00 UTC
    switch($currency) {
        case 'AUD':
            //$in = $in * 1.03861;
            $in = $in * 1.02741;
            break;
        case 'CAD':
            //$in = $in * 1.03643;
            $in = $in * 0.99102;
            break;
        case 'EUR':
            //$in = $in * 1.42386;
            $in = $in * 1.34066;
            break;
        case 'GBP':
            //$in = $in * 1.61264;
            $in = $in * 1.56030;
            break;
        case 'NZD':
            //$in = $in * 0.8554;
            //$in = $in * 0.78065;
            $in = $in * 0.85127;
            break;

    }

    return $in;
}

function dw_campaigns_convert_from_usd($currency, $in) {

    $exchangeRates = variable_get("exchangeRates");

    $rate = $exchangeRates->{ $currency };

    if( !$rate ){
        //@todo should redirect to error page, and log/email error to admins
    }

    $converted = $in * $rate;

    return $converted;
    //JFN - january 24 2014 0758 - [#todo "below is the old static code for currency exchange. above is the new currency exchange stuff. remove the old stuff in february 2014"]

    switch($currency) {
        case 'AUD':
            $in = $in * 0.97322;
            break;
        case 'CAD':
            $in = $in * 1.00906;
            break;
        case 'EUR':
            $in = $in * 0.74590;
            break;
        case 'GBP':
            $in = $in * 0.64090;
            break;
        case 'NZD':
            $in = $in * 1.17472; // paypal converion sept 29th
            break;

    }

    return $in;
}

function dw_campaigns_force_k($in) {
    if($in < 1000) {
        return dw_campaigns_force_decimal($in);
    }

    $k  = $in/1000;

    return floor($k) . 'K';
}

function dw_campaigns_build_location_addr($campaign) {
    $address['street'] = isset($campaign->field_dw_address_street['und']['0']['value']) ? $campaign->field_dw_address_street['und']['0']['value'] : '';
    $address['city'] = isset($campaign->field_dw_address_city['und']['0']['value']) ? $campaign->field_dw_address_city['und']['0']['value'] : '';
    $address['province'] = isset($campaign->field_dw_address_province['und']['0']['value']) ? $campaign->field_dw_address_province['und']['0']['value'] : '';
    $address['postal_code'] = isset($campaign->field_dw_address_postal_code['und']['0']['value']) ? $campaign->field_dw_address_postal_code['und']['0']['value'] : '';

    $res = $address['street'] . ' ' . $address['city'] . ', ' . $address['province'] . ' ' . $address['postal_code'];
    return $res;
}

function dw_campaigns_build_location_addr_old($address) {
    $res = $address['street'] . ' ' . $address['city'] . ', ' . $address['province'] . ' ' . $address['postal_code'];
    return $res;
}

// completely taken from Taylor
function dw_campaigns_get_location_for_address($address) {
	$cacheKey	= "geo-" . md5($address);
	$location 	= cache_get($cacheKey);

	if(!is_object($location)) {
		
		$results 	= json_decode(file_get_contents("http://maps.google.com/maps/api/geocode/json?address=" . urlencode($address) . "&sensor=false"));
		$location 	= isset($results->results[0]->geometry->location) ? (array) $results->results[0]->geometry->location : NULL;
		
		cache_set($cacheKey, $location);
		
	} else {
		$location = $location->data;
	}
	
	return $location;
}


function dw_campaigns_get_visitor_location() {
    
    //check cache
    $ip     = $_SERVER["REMOTE_ADDR"];
    $tags   = cache_get('geo-for-' . $ip);
          
    if(!$tags) {
        $tags   = get_meta_tags('http://www.geobytes.com/IpLocator.htm?GetLocation&template=php3.txt&IpAddress=' . $ip);
    	cache_set('geo-for-' . $ip, $tags);
    } else {
    	$tags	= $tags->data;
    }
    
    //the ip location service returns a string of false if it's not found
    if($tags['known'] == 'false') {
    	//if not found, use center of unitded states
    	$tags['known']      = false;
    	
    	$results    = dw_campaigns_get_location_for_address("United States");
    	
    	$tags['latitude']   = $results->lat;
    	$tags['longitude']  = $results->lng;
    	
    } else {
    	$tags['known']      = true;
    }
    
    return $tags;
}


  
//function calculate_distance  (seen multiple places with google search)
// I copied this from http://www.sudhirhub.com/2010/05/find-distance-between-two-cities-using.html
function calculate_distance($lat1, $long1, $lat2, $long2, $unit = 'M') {

    $theta = $long1 - $long2;  
    $dist = sin(deg2rad($lat1)) * sin(deg2rad($lat2)) +  cos(deg2rad($lat1)) * cos(deg2rad($lat2)) * cos(deg2rad($theta));  
    $dist = acos($dist);  
    $dist = rad2deg($dist);  
    $miles = $dist * 60 * 1.1515;  
    $unit = strtoupper($unit);  
    if ($unit == "K") {  
        return ($miles * 1.609344);  
    }  
    else if ($unit == "N") {  
         return ($miles * 0.8684);  
    }  
    else {
        return $miles;  
    }  
}

function dw_campaigns_get_close_campaigns($in_query = NULL) {

    if(is_null($in_query)) {
        $in_query	=	isset($_REQUEST['query'])?$_REQUEST['query']:'';
    }

    if(empty($in_query)) {
        $userGeo    = dw_campaigns_get_visitor_location();
    } else {
    	$results    = dw_campaigns_get_location_for_address($in_query);

    	$userGeo['latitude']   = $results['lat'];
    	$userGeo['longitude']  = $results['lng'];
    }

    $unit = variable_get('dw_campaigns_walking_distance_unit', 'M');
    $campaigns = dw_campaigns_get_active_campaigns();
    
    $dist_camp = array();
    foreach($campaigns as $campaign) {
        $address            = dw_campaigns_build_location_addr($campaign);
        $campLocation       = dw_campaigns_get_location_for_address($address);

	// don't show locations which don't have a lat/long
	if(!isset($campLocation['lat'])) {
            continue;
        }

        $miles              = calculate_distance($userGeo['latitude'], $userGeo['longitude'], $campLocation['lat'], $campLocation['lng'], $unit);
        $key                = $miles . '-' . $campaign->nid;
        $dist_camp[$key]    = $campaign;
    }
    ksort($dist_camp, SORT_NUMERIC);
    
    return $dist_camp;
}

function dw_campaigns_get_body_class() {
    return dw_campaigns_make_body_class();
}

function dw_campaigns_get_class_body() {
    return dw_campaigns_make_body_class();
}

function dw_campaigns_make_body_class() {
    if(arg(4) == 'donate') {
        return 'donation-page';
    }
    if(arg(4) == 'donated') {
        return 'donate-complete';   
    }
    if(arg(4) == 'supporters') {
        return 'donors-page';   
    }
    if(arg(3) == 'leaderboard') {
        return 'leaderboard';
    }
    // this page is almost identical to the homepage, so let it have its styling
    if(substr($_GET['q'], 0, 19) == 'dw/walking/location') {
        return 'dw-walking walking-location';   
    }

    $extra = '';
    if(arg(1) == 'users') {
        $extra = ' dw-users';
    }   

    if(arg(1) == 'user') {
        $extra = ' dw-user';
    }

    if($_GET['q'] == 'dw/walking') {
        $extra = ' walking-home';
    } 

    if(isset($_REQUEST['ajax'])) {
       $extra .= ' ajax';
    }

    $page = $_GET['q'];
    $page = strtolower($page);
    $page = preg_replace("_/_", '-', $page);
    $page = preg_replace("/[^a-z0-9\-]/", '', $page);
   
    return $page . $extra;;
}

function dw_campaigns_get_all_campaign_location() {
    static $list = NULL;
    
    if(is_null($list)) {
        $campaigns  = dw_campaigns_get_active_campaigns();
        $list       = array();
        if(count($campaigns) > 0 ) {
            foreach($campaigns as $campaign) {
                $nid        = $campaign->nid;

                $location   = $campaign->field_dw_campaign_location['und']['0']['value'];
		if(dw_campaigns_hide_campaign($campaign) !== FALSE) {
                    // skip
                } else {
                    $list[$nid] = $location;
                }
            }
        } 
        asort($list, SORT_STRING);
    }
    return $list;
}

function dw_campaigns_get_user_pcp_details($user) {   
    $mode_type  = dw_campaigns_get_selected_type();
    
    $campaignId = NULL;
    $campaign   = NULL;
    $pcp        = NULL;
    $rawurl     = NULL;
   
    if($mode_type == 'walking') {	
        $pcp = dw_campaigns_user_get_pcp_for_walking($user);

        // only look up the campaign if we have a pcp
        if(!is_null($pcp)) {    
            $contribution_page_id   = $pcp->contribution_page_id;
    
            $campaigns  = dw_campaigns_get_active_campaigns(TRUE);
            $campaign   = $campaigns[$contribution_page_id];
            $campaignId = $campaign->nid;
        }
    } else {
        $campaignId = dw_campaigns_get_default_campaign();
        
        $campaign   = node_load($campaignId);
        $pcp        = dw_campaigns_user_get_pcp_for_campaign($user, $campaign);
    }
    
    if(isset($pcp->id) && !is_null($pcp->id)) {
        $rawurl = dw_campaigns_user_get_pcp_url($user, $campaign, TRUE);
    }
    
    return array('campaign_id' => $campaignId, 'campaign' => $campaign, 'pcp' => $pcp, 'url' => $rawurl);
    
}

function dw_campaigns_hide_campaign($campaign) {

    $campaign = is_object($campaign) ? $campaign : node_load($campaign);

    $location = isset($campaign->field_dw_campaign_location['und']['0']['value']) ? $campaign->field_dw_campaign_location['und']['0']['value'] : NULL;

    if(strstr($location, 'DONOTDISPLAY') !== FALSE) {
        return TRUE;
    }
    
    return FALSE;
}

function dw_campaigns_hide_contact($contactId) {
    static $hidden_contacts = NULL;

    if(is_null($hidden_contacts)) {
        $hidden_contacts = array();

        $params = array(
            'last_name'     => 'GeneralDonation',
            'rowCount'      => 999999
        );
        
        $contacts       = _dw_civicrm_contact_get($params);
        foreach($contacts as $contact) {
            $hidden_contacts[$contact->contact_id] = TRUE;
        }
    }
    
    if(isset($hidden_contacts[$contactId])) {
        return TRUE;
    }

    return FALSE;
}

function dw_campaigns_hide_contact_old($contactId) {

    $params = array(
        'contact_id'    => $contactId,
        'returnFirst'   => 1
    );
    
    $contact        = _dw_civicrm_contact_get($params);
    if($contact->last_name == 'GeneralDonation') {
        return TRUE;
    }

    return FALSE;
}

function dw_campaign_get_event_participants() {
   $campaigns = dw_campaign_get_event_participants_campaigns();


   foreach($campaigns as $campaignId) {
       $data[$campaignId] = dw_campaign_get_event_participants_by_campaign($campaignId);
   }

   return $data;
}

function dw_campaign_get_event_participants_campaigns($showLegacy = FALSE) {
    static $data = NULL;
$showLegacy = TRUE;
    if(is_null($data)) {
        $data   = array();

        //$result = db_query("select campaign_id from `dw_campaigns_event_participants`");
        //while($row = db_fetch_object($result)) {
    
        $active     = dw_campaigns_get_active_campaigns('nodeid');

        if(!$showLegacy) {
            $actives    = implode(',', array_keys($active));
    
            $result = db_query("select campaign_id from {dw_campaigns_event_participants} where campaign_id in ($actives) group by campaign_id");

            foreach($result as $row) {
                $data[] = $row->campaign_id;
            }
        } else {
            // MARKZ
            $data = dw_campaigns_get_all_campaign_ids();
        }
 
    }

    return $data;
}

function dw_campaign_get_event_participants_by_campaign($campaignId) {

    //$result = db_query("select * from `dw_campaigns_event_participants` where campaign_id = '%s'", $campaignId);
    //while($row = db_fetch_object($result)) {

    $result = db_query("select * from {dw_campaigns_event_participants} where campaign_id = :campaignid", array(':campaignid' => $campaignId));

    $data = array();
    foreach($result as $row) {
        if($row->pcp_id != 0) {
            $pcp = dw_campaigns_user_get_pcp_by_id($row->pcp_id);
            $contactId = $pcp->contact_id;
        } else {
            $contactId = $row->contact_id;
        }

        $params = array(
            'contact_id'    => $contactId,
            'returnFirst'   => 1
        );
        $contact = _dw_civicrm_contact_get($params);

        $seed = isset($row->participant_type) ? $row->participant_type : null;

        $data[$contactId . '-' . $seed] = array('contact' => $contact, 'row' => $row);
    }

    return $data;

// 07/02/2012 - we might not need this anymore, I think this was a work around for 2011
// we were missing a lot of data, so lets look up some more people

    $pcps = _dw_campaigns_get_pcps_for_campaign($campaignId);

    foreach($pcps as $pcp) {
        $contactId = $pcp->contact_id;
        if(!isset($data[$contactId])) {
            $params = array(
                'contact_id'    => $contactId,
                'returnFirst'   => 1
            );

            $contact               = _dw_civicrm_contact_get($params);

            $dummy      = new stdClass;
            $dummy->id  = $pcp->contribution_page_id;
            $campaign   = dw_campaigns_get_campaign_from_contribution_page($dummy);

            $row                   = new stdClass;

            $row->campaign_id      = $campaign->nid;
            $row->participant_type = 'adult_unknown';
            $row->pcp_id           = $pcp->id;
            $row->participants     = 1;
            // we currently don't set contact id if we have a pcp, NOTE - contact_id can get out of sync with dedupe merges!!!
            $row->contact_id       = 0; // $pcp->contact_id; 


            $data[$contactId] = array('contact' => $contact, 'row' => $row);
        } 
    }


    return $data;
}

function dw_campaigns_build_receipt_merge($campaign, $pcp_id, $contact_id, $contribution) {

    $mergeObjects   = dw_campaigns_get_merge_object($campaign, $pcp_id, $contact_id);
    // extra fields, fixups
    $mergeObjects['contribution']               = $contribution;

// TODO - make this honor the contributions currency, not the campaigns (since we added multi)
    $mergeObjects['contribution']->total_amount = dw_campaigns_force_decimal($mergeObjects['contribution']->total_amount, $campaign->field_dw_currency['und']['0']['value']);

    $receive_date                               = str_replace("-", "", $mergeObjects['contribution']->receive_date);
    $temp_receive_string                        = substr($receive_date, 0, 4) . '/' . substr($receive_date, 4, 2) . '/' . substr($receive_date, 6, 2);

    $mergeObjects['contribution']->receive_date = date("M d, Y", strtotime($temp_receive_string));
    $parts = explode("-", $contribution->invoice_id, 2);
    if(empty($parts[1])) {
        // generate new receipt id
        $res = dw_campaigns_invoice_sync_with_contributions($mergeObjects['contribution']->currency, $mergeObjects['contribution']->contribution_id, NULL);
        if(is_null($res)) {
            echo "failed to generate new receipt data";
            watchdog("failed", "failed generating new receipt data" .  $contribution->contribution_id);
            return;
        }

        $parts = explode("-", $res);
    }

    $mergeObjects['contribution']->receipt_id   = $parts[1];


    // this is a kludgy work around, I split the merge object creation so we can use this more places
    $pcp_contact    = $mergeObjects['pcp_contact'];

    //$from = sprintf("%s <%s>", $campaign->field_dw_receipt_from_name['und']['0']['value'], $campaign->field_dw_receipt_from_email['und']['0']['value']);
    $from = '"'. addslashes(mime_header_encode($campaign->field_dw_receipt_from_name['und']['0']['value'])) .'" <'. $campaign->field_dw_receipt_from_email['und']['0']['value'] .'>';

    $mergeObjects['campaign_obj'] = $campaign;

    return $mergeObjects;
}

function dw_campaign_receipt_report($arg) {
    global $dw_campaign_module_path;

    drupal_add_css($dw_campaign_module_path .'/theme/receipt.css');

    $merge_data = dw_campaigns_build_receipt_data($arg); 
    $data=theme('dw_campaigns_receipt_report', array('receiptData' => $merge_data));
    return $data;
}

function dw_campaign_receipt_report_email($arg, $force = TRUE, $no_redir = FALSE) {

    $merge_data = dw_campaigns_build_receipt_data($arg);
    if(is_null($merge_data)) {
        watchdog("email receipt", "Could not lookup data on contribution #{$arg}");
        return;
    }

    $status     = dw_campaigns_invoice_get_receipt_email_status_by_contribution_id($merge_data['contribution']->currency, $arg);

    if($status && !$force) {
       echo "Did not resend to: $arg - "  . $merge_data['contact']->email . "<br>";
       watchdog("email receipt", "Did not resend to: $arg - "  . $merge_data['contact']->email);
       return;
    }

    $file = dw_campaign_create_receipt_pdf($merge_data);

    if(!file_exists($file)) {
        watchdog('emailing pdf failed', 'file not found: ' . $file);
        file_put_contents("/tmp/pdf_email_fail", $file . "\n", FILE_APPEND);
        return;
    }

    $language = language_default();

    $params             = array();
    $params['subject']  = 'Your Official FPWR donation receipt';
    $params['file']     = $file; 

    $params['attachments'] =  array(array(
        'filepath' => $params['file'],
        'filemime' => 'application/pdf'
    ));

    $from               = variable_get('dw_campaigns_admin_receipt_from_email', '');
    $to                 = $merge_data['contact']->email;

    drupal_mail('dw_campaigns_admin_send_receipt', 'send_receipt', $to, $language, $params, $from);

    $parts = explode("-", $merge_data['contribution']->invoice_id);
    dw_campaigns_invoice_update_receipt_email($parts[1], $parts[2]);

    file_put_contents("/tmp/sent_pdf", $arg . "\n", FILE_APPEND);

    if(!$no_redir) {
        drupal_set_message(t('Receipt email sent to @email', array('@email' => $to)));
        drupal_goto('admin/reports/dw_receipts/' . $merge_data['contribution']->contribution_id);
    }
}

function dw_campaigns_admin_send_receipt_mail($key, &$message, $params) {

    switch($key)
    {   
        case 'send_receipt':

            $body = variable_get('dw_campaigns_admin_receipt_text', '');

            $body2 = variable_get('dw_campaigns_canadian_receipt_email', '');
            if(!empty($body2)) {
                $body = $body2;
            }

            $message['subject']             = $params['subject'];

            $message['body'][]              = $body;

            break;
    }


}

function dw_campaign_receipt_report_pdf($arg, $noreply = FALSE) {

    $merge_data = dw_campaigns_build_receipt_data($arg); 

    $file = dw_campaign_create_receipt_pdf($merge_data);

    if($noreply && 1==2) {
        echo "completed";
        die;
    }    

    $date = date("Y-m-d_His");
    $outfile = 'receipt-' . $arg . '-' . $date . '.pdf';
    $fsize   = filesize($file);

    header("Pragma: public"); // required
    header("Expires: 0");
    header("Cache-Control: must-revalidate, post-check=0, pre-check=0");
    header("Cache-Control: private",false); // required for certain browsers
    header("Content-Type: application/force-download"); 
    header("Content-Disposition: attachment; filename=\"$outfile\"");
    header("Content-Transfer-Encoding: binary");
    header("Content-Length: $fsize");
    header("Content-type: application/pdf");   
    echo file_get_contents($file);

    die;
}

function dw_campaign_create_receipt_pdf($merge_data, $force_generate = FALSE) {
    global $dw_campaign_module_path;


    $target_file = sprintf("%s/Receipt_Cache/OSS_Receipt-%s-%s.pdf", $dw_campaign_module_path, $merge_data['contribution']->contribution_id, $merge_data['contribution']->receipt_id);

    if(file_exists($target_file) && !$force_generate) {
        watchdog("pdf exists", $target_file);
        return $target_file;
    }

    $path =  $_SERVER['DOCUMENT_ROOT'] . '/' . $dw_campaign_module_path . '/zend_1.11.9-minimal/library';
    set_include_path(get_include_path() . PATH_SEPARATOR . $path);

    require_once 'Zend/Service/LiveDocx/MailMerge.php';
    require_once 'Zend/Service/LiveDocx/Exception.php';
   
    try {  
        $mailMerge = new Zend_Service_LiveDocx_MailMerge();
       
        $login      = variable_get('dw_campaigns_livedocx_login', '');
        $password   = variable_get('dw_campaigns_livedocx_password', '');
 
        $mailMerge->setUsername($login)
                  ->setPassword($password);
        
        include($dw_campaign_module_path . 'dw_campaigns.receipts_mapping.inc');
        
        $currency = $merge_data['contribution']->currency;
    
        $temp_source_doc = isset($receipt_mapping[$currency]) ? $receipt_mapping[$currency] : $receipt_mapping['default'];
        
        $source_doc = $_SERVER['DOCUMENT_ROOT'] . '/' . $dw_campaign_module_path . '/PDF_Receipts/' . $temp_source_doc;
    
        $mailMerge->setLocalTemplate($source_doc);
       
        if(!isset($merge_data['contact']->supplemental_address_1)) {
            $merge_data['contact']->supplemental_address_1 = '';
        }
 
        if(!isset($merge_data['contact']->street_address)) {
            $merge_data['contact']->street_address = '';
        } 
        if(!isset($merge_data['contact']->city)) {
            $merge_data['contact']->city = '';
        } 
     
        $mailMerge->assign('pcp_userdisplayname',           $merge_data['pcp_user']->displayname)
                  ->assign('fundraising_campaigntitle',     $merge_data['fundraising_campaign']->title)
                  ->assign('contactdisplay_name',           $merge_data['contact']->display_name)
                  ->assign('contactstreet_address',         $merge_data['contact']->street_address)
                  ->assign('contactsupplemental_address_1', $merge_data['contact']->supplemental_address_1)
                  ->assign('contactcity',                   $merge_data['contact']->city)
                  ->assign('contactstate_province_name',    $merge_data['contact']->state_province_name)
                  ->assign('contactpostal_code',            $merge_data['contact']->postal_code)
                  ->assign('contactcountry',                $merge_data['contact']->country)
                  ->assign('contactemail',                  $merge_data['contact']->email)
                  ->assign('contributionreceive_date',      $merge_data['contribution']->receive_date)
                  ->assign('contributiontotal_amount',      $merge_data['contribution']->total_amount)
                  ->assign('receiptid',                     $merge_data['contribution']->receipt_id);
        
        $mailMerge->createDocument();
        
        $document = $mailMerge->retrieveDocument('pdf');
    } catch (Zend_Service_LiveDocx_Exception $e) {
        watchdog("did I catch?", $e->getMessage());
    }

    if(isset($document)) {
        file_put_contents($target_file, $document);
    }
 
    return $target_file;
}

function dw_campaigns_build_receipt_data($arg) {
    // lookup regular contributions
    $params                             = array();
    $params['contribution_id']          = $arg;
    $params['rowCount']                 = 999999;
    $contributions                      = _dw_civicrm_contribution_get($params);

    if(count((array)$contributions) == 0) {
        watchdog("build_receipt_data", "Invoice for Contribution failed - no contribution #{$arg}");
        return NULL;
    }

    foreach($contributions as $contribution) {

    }

    $softcontributions  = _dw_civicrm_contributionsoft_get($params);
    foreach($softcontributions as $softcontribution) {

    }

    $pcp = dw_campaigns_user_get_pcp_by_id($softcontribution->pcp_id);

    $dummy      = new stdClass;
    $dummy->id  = $pcp->contribution_page_id;
    $campaign   = dw_campaigns_get_campaign_from_contribution_page($dummy);

    $merge_data = dw_campaigns_build_receipt_merge($campaign, $softcontribution->pcp_id, $softcontribution->contact_id, $contribution);

    if(!isset($contribution)) {
        file_put_contents("/tmp/f-$arg.dump", serialize($contributions));
    }

    return $merge_data;
}

function dw_campaign_offline_report($arg, $csv = FALSE) {
    $campaign = node_load($arg);
    $pcps = _dw_campaigns_get_pcps_for_campaign($campaign);

    $countries_longname = _dw_civicrm_pseudoconstant_country();
    $countries          = _dw_civicrm_pseudoconstant_countryIsoCode();

    $states             = _dw_civicrm_pseudoconstant_stateprovince();


    $printed = 0;

    echo "<table>";

    $contributions = NULL;

    foreach($pcps as $id => $pcp) {

        $params = array(
            'contact_id'    => $pcp->contact_id,
            'returnFirst'   => 1
        );

        $pcp_contact        = _dw_civicrm_contact_get($params);

        $maxtitlelength = 20;

        $title = $pcp->title;
        if(strlen($title) > $maxtitlelength) {
            $title = substr($pcp->title, 0, $maxtitlelength) . '...';
        }

        $results    = db_select('dw_campaigns_offline_donation', 'ol')->fields('ol')->condition('deleted', '0', '=')->condition('contribution_id', '-1', '=')->condition('pcp_id', $pcp->id, '=')->execute();

        foreach($results as $donor_contact) { 

            $countryID          = $donor_contact->country;
            $country_iso_code   = isset($countries[$countryID])?$countries[$countryID]:'';
            $stateID            = $donor_contact->state; 
            $state              = isset($states[$stateID])?$states[$stateID]:'';

            $parts = array(
                //'PCP Civi Id'                 => $pcp_contact->contact_id,
                'Participant First Name'      => $pcp_contact->first_name,
                'Participant Last Name'       => $pcp_contact->last_name,
                'Participant Display Name'    => $pcp_contact->display_name,
                'Page Title'                  => $title,              
                //'Donor Civi Id'               => $contribution->contact_id,
                'Donor First'             => $donor_contact->first_name,
                'Donor Last'              => $donor_contact->last_name,
                'Donor Address'           => $donor_contact->address_1,
                'Donor City'              => $donor_contact->city,
                'Donor State/Province'    => $state,
                'Donor Country'           => $country_iso_code,
                'Donor Postal Code'       => $donor_contact->postal_code,
                'Donor Email'             => $donor_contact->email,
                'Donation Amount'         => $donor_contact->donation_amount,
                'Payment Instrument'      => str_replace('Credit Card', 'Cash', $donor_contact->payment_instrument),
                'Check #'                 => $donor_contact->payment_check_number,
    
            );
            if($printed == 0) { 
                $keys = array_keys($parts);
                echo '<tr><th>' . implode('</th><th>', $keys) . '</th></tr>';   
                $printed = 1;
            }
    
            $keys = array_values($parts);
            echo '<tr><td>' . implode('</td><td>', $keys) . '</td></tr>';   
        }
    }
    echo '</table>';
}

function dw_campaign_offline_report_old($arg, $csv = FALSE) {
    $campaign = node_load($arg);
    $pcps = _dw_campaigns_get_pcps_for_campaign($campaign);


    $printed = 0;

    echo "<table>";

    $contributions = NULL;

    foreach($pcps as $id => $pcp) {

        $params             = array();
        $params['pcp_id']   = $pcp->id;
        $params['rowCount'] = 999999;

        $softcontributions  = _dw_civicrm_contributionsoft_get($params);

        if(is_null($contributions)) {
            $params                             = array();
            $params['contribution_page_id']     = $pcp->contribution_page_id;
            $params['rowCount']                 = 999999;
            $params['contribution_status_id']   = 2; // pending only
            $contributions                      = _dw_civicrm_contribution_get($params);
        }

        $params = array(
            'contact_id'    => $pcp->contact_id,
            'returnFirst'   => 1
        );
        $pcp_contact        = _dw_civicrm_contact_get($params);

        foreach($softcontributions as $id => $softcontribution) {
            // this should never happen
            if($softcontribution->pcp_id != $pcp->id) {
                continue;  
            }

            $key = '_' . $softcontribution->contribution_id;
            $contribution = $contributions->$key;

            // since we only look up contributions which are "offline" we need to skip a lot
            if(is_null($contribution)) {
                continue;
            }

            $maxtitlelength = 20;

            $title = $pcp->title;
            if(strlen($title) > $maxtitlelength) {
                $title = substr($pcp->title, 0, $maxtitlelength) . '...';
            }

            $params = array(
                'contact_id'    => $contribution->contact_id,
                'returnFirst'   => 1
            );
            $donor_contact      = _dw_civicrm_contact_get($params);
//var_dump($donor_contact);
            $temp         = explode(",", $contribution->sort_name);
            $parts = array(
                //'PCP Civi Id'                 => $pcp_contact->contact_id,
                'Participant First Name'      => $pcp_contact->first_name,
                'Participant Last Name'       => $pcp_contact->last_name,
                'Participant Display Name'    => $pcp_contact->display_name,
                'Page Title'                  => $title,              
                //'Donor Civi Id'               => $contribution->contact_id,
                'Donor First'             => $temp[1],
                'Donor Last'              => $temp[0],
                'Donor Address'           => $donor_contact->street_address,
                'Donor City'              => $donor_contact->city,
                'Donor State/Province'    => $donor_contact->state_province_name,
                'Donor Country'           => $donor_contact->country,
                'Donor Postal Code'       => $donor_contact->postal_code,
                'Donor Email'             => $donor_contact->email,
                'Amount'                  => $contribution->total_amount,
                'Payment Instrument'      => str_replace('Credit Card', 'Cash', $contribution->payment_instrument),
                'Check #'                 => $contribution->check_number,

            );
            if($printed == 0) { 
                $keys = array_keys($parts);
                echo '<tr><th>' . implode('</th><th>', $keys) . '</th></tr>';   
                $printed = 1;
            }

            $keys = array_values($parts);
            echo '<tr><td>' . implode('</td><td>', $keys) . '</td></tr>';   
        }
    }
    echo '</table>';
}

function dw_campaign_pcp_report($arg = 'ALL', $csv = FALSE) {
    global $dw_campaign_module_path;

    $arg = trim($arg);

    $file = $dw_campaign_module_path . "/civi_cache/report-$arg-" . time() . ".csv";
    $fp = fopen($file, "w");

    $printed = 0; 

    ob_start();

    $master_list = dw_campaigns_get_all_campaign_location();

    if(strcasecmp($arg, 'ALL') == 0) {
        $list = $master_list; 
    } else {
        $list[$arg] = $master_list[$arg];
    }

    echo "<table>";
    foreach($list as $campaign_id => $location) {
        $data = dw_campaign_get_event_participants_by_campaign($campaign_id);
        $campaign = node_load($campaign_id);
        foreach($data as $row) {
            $total_fee = 0;
            $total_donation = 0;
        
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
                $total_donation = 0;
                $total_fee      = 0;
                $total_offline  = 0;

                foreach($contributions as $donation) {
                    $total_fee      += $donation->non_deductible_amount;
                    $total_donation += ($donation->total_amount - $donation->non_deductible_amount);
                }

            }
       
            $parts = array(
                'civi_id'          => $row['contact']->contact_id,
                'first_name'       => $row['contact']->first_name,
                'last_name'        => $row['contact']->last_name,
                'display_name'     => $row['contact']->display_name,
                'email'            => $row['contact']->email,
                'phone'            => $row['contact']->phone,
                'location'         => $location,
                'currency'         => $campaign->field_dw_currency['und']['0']['value'],
                'total_donations'  => $total_donation,
                'total_offline'    => $total_offline,
                'total_fees'       => $total_fee,
                'num_donations'    => $total_count,
                'participant_type' => $row['row']->participant_type,
                'participant_num'  => $row['row']->participants,
            ); 
    
            if($row['contact']->last_name == 'GeneralDonation') {
                $name    = 'Total non pcp donations and fees';
                $email   = '';
                continue;
            }
        
            if($row['row']->pcp_id == 0 && $row['row']->contact_id == 0) {
                echo "error, both pcpid and contactid are 0!";
                continue;
            }
       
            if($printed == 0) { 
                $keys = array_keys($parts);
                array_walk($keys, 'dw_campaigns_report_headers');
                fputcsv($fp, $keys);
                echo '<tr><th>' . implode('</th><th>', $keys) . '</th></tr>';   
                $printed = 1;
            }
    
            $keys = array_values($parts);
            fputcsv($fp, $keys);
            echo '<tr><td>' . implode('</td><td>', $keys) . '</td></tr>';   
        }
        //flush();
    }
    echo "</table>";
    
    $contents = ob_get_contents();
    ob_end_clean();
    fclose($fp);

    if(!$csv) {
        echo $contents;
    } else {
        $date = date("Y-m-d_His");
        $outfile = 'report-' . $arg . '-' . $date . '.csv';
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
    }
}

function dw_campaigns_report_headers(&$value, $key) {
    $value = ucfirst(str_replace("_", ' ', $value));
}

/**
 *   Pass userid to get back a simplified version of the user record, pass 'ALL' to get back all the simplified data as an array
 *
 */
function dw_campaigns_cache_simple_user_data($uid) {

    static $userdata  = NULL;

    if(is_null($userdata)) {
        $userdata   = array();

        $result = db_query("select uid, name, mail, data from {users}");

        //while($row = db_fetch_object($result)) {
        foreach($result as $row) {
            if($row->uid == 0) {
                continue;
            }

            $data = unserialize($row->data);

            $row->data              = $data;

            // work around for how they used to do it
            $row->displayname       = isset($data['displayname'])  ? $data['displayname'] : NULL;

            $userdata[$row->uid]    = $row;
        }
    }

    if($uid == 'ALL') {
        return $userdata;
    }

    return isset($userdata[$uid]) ? $userdata[$uid] : NULL;
}

?>
