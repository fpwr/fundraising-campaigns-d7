<?php

define('CACHE_LENGTH', 7200);

/**
 *  This seems completely unnecessary to do this every 'session', since the api_key doesn't change - this could just be done once
 */
function civicrm_rest_login($login, $pass) {
    
    $data		= array();
    $params['name']	= $login;
    $params['pass']	= $pass;

    $loginResponse	= civicrm_query('login', $params, TRUE, FALSE, TRUE);
    
    if(!$loginResponse) {
        throw new Exception('Invalid Login Credentials');
    }
    // this should only return one set
    foreach($loginResponse->children() as $loginData) {		
        if($loginData->is_error == 0) {
            $data['apiKey']     = (string) $loginData->api_key;
            $data['sessionId']	= (string) $loginData->PHPSESSID;
        } else {
            dw_campaigns_cache_dirty('login_');
            $error_message = isset($loginData->error_message) ? $loginData->error_message : '(unknown)';
            throw new Exception('Unable to login with these credentials - ' . $error_message);
        }
    }
    
    return $data;
}

function dw_campaigns_cache_get_filename($page, $params) {

    global $dw_campaign_module_path;

    static $md5s    = array();

    $secret_params  = array('key' => 1, 'api_key' => 1, 'name' => 1, 'pass' => 1);
    $page_filtered  = str_replace('/', '-', $page);

    ksort($params);

    $args = '';
    $secret_accumulator = '';

    foreach($params as $key => $value) {
        if(isset($secret_params[$key])) {
            $secret_accumulator .= "_{$key}={$value}";
        } else {
            $args .= "_{$key}={$value}";
        }
    }

    if(isset($md5s[$secret_accumulator])) {
        $md5    = $md5s[$secret_accumulator];
    } else {
        $md5    = md5($secret_accumulator);

        $md5s[$secret_accumulator] = $md5;
    }

    $cache_path = $dw_campaign_module_path . '/civi_cache/';
    $cache_file = $page_filtered . $args . '_' . $md5;

    return array($cache_path, $cache_file);
}

function dw_campaigns_cache_dirty($prefix, $pattern = '') {
    global $dw_campaign_module_path;

    $dir = $dw_campaign_module_path . '/civi_cache/';
    $search_pattern = $dir . $prefix.'*'.$pattern.'*';

    $files = glob($search_pattern);

    $target = $dw_campaign_module_path . "/dirty_log.txt";

    foreach($files as $file) {
        @file_put_contents($target, date('m-d-Y H:i:s ') . $file . "\n");
        unlink($file);
    }
}

/**
 * this naming is a bit wrong.  I put 'stale' cached files in a dirtied folder for later bulk processing
 * so that we don't dirty 30 entries on a single page load.  This is the bulk process
 */
function dw_campaigns_clean_dirtied_cache() {
    global $dw_campaign_module_path;

    $timestarted = time();

    $dest_dir    = $dw_campaign_module_path . '/civi_cache/';
    $src_dir     = $dw_campaign_module_path . '/dirtied_cache/';

    $files = glob($src_dir . '*');
   
    $file_times = array(); 
    foreach($files as $filename) {
        $ctime               = filectime($filename);
        $file_times[$ctime]  = $filename;
    }
    
    ksort($file_times);

    foreach($file_times as $filename) {

        if(time() > $timestarted + 180) {
            break;
        }

        $target = $dest_dir . basename($filename);

        $url    = file_get_contents($filename);

        if(strncmp($url, 'http', 4) == 0) {
            $data   = file_get_contents($url);
            file_put_contents($target, $data);
        }

        unlink($filename);       
        sleep(6);
    }
}

function civicrm_query($page, $params = array(), $cacheable = FALSE, $post = FALSE, $noLogin = FALSE) {
    global $debug;
    global $dw_settings;
    global $dw_campaign_module_path;
    
    static $dirtied_in_request = 0;

    $dirty  = FALSE;
    
    // if we aren't in the process of logging in (or doing an explicit non logged in query), and we aren't logged in login
    if(!$noLogin && (!isset($dw_settings['loggedIn']) || !$dw_settings['loggedIn'])) {
        if($debug) {
            echo "doing login";
        }
       _dw_do_login(); 
    }
    
    if($page[0] == '/') {
        $page = substr($page, 1);
    }
    
    $query	= '';
    
    $params['key'] = $dw_settings['siteKey'];

    if(isset($dw_settings['apiKey']) && !is_null($dw_settings['apiKey'])) {
        $params['api_key']  = $dw_settings['apiKey'];
    }

    $baseUrl        = "{$dw_settings['restUrl']}/{$page}";

    $queryString    = http_build_query( $params );


    $data   = NULL;

    if( !$post ) {
        
        // I don't like this really, but they follow the collapsing of arrays better than I will
        $url    = $baseUrl . '&' . $queryString;
        $url    = str_replace("&amp;", '&', $url);

        if($cacheable) {       
            list($cache_path, $cache_filename) = dw_campaigns_cache_get_filename($page, $params);
            $cache_file = $cache_path . $cache_filename;

            
            if(file_exists($cache_file)) {
                // we should expire old entries once in awhile, but lets not do more than 3 a page load
                if(strncmp($cache_filename, "contact-get_contact_id=", 23) !== 0 && filectime($cache_file) + CACHE_LENGTH < time()) {
                    if($dirtied_in_request < 3) {
                        $dirtied_in_request++;
                    } else {
                        $data   = file_get_contents($cache_file);
                        //file_put_contents($dw_campaign_module_path . '/dirtied_cache/' . basename($cache_file), $url);
                    }
                } else {
                    $data   = file_get_contents($cache_file);         
                }
            }
        
        }

        // even if we load from cache, if its null we want to reload it
        if(is_null($data)) {
            $dirty  = TRUE;
            $data   = do_curl($url);
        }


        if(strlen($data) == 0) {
            // file_put_contents("/tmp/ERR-" . microtime(TRUE), $cache_file . "\n" . $data);
        }
    } else {

        $data       = do_post_request($baseUrl, $queryString);

    }

    $cnt        = preg_match('/(<\?xml version="1.0"\?>.*)/sm', $data, $match);
    $newdata    = $match[0];
        
    $result     = simplexml_load_string($newdata);

    if($debug) {
        echo $url . "\n<br>\n";
    }

    if($cacheable && $dirty && strlen($data) > 0) {

        file_put_contents($cache_file, $data);
    }
    
    return $result;
}

function do_curl($url) {
    $ch = curl_init();
    curl_setopt($ch, CURLOPT_URL, $url);
    curl_setopt($ch, CURLOPT_HEADER, 0);
    curl_setopt($ch, CURLOPT_FOLLOWLOCATION, true);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
    return curl_exec($ch);
    //curl_close($ch);
}


function do_post_request($baseUrl, $queryString) {
    $curl = curl_init(); 

    $queryString    = str_replace("&amp;", '&', $queryString);

    curl_setopt($curl, CURLOPT_URL, $baseUrl); 
    curl_setopt($curl, CURLOPT_VERBOSE, 0); 
    curl_setopt($curl, CURLOPT_HEADER, 0); 
    curl_setopt($curl, CURLOPT_POST, 1);  
    curl_setopt($curl, CURLOPT_RETURNTRANSFER, 1); 
    curl_setopt($curl, CURLOPT_POSTFIELDS, $queryString); 

    $data = curl_exec($curl); 

    if(!curl_errno($curl)) { 
        $info = curl_getinfo($curl); 
    } else { 
        echo 'Curl error: ' . curl_error($curl); 
    } 
    
    curl_close($curl); 
    
    return $data;
}

function _dw_do_login() {
    global $dw_settings;
   
    dw_campaigns_civicrm_rest_settings_load();
 
    if(!isset($dw_settings['loggedIn']) || !$dw_settings['loggedIn']) {
        try {
            $loginResults               = civicrm_rest_login($dw_settings['loginName'], $dw_settings['loginPass']);
        } catch(Exception $e) {
            echo "Unable to login. Please Contact Support";
            die;
        }
        $dw_settings['apiKey']      = $loginResults['apiKey'];
        $dw_settings['loggedIn']    = TRUE;
    }
}


function _dw_civicrm_tag_create($params) {
        
    dw_campaigns_cache_dirty('tag');

    $returnOne      = false;

    if(!isset($params['used_for'])) {
        $params['used_for'] = 'civicrm_contribution_page';
    }

    $res = civicrm_query('/tag/create', $params, TRUE);

    $return = new stdClass;

    $return->tag_id = isset($res->Result->tag_id) ? (int) $res->Result->tag_id : NULL;

    dw_campaigns_cache_dirty('tag');

    return $return;

}

function _dw_civicrm_tag_get($params) {

    $returnOne      = false;

    $res = civicrm_query('/tag/get', $params, TRUE);

    $return = new stdClass;
    $return->tag_id = isset($res->Result->id) ? (int)$res->Result->id : NULL;

    return $return;

}


// can take multiple tag_id's - in the format of tag_id_1 = 12 tag_id_2 = 14
function _dw_civicrm_tag_entity_add($params) {

    if(!isset($params['entity_table'])) {
        $params['entity_table'] = 'civicrm_contribution_page';
    }

    $res = civicrm_query('/entity_tag/add', $params, FALSE);
    $return = json_decode(json_encode($res));

    return $return;

}

// can take multiple tag_id's - in the format of tag_id_1 = 12 tag_id_2 = 14
function _dw_civicrm_tag_entity_remove($params) {

    if(!isset($params['entity_table'])) {
        $params['entity_table'] = 'civicrm_contribution_page';
    }

    $res = civicrm_query('/entity_tag/remove', $params, FALSE);
    $return = new stdClass;

    return $return;

}

function _dw_civicrm_tag_entity_display($params) {

    if(!isset($params['entity_table'])) {
        $params['entity_table'] = 'civicrm_contact';
    }

    $res = civicrm_query('/entity_tag/display', $params, FALSE);
    $return = json_decode(json_encode($res));

    return $return;

}

/*
function civicrm_entity_tag_display( &$params ) {
    if ( !is_array($params) ) {
        return civicrm_create_error( ts( 'params should be an array.' ) );
    }
    
    $entityID    = null;
    $entityTable = 'civicrm_contact';
   
    if ( !( $entityID = CRM_Utils_Array::value( 'entity_id', $params ) ) ) {
        $entityID = CRM_Utils_Array::value( 'contact_id', $params );
*/



function _dw_civicrm_contact_get_from_user($user) {
    
    $contactId  = _dw_campaigns_user_get_contact_id($user);
    if(is_null($contactId)) {
        return NULL;
    }

    file_put_contents("/tmp/contact_from_user", "$contactId : " . $user->uid . "\n" , FILE_APPEND);

    $params = array(
        'contact_id'    => $contactId,
        'returnFirst'   => 1
    );
    return _dw_civicrm_contact_get($params);

}

/**
 * 
 *
 **/
function _dw_civicrm_contact_get($params) {

    $returnOne      = false;

    if(isset($params['returnFirst']) && $params['returnFirst']) {
        $returnOne  = true;
        unset($params['returnFirst']);
    }

    $res = civicrm_query('/contact/get', $params, TRUE);

    $return = new stdClass;
    if($res->Result->is_error === 0 ) {
        $return->contact_id = NULL; // when we look up a record, we only care if we know its number
    } else {
        $return = _dw_undo_simplexml($res->Result->children());
        unset($return->is_error);
        if($returnOne) {
            foreach($return as $key => $value) {
                $return = $value;
                break; // don't break if we want the last match
            }
        }
    }

    return $return;

}



function _dw_civicrm_contact_create($params) {
    if(isset($params['contact_id'])) {
        dw_campaigns_cache_dirty('contact-get', "contact_id=" . $params['contact_id']);
    }

    $res = civicrm_query('/contact/create', $params);
    
    $return = new stdClass;

    if(_dw_check_error($res)) {
        $return->id = NULL; // when we look up a record, we only care if we know its number
    } else {
        $return = _dw_undo_simplexml($res->Result->children());
        @file_put_contents("/tmp/create_contact", date("Y-m-d H:i:s") . "\n" . serialize($params) . "\n" . print_r($return, TRUE) . "\n\n\n", FILE_APPEND);
    }
    
    return $return;

}

function _dw_civicrm_contact_update($params) {
    return dw_civicrm_contact_create($params);
}
    

function _dw_civicrm_contributionsoft_get($params) {
    $res = civicrm_query('/contributionSoft/get', $params, TRUE);

    $return = new stdClass;

    if(isset($params['returnFirst']) && $params['returnFirst']) {
        if(_dw_check_error($res)) {
            $return->id = NULL; // when we look up a record, we only care if we know its number
        } else {
            $return = _dw_undo_simplexml($res->Result->children());
            
        }
    } else {
        if(_dw_check_error($res)) {
            $return = _dw_undo_simplexml($res->Result->error_data->children());
        } else {
            // lets wrapper it
            $res            = _dw_undo_simplexml($res->Result);
            if(isset($res->id)) {
                $id             = $res->id;
                $return->$id    = $res;
            }
        }
    }

    return $return;

}

function _dw_civicrm_contributionsoft_create($params) {
    global $debug;
    dw_campaigns_cache_dirty('contributionSoft-get_rowCount=999999');

    if(isset($params['pcp_id'])) {
        cache_clear_all($params['pcp_id'] . '_CONVERT', 'cache');
        cache_clear_all($params['pcp_id'] . '_DONT', 'cache');
    }

    $res = civicrm_query('/contributionSoft/create', $params);

    $return = new stdClass;

    if(_dw_check_error($res)) {
        $return->id = NULL; // when we look up a record, we only care if we know its number
    } else {
        $return = _dw_undo_simplexml($res->Result->children());
    }
    
    return $return;

}



function _dw_civicrm_contributionpage_create($params) {
    if(isset($params['id'])){
        dw_campaigns_cache_dirty('contributionPage-get', "id=" . $params['id']);
        dw_campaigns_cache_dirty('contributionPage-get_rowCount');
    } else {
        dw_campaigns_cache_dirty('contributionPage-get_rowCount');
    }   



    $res = civicrm_query('/contributionPage/create', $params);
    
    $return = new stdClass;

    if(_dw_check_error($res)) {
        $return->id = NULL; // when we look up a record, we only care if we know its number
    } else {
        $return = _dw_undo_simplexml($res->Result->children());
    }
    
    return $return;

}

function _dw_civicrm_contributionpage_get($params) {
    $res = civicrm_query('/contributionPage/get', $params, TRUE);
    
    $return = new stdClass;
    
    if(_dw_check_error($res)) {
        $return = _dw_undo_simplexml($res->Result->error_data->children());
    } else {
        if(!isset($res->Result)) {
            file_put_contents("/tmp/bah", print_r($res, TRUE));
        } else {
        $return = _dw_undo_simplexml($res->Result->children());
        }
    }
    
    return $return;

}

function _dw_civicrm_contribution_create($params) {

    cache_clear_all('dw_campaigns_campaigns_total', 'cache');

    dw_campaigns_cache_dirty('contributionSoft-get_rowCount');
    dw_campaigns_cache_dirty('contribution-get_rowCount');

    if(isset($params['pcp_made_through_id'])) {
        dw_campaigns_cache_dirty('contributionSoft', 'pcp_id=' . $params['pcp_made_through_id']);
    }
    if(isset($params['contribution_page_id'])) {
        dw_campaigns_cache_dirty('contribution', 'contribution_page_id=' . $params['contribution_page_id']);
    }

    $res = civicrm_query('/contribution/add', $params);
    
    $return = new stdClass;

    if(_dw_check_error($res)) {
        $return->id = NULL; // when we look up a record, we only care if we know its number
    } else {
        $return = _dw_undo_simplexml($res->Result->children());
    }
    
    return $return;

}

// for some reason 'id' looks up 'contact_id' - so you MUST specify contribution_id to look up by the 'id' column in civicrm_contribution
// this function returns a single object if returnFirst is set, or an obj->objects 
function _dw_civicrm_contribution_get($params) {

    $return = array(
        'contribution_page_id'    => 1, // added by Ryan
        'contact_type'            => 1,
        'contact_sub_type'        => 1,
        'sort_name'               => 1,
        'display_name'            => 1,
        'contribution_type'       => 1,
        'contribution_source'     => 1,
        'receive_date'            => 1,
        'thankyou_date'           => 1,
        'cancel_date'             => 1,
        'total_amount'            => 1,
        'accounting_code'         => 1,
        'payment_instrument'      => 1,
        'check_number'            => 1,
        'non_deductible_amount'   => 1,
        'fee_amount'              => 1,
        'net_amount'              => 1,
        'trxn_id'                 => 1,
        'invoice_id'              => 1,
        'currency'                => 1,
        'cancel_date'             => 1,
        'cancel_reason'           => 1,
        'receipt_date'            => 1,
        'thankyou_date'           => 1,
        'product_name'            => 1,
        'sku'                     => 1,
        'product_option'          => 1,
        'fulfilled_date'          => 1,
        'contribution_start_date' => 1,
        'contribution_end_date'   => 1,
        'is_test'                 => 1,
        'is_pay_later'            => 1,
        'contribution_status'     => 1,
        'contribution_status_id'  => 1,
        'contribution_recur_id'   => 1,
        'amount_level'            => 1,
        'contribution_note'       => 1
    ); 

    $params['return'] = $return;

    $res = civicrm_query('/contribution/get', $params, TRUE);

    $return = new stdClass;
    if(isset($params['returnFirst']) && $params['returnFirst']) {
        if(_dw_check_error($res)) {
            $return->id = NULL; // when we look up a record, we only care if we know its number
        } else {
            if(isset($res->Result)) {
                $return = _dw_undo_simplexml($res->Result->children());
            } 
        }
    } else {
        if(_dw_check_error($res)) {
            $return = _dw_undo_simplexml($res->Result->error_data->children());
        } else {
            $res            = _dw_undo_simplexml($res->Result);
            $id             = $res->contribution_id;
            $return->$id    = $res;
        }
    }

    return $return;

}


function _dw_civicrm_pcp_create($params) {
    // any attempt to create / update should dirty all pcp records
    dw_campaigns_cache_dirty('PCP-get', '');
    
    $res = civicrm_query('/PCP/create', $params);

    $return = new stdClass;
    
    if(_dw_check_error($res)) {
        $return->id = NULL; // when we look up a record, we only care if we know its number
    } else {
        $return = _dw_undo_simplexml($res->Result->children());
    }
    
    return $return;   
}

function _dw_civicrm_pcp_get($params) {
    
    $res = civicrm_query('/PCP/get', $params, TRUE);
    
    $return = new stdClass;
    $return->id = NULL; // If we don't hit any 'good' cases we should fail
    
    if(isset($params['returnFirst']) && $params['returnFirst']) {
        if(_dw_check_error($res)) {
            // we should never error if its just one record we want  
        } else {
            // get the single child record
            $return = _dw_undo_simplexml($res->Result->children());
        }
    } else {
	unset($return->id);

        // if we are expecting 1 or more, see if the error flag is set, if not, we only got one record
        if(! _dw_check_error($res) ) {
            $res            = _dw_undo_simplexml($res->Result);
            $id             = $res->id;
            if(is_null($id)) {
              $error = array();
              $error['params'] = $params;
              $error['res']    = $res;
              $error['data']   = $_REQUEST;

              // this error has been tracked down to the remote end doing - DB_DataObject Error: DB Error: connect failed
              //mail('', 'PCP READ ERROR', serialize($error));
            } else {
                $return->$id    = $res;
            }
        } else {
            // we fall in here if multiple records were returned, which they might if we want all pcps for a contribution page
            // but we should make sure its a valid response by checking the error string
            if(isset($res->Result->error_message) && strstr($res->Result->error_message, 'pcps matching input params') !== FALSE) {
                $temp_return = _dw_undo_simplexml($res->Result->error_data->children());
                $return = new stdClass;                
                foreach($temp_return as $key => $pcp) {
                    $key            = (string) preg_replace('/^_/', '', $key);
                    $return->$key   = $pcp;
                }
            }
        }
    }
    
    return $return;

}

function _dw_civicrm_pcp_search_owner($params) {
    global $debug;

    $res = civicrm_query('/PCP/search_owner', $params);
    
    $return = new stdClass;

    if(_dw_check_error($res)) {
        $return = NULL; // when we look up a record, we only care if we know its number
    } else { 
        $res = _dw_undo_simplexml($res->Result->Results->children());
        foreach($res as $key => $value) {
            $key            = preg_replace('/^_/', '', $key);
            $return->$key   = $value;
        }
    }
    
    return $return;

}

function _dw_civicrm_contributiontype_create($params) {
     $res = civicrm_query('/contributionType/create', $params);
    
    $return = new stdClass;
    
    if(_dw_check_error($res)) {
        $return->id = NULL; // when we look up a record, we only care if we know its number
    } else {
        $return = _dw_undo_simplexml($res->Result->children());
    }
    
    return $return;   
}


function _dw_civicrm_event_create($params) {
    
    $res = civicrm_query('/event/create', $params);
    
    $return = new stdClass;

    if(_dw_check_error($res)) {
        $return->id = NULL; // when we look up a record, we only care if we know its number
    } else {
        $return = _dw_undo_simplexml($res->Result->children());
    }
    
    return $return;

}

function _dw_civicrm_event_get($params) {
    
    $res = civicrm_query('/event/get', $params);
    
    $return = new stdClass;
    
    if(_dw_check_error($res)) {
        $return->id = NULL; // when we look up a record, we only care if we know its number
    } else {
        $return = _dw_undo_simplexml($res->Result->children());
    }
    
    return $return;

}

/**
 * Note - unlike their function, this will return multiple matches if you use a generic enough WHERE clause like event_id or no WHERE at all
 * they default to returning 25 rows, and if there is somehow a participant_id dupe in those rows they will only return one row for each dupe
 * so use  $params['rowCount'] = 999999; to ensure you get all the results
 */
function _dw_civicrm_participant_get($params) {
    
    $res = civicrm_query('/participant/get', $params);
    
    $return = new stdClass;
    
    if(_dw_check_error($res)) {
        if(!isset($res->Result->error_data)) {
            $return->participant_id = NULL; // when we look up a record, we only care if we know its number
        } else {
            $return = _dw_undo_simplexml($res->Result->error_data->children());
        }
    } else {
// TODO - double check this is how we want this to behave
        $return = _dw_undo_simplexml($res->Result->children());
        if(isset($return->result)) {
            $return->participant_id = $return->result; // this function seems to behave different then others
        }
    }

    return $return;

}

function _dw_civicrm_participant_create($params) {
    
    $res = civicrm_query('/participant/create', $params);
    
    $return = new stdClass;

    if(_dw_check_error($res)) {
        $return->id = NULL; // when we look up a record, we only care if we know its number
    } else {
        $return = _dw_undo_simplexml($res->Result->children());
    }
    
    return $return;

}

function _dw_civicrm_friend_create($params) {
    global $debug;
    $debug = true;
    $res = civicrm_query('/friend/create', $params);
    $debug = false;
    $return = new stdClass;

    if(_dw_check_error($res)) {
        $return->id = NULL; // when we look up a record, we only care if we know its number
    } else {
        $return = _dw_undo_simplexml($res->Result->children());
    }
    
    return $return;

}


function _dw_civicrm_pseudoconstant_stateprovince() {
    $res = civicrm_query('/pseudoconstant/stateprovince', array(), TRUE);

    $return = array();
    foreach($res->Result->stateProvince->children() as $key => $value) {
        $key            = (string) preg_replace('/^_/', '', $key);
        $value          = (string) $value;
        $return[$key]   = $value;
    }
   
    $return['nostate'] = '(No State)';
 
    return $return;
}

function _dw_civicrm_pseudoconstant_country() {
    $res = civicrm_query('/pseudoconstant/country', array(), TRUE);

    $return = array();
    foreach($res->Result->countries->children() as $key => $value) {
        $key            = (string) preg_replace('/^_/', '', $key);
        $value          = (string) $value;
        $return[$key]   = $value;
    }

    // scoot the US and Canada to the top
    $others = array();
    if(isset($return[1228])) {
        $others[1228] = $return[1228];
        unset($return[1228]);
    }
    if(isset($return[1039])) {
        $others[1039] = $return[1039];
        unset($return[1039]);
    }
    // france
    if(isset($return[1076])) {
        $others[1076] = $return[1076];
        unset($return[1076]);
    }
    // begium
    if(isset($return[1020])) {
        $others[1020] = $return[1020];
        unset($return[1020]);
    }
    // New Zealand
    if(isset($return[1154])) {
        $others[1154] = $return[1154];
        unset($return[1154]);
    }

    return $others + $return;
}

function _dw_civicrm_pseudoconstant_countryIsoCode() {
    $res = civicrm_query('/pseudoconstant/countryIsoCode', array(), TRUE);

    $return = array();
    foreach($res->Result->countries->children() as $key => $value) {
        $key            = (string) preg_replace('/^_/', '', $key);
        $value          = (string) $value;
        $return[$key]   = $value;
    }
    
    return $return;
}



function _dw_campaigns_pseudoconstant_paymentprocessor() {
    $res = civicrm_query('/pseudoconstant/paymentprocessor', array());
    
    $return = array();
    foreach($res->Result->paymentprocessor->children() as $key => $value) {
        $key            = (string) preg_replace('/^_/', '', $key);
        $value          = (string) $value;
        $return[$key]   = $value;
    }
    
    return $return;
}
// PaymentProcessor::getPayment

function _dw_campaigns_paymentprocessor_getpayment($params) {
    $res = civicrm_query('/paymentprocessor/getpayment', $params);

    $return = new stdClass;

    if(_dw_check_error($res)) {
        return NULL; // when we look up a record, we only care if we know its number
    } else {
        $return = _dw_undo_simplexml($res->Result);
    }

    return $return;
}

function _dw_campaigns_payment_dodirectpayment($params) {
    $res = civicrm_query('/payment/dodirectpayment', $params, FALSE, TRUE);

    $return = new stdClass;

    if(_dw_check_error($res)) {
        $return->error = (int) $res->Result->error_message; // when we look up a record, we only care if we know its number
    } else {
        $return = _dw_undo_simplexml($res->Result);
    }

    return $return;
}

function _dw_campaigns_dwutils_get_deduped_contact_id($params) {
    $res = civicrm_query('/dwutils/get_deduped_contact', $params);

    $return = new stdClass;

    if(_dw_check_error($res)) {
        $return->id = NULL; // when we look up a record, we only care if we know its number
    } else {

        $contact_id   = (string) $res->Result->result;

        dw_campaigns_cache_dirty('contact-get', "contact_id=" . $contact_id);

        $return       = $contact_id;
    }
    
    return $return;
}

// for our purpose we need an ajax version of stateProvinceForCountry, but we'll use the civi ajax way (this is how taylors states worked)
function _dw_civicrm_jq_state($country) {
    global $dw_settings;
    
    $parsed = parse_url($dw_settings['restUrl']);
    
    $target = sprintf("http://%s/civicrm/ajax/jqState?_id=address_1_country_id&_name=address%%5B1%%5D%%5Bcountry_id%%5D&_value=%s", $parsed['host'], $country);

    $res = file_get_contents($target);
    $data = json_decode($res);

    if(count($data) == 1) {
        $dummy              = new stdClass;
//     $return['nostate']   = '(No State)';
        $dummy->name        = "(No State)";
        $dummy->value       = "nostate";

        $data       = array();
        $data[]     = $dummy;
    }

    echo json_encode($data);

    die;
}

function dw_campaigns_make_url($name = NULL, $echoResults = true) {

    $post_name  = isset($_POST['name']) ? $_POST['name'] : NULL;
    $name       = !is_null($name) ? $name : $post_name;
    
    $result = strtolower($name);
    $result = preg_replace("/[^a-z_0-9]/", '', $result);
    
    if($echoResults) {
        echo $result;
        exit;
    } 
    
    return $result;
}

function _dw_save_node_to_civi_campaign_glue($nid, $page_id, $enabled = 1) {
    // the key is the node -> contribution page combo, so if we have it, we have it

    $result = db_query("select * from {dw_campaigns_campaigns_to_contribution_pages} where nid = :nid", array(':nid' => $nid));

    if($result->rowCount() == 1) {
        db_query("update {dw_campaigns_campaigns_to_contribution_pages} set active = :active where nid = :nid", array(
            ':nid'      => $nid, 
            ':active'   => (($enabled == 1) ? 'Y' : 'N'),
        ));

    } else {

        db_query("insert INTO dw_campaigns_campaigns_to_contribution_pages SET nid = :nid, contribution_page_id = :pageid, active = :active", array(
            ':nid'      => $nid, 
            ':pageid'   => $page_id,
            ':active'   => (($enabled == 1) ? 'Y' : 'N'),
        ));

    }
}

function _dw_save_node_to_civi_event_glue($nid, $event_id) {
    
    db_query("update dw_campaigns_campaigns_to_contribution_pages SET event_page_id = :eventid where nid = :nid", array(
        ':eventid'  => $event_id, 
        ':nid'      => $nid));
    
}

function _dw_save_user_to_contact_glue($user, $contact) {

    if($contact->contact_id == 1) {
        drupal_set_message(t('A system error has occured, please contact ossbug@fpwr.org with details about what you were doing when this error occured - if you were signing up please try again later'), 'error');

		// FIXME - this should be configured on an admin form
        mail('root@', 'failure - contact_id = 1', date("Y-m-d H:i:s"));

        drupal_goto('dw');
    }

    //db_query("insert into dw_campaigns_drupal_civi_contact_mapping (drupal_id, civi_contact_id) VALUES('%s', '%s')", $user->uid, $contact->contact_id);
    db_query("insert into dw_campaigns_drupal_civi_contact_mapping (drupal_id, civi_contact_id) VALUES(:drupalid, :contactid)", array(
        ':drupalid'     => $user->uid, 
        ':contactid'    => $contact->contact_id));
   
}

function _dw_civicrm_pcp_sync_titles($params) {
    $res = civicrm_query('/PCP/sync_titles', $params);
    
    $return = new stdClass;
    
    if(_dw_check_error($res) || !isset($res->Result)) {
        $return->id = NULL; // when we look up a record, we only care if we know its number
    } else {
        $return = _dw_undo_simplexml($res->Result->children());
    }
    
    return $return;   
}

function _dw_check_error($xml) {
    global $debug;
    
    if(isset($xml->Result->is_error) && $xml->Result->is_error == 1) {
        if($debug) {
            echo "there was an error, do something";
            print_r($xml);
        }
        return true;
    }
    return false;
}

function _dw_undo_simplexml($xml) {
    @$data = json_decode(json_encode($xml));
    return $data;
}


/**
 * Updates a users civic crm contact quickly and easily. You can pass it $form_state['values'] and it
 * should do all the majic for you
 * 
 * @param $user
 * @param $params
 */
function _dw_campaigns_update_civicrm_contact($user, $values) {


    $address_values = array(
        'address_1'     => 'street_address',
        'address_2'     => 'supplemental_address_1',
        'city'          => 'city',
        'postal_code'   => 'postal_code',
        'state'         => 'state_province_id',
        'country'       => 'country_id'
    );
    
    //if a user is passed, we're updating
    $contact = _dw_civicrm_contact_get_from_user($user);
    
    if(isset($contact->contact_id) && !is_null($contact->contact_id)) {
        $params['contact_id']   = $contact->contact_id;
        dw_campaigns_cache_dirty('contact-get', "contact_id=" . $contact->contact_id);
    } else {
        $params['contact_id'] = NULL;
    }

    if(isset($values['email'])) {
        dw_campaigns_cache_dirty('contact-get', "email=" . $values['email']);
    }
 
    if(isset($values['first_name'])) { 
        $params['first_name']       = $values['first_name'];
    }

    if(isset($values['last_name'])) { 
        $params['last_name']        = $values['last_name'];
    }

    $params['contact_type']     = 'Individual';

    if(isset($values['email']) && !empty($values['email'])) {
        $params['email'][1]         = array(
            'email'                 => $values['email'],
            'location_type_id'      => 1
        );
        $params['email'][2]         = array(
            'email'                 => $values['email'],
            'location_type_id'      => 5
        );
    }
 
    // only update it if its defined..
    if(isset($values['telephone'])) {
        $params['phone'][1]         = array(
            'phone_type_id'         => 1,
            'phone'                 => $values['telephone'],
            'location_type_id'      => 1,
            'is_primary'            => 1
        );
    }
    
    // all address[1] things used to be in an array which assumed all values were set
    // so this now handles it a bit smarter...
    $foundAddress   = FALSE;
    foreach($address_values as $source => $destination) {
        if(isset($values[$source])) {
            if(!isset($params['address'][1])) {
                $params['address'][1] = array();
            }
            $params['address'][1][$destination] = $values[$source];
            $foundAddress = TRUE;
        }
    }
    
    // TODO - these fields were set to these values, I'm not sure if its necessary or if these were place holders
    if($foundAddress) {
        $params['address'][1]['location_type_id']          = 1;
        $params['address'][1]['is_primary']                = 1;
        $params['address'][1]['supplemental_address_2']    = '';
        $params['address'][1]['postal_code_suffix']        = '';
        $params['address'][1]['geo_code_1']                = ''; 
        $params['address'][1]['geo_code_2']                = '';
    }
    
    // this used to call civicrm_contact_update
    $contact = _dw_civicrm_contact_create($params);

    if(isset($values['pws_relationship'])) {
        @file_put_contents("/tmp/relationship", $contact->contact_id . "," . $values['pws_relationship']);
        dw_campaigns_tag_contact($values['pws_relationship'], array($contact->contact_id));
    }


    return $contact;
    // TODO - note: $contact is not really a fully contact record, its just $contact->contact_id being the new / updated contact id (create / update)
    //$contact->contact_id;
}



function fancy_merge_string($keyArray, &$subject)
{
    global $debug;

    // if its not an array bail
    if(!is_array($keyArray))
    {
        drupal_set_message('invalid key array to fancy merge', 'error');
	return;
    }

    foreach($keyArray as $group => $list)
    {
        if(is_null($list) || (!is_object($list) && !is_array($list))) {
            continue;
        }

        foreach($list as $key => $value) {
            $recursed = FALSE;

            $token = sprintf('{{%s}{%s}}', $group, strtolower($key));

            // if the first level is an object cast it as an array and check it below
            if(is_object($value)) {
    
                $value  = (array) $value;
            }
    
            if(is_array($value))
            {

                // if this a normal drupal sub array?
                if(isset($value['und']['0']['value'])) {
                    $value = $value['und']['0']['value'];

                } elseif(isset($value[0])) {
                    // if we have an array of items (like the old cck field for address) loop through the parts
                    $recursed = TRUE;
                    foreach($value[0] as $key2 => $value2) {
                        if(!is_array($value2) && !is_object($value2)) {
                            $token = sprintf('{{%s}{%s}{%s}}', $group, strtolower($key), strtolower($key2));
                            if($debug) {
                                echo $token . " = $value2<br>";
                            }
                            $subject = str_replace($token, $value2, $subject);
                        }
                    }
                }
            }
             

// do the substitution if it was not done in a recursive above
            if(!$recursed) {
                if($debug) {
                    echo $token . " = $value<br>";
                }

                if(!is_array($value)) {                
                    $subject = str_replace($token, $value, $subject);
                }
            }     

        }
    }

}
