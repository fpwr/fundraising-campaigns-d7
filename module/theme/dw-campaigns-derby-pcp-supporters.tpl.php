<?php

    if(is_null($pcp)) {
        return;    
    }

    $mode_type = dw_campaigns_get_selected_type();


    $supporters = dw_campaigns_pcp_get_supporters($pcp);


    if($mode_type != 'walking') {
        $headers        = array(
            array(
                'data'  => t('Name'),
                'field' => 'name',
            ),
            array(
                'data'  => t('Amount'),
                'field' => 'amount',
            ),
            array(
                'data'  => t('Location'),
                'field' => 'location',
            ),
            array(
                'data'  => t('date'),
                'field' => 'donationdate',
                'sort'  => 'desc',
            ),
            array(
                'data'  => t('Message'),
                'field' => 'note',
            )
        );
    } else {
        $headers        = array(
            array(
                'data'  => t('Name'),
                'field' => 'name',
            ),
            array(
                'data'  => t('Amount'),
                'field' => 'amount',
                'sort'  => 'desc',
            ),
            array(
                'data'  => t('Location'),
                'field' => 'location',
            ),
            array(
                'data'  => t('date'),
                'field' => 'donationdate',
            ),
            array(
                'data'  => t('Message'),
                'field' => 'note',
            )
        );
    }

    $table = "donations_as_$num_per_page";

    $query = "
        CREATE TEMPORARY TABLE
            $table
            (
                name char(255),
                photo char(255),
                email char(255),
                amount float,
                location char(255),
                donationdate date,
                note char(255) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,
                currency char(255),
                supporter_id int,
                supporter_in_roll int
            ) ENGINE = MYISAM CHARACTER SET utf8 COLLATE utf8_unicode_ci
    ";
    db_query($query);

    $contributions = array();

    $image_match = '';

    $query = db_insert($table)->fields(array('name', 'photo', 'email', 'amount', 'location', 'donationdate', 'note', 'currency', 'supporter_id', 'supporter_in_roll'));

    $rows = '';

    $contributions = dw_campaigns_get_contributions_by_pageid($pcp->contribution_page_id, TRUE);

    if(count($supporters) > 0) {
        $states         = _dw_civicrm_pseudoconstant_stateprovince();
    }

    foreach($supporters as $key => $supporter) {
/*
        // we now load all contributions for an entire campaign at once, and use that
        $params                     = array();
        $params['returnFirst']      = 1;
        $params['contribution_id']  = $supporter->contribution_id;
        $contribution               = _dw_civicrm_contribution_get($params);

        $contributions[$contribution->contribution_id] = $contribution;
*/
        // I don't think this is supposed to happen - this would only happen if there was not a contribution record that matched the soft contribution record!  Whats most likely is that someone altered the pcp record, but did not alter the page id on the contribution to match the pcps parent page id

        if($supporter->contribution_id == -1) {
            $contact                        = $supporter;
            $contribution                   = $supporter;

            $supporter->amount              = $supporter->donation_amount;
            $supporter->pcp_display_in_roll = $supporter->include_in_honor_roll;
            $contact->state_province        = $states[$supporter->state];

            if($supporter->pcp_display_in_roll == 1) {
                //$supporter->pcp_roll_nickname   = ucfirst($supporter->first_name) . substr(ucfirst($supporter->last_name), 0, 1) . "."; 
                $supporter->pcp_roll_nickname   = $supporter->first_name . ' ' . $supporter->last_name;
            }
        } else {

            if(isset($supporter->offline_id)) {
                continue;
            }

            $contribution           = isset($contributions[$supporter->contribution_id]) ? $contributions[$supporter->contribution_id] : NULL;
    
    
            // TODO - we should do or log something, this means they have a soft contribution but the normal contribution isn't visible!
            if(is_null($contribution)) {
                continue;
            }
    
    
            $params                 = array();
            $params['contact_id']   = $supporter->contact_id;
            $params['returnFirst']  = 1;
            $contact                = _dw_civicrm_contact_get($params);
        }
if(isset($supporter->pcp_roll_nickname) && is_object($supporter->pcp_roll_nickname)) {
    $supporter->pcp_roll_nickname = '';
}
        $photo = NULL;
        if($supporter->pcp_display_in_roll == 1) {
            $name   = $supporter->pcp_roll_nickname;
            //$photo  = _dw_campaigns_get_photo(array(), array(), 'donation-photo', $supporter->id, $image_match);
        } else {
            $name   = 'Anonymous';
            //$photo  = _dw_campaigns_get_photo(array(), array(), 'donation-photo', 0, $image_match);
        }

        @$name = "<!-- CONTRIB_ID: {$supporter->contribution_id} | CONTACT_ID: {$supporter->contact_id} -->" . $name;

        $email      = isset($contact->email) ? $contact->email : '';
        $amount     = $supporter->amount;
        $location   = (isset($contact->city) ? $contact->city . ", " : '') . (isset($contact->state_province) ? $contact->state_province : '');
        $date       = substr(trim($contribution->receive_date), 0, 10);
        $note       = ''; 

        if(isset($supporter->pcp_personal_note)) {
            if(is_object($supporter->pcp_personal_note)) {
                $note = '';
            } else {
                $note = $supporter->pcp_personal_note;
            }
        }
 
        $note       .= '<!-- CONTRIBUTION_ID: ' . $supporter->contribution_id . '-->';
        
        $currency   = $contribution->currency;

if(!$date) {
$date = NULL;
}
/*
var_dump($contribution);
var_dump($date);
*/

        $query->values(array(
            'name'              => $name,
            'photo'             => $photo,
            'email'             => $email,
            'amount'            => (float)$amount,
            'location'          => $location,
            'donationdate'      => $date,
            'note'              => $note,
            'currency'          => $currency,
            'supporter_id'      => isset($supporter->id) ? $supporter->id : 0,
            'supporter_in_roll' => $supporter->pcp_display_in_roll
        ));
    }

    $query->execute();


    //$sql_count = "select count(*) from donations_as_$num_per_page";
    //$result = db_query("select * from donations_as " . tablesort_sql($headers));
    //$result = pager_query("select * from donations_as_$num_per_page " . tablesort_sql($headers), $num_per_page, 0, $sql_count);

    $result = db_select($table)->fields($table)->extend('PagerDefault')->limit($num_per_page)->extend('TableSort')->orderByHeader($headers)->execute();

    $multi    = FALSE;
    $currency = $campaign->field_dw_currency['und']['0']['value'];
    if($currency == 'MULTI') {
        $multi = TRUE;
    }

    $rows = array();

    $i    = 0;

    //while ($db_row = db_fetch_object($result)) {
    foreach($result as $db_row) {
        $i++;
        if($multi) {
            $currency = $db_row->currency;
        }

        if($db_row->supporter_in_roll) {
            $photo  = _dw_campaigns_get_photo(array(), array(), 'donation-photo', $db_row->supporter_id, $image_match);

        } else {
            $photo  = _dw_campaigns_get_photo(array(), array(), 'donation-photo', 0, $image_match);
        }

        $rows[] = array(
            'data' => array(
                array('data' => '<a><img src="' . $photo . '" width="25">' . $db_row->name . '</a>', 'class' => array('name')),
                array('data' => dw_campaigns_force_decimal($db_row->amount, $currency), 'class' => array('amount')),
                array('data' => $db_row->location, 'class' => array('location')),
                array('data' => dw_campaigns_format_date($db_row->donationdate), 'class' => array('date')),
                array('data' => $db_row->note, 'class' => array('message')),
            )    
        );

        if($i==$num_per_page) {
            break;
        }
    }

    echo '<h2>' . $thisUser->data['displayname'] . ' Donors</h2>';
    //echo theme('table', $headers, $rows);

    echo theme('table', array('header' => $headers, 'rows' => $rows));

    // if we are showing the big page, enable paging
    if($num_per_page!=3) {
    	echo theme('pager');
        if($show_return) {
            echo '<a href="' . dw_campaigns_get_campaign_path($campaign->nid, '/dw/users/' . $thisUser->name) . '" class="see-all">' . t('return to fundraising page') . '</a>';
        }
    } else {    
        echo '<a href="' . dw_campaigns_get_campaign_path($campaign->nid, '/dw/users/' . $thisUser->name,  '/supporters') . '" class="see-all">' . t('see all') . '</a>';
    }
    
