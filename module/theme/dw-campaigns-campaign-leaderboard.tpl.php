<?php

function sort_leaders($a, $b) {

    if($a['amount'] < $b['amount']) {
       return 1;
    }

    if($a['amount'] > $b['amount']) {
       return -1;
    }
    
    return 0;

}
    
    $num_per_page = 40;

    if(!is_null($campaign)) {
        $leaders = _dw_campaigns_campaign_leaders($campaign, 9999);
    } else {
        if($campaignPath == 'legacy') {
            $leaders = _dw_campaigns_all_leaders(9999, TRUE, 'PREVIOUS_YEARS');
            $num_per_page   = 50;
        } else {
            $leaders = _dw_campaigns_all_leaders(9999, TRUE);
        }
    }
    
    $type = dw_campaigns_get_selected_type();


    $table = "{leader_as_$num_per_page}";

    $headers        = array(
        array(
            'data'  => t('Position'),
            'field' => 'position',
        ),
        array(
            'data'  => t('Name'),
            'field' => 'name',
        ),
        array(
            'data'  => t('Location'),
            'field' => 'location',
        ),
        array(
            'data'  => t('Donations'),
            'field' => 'donations',
        ),
        array(
            'data'  => t('Total Raised'),
            'field' => 'amount',
            'sort'  => 'desc'
        )
    );


    $query = "
        CREATE TEMPORARY TABLE
            $table
            (
                name char(255),
                drupal_id int,
                photo char(255),
                position int,
                amount float,
                raw_amount float,
                campaign_id int,
                location char(255),
                donations int,
                url char(255)
            )
    ";
    db_query($query);

    $position   = 0;
    $rows       = array();
    $campaigns  = array();

    $extra='';

    if(count($leaders) > 0) { 
        $sorted_leaders = array();

        foreach($leaders as $leader) {
            if(!isset($campaigns[$leader['campaign_id']])) {
                $campaigns[$leader['campaign_id']] = node_load($leader['campaign_id']);
            }
            $our_campaign       = $campaigns[$leader['campaign_id']];
            $amount             = dw_campaigns_convert_to_usd($our_campaign->field_dw_currency['und']['0']['value'], $leader['total']);

            if(dw_campaigns_hide_campaign($campaign)) { 
                continue;
            }

            $leader['amount']   = $amount;
            $sorted_leaders[]   = $leader;
        }

        usort($sorted_leaders, "sort_leaders");
$start = microtime(TRUE);


$query = db_insert($table)->fields(array('name', 'drupal_id', 'photo', 'position', 'amount', 'raw_amount', 'campaign_id', 'location', 'donations', 'url'));

        $rows = '';
        foreach($sorted_leaders as $leader) {

            $campaign_id    = $leader['campaign_id'];

            $position++;
            $contact        = $leader['contact'];
    
            $name           = $leader['name'];
            $photo          = '';
            $raw_amount     = $leader['total'];
            $amount         = $leader['amount'];   
/* 
            if($type != 'walking') {
                $location   = $contact->city . ', ' . $contact->state_province;
            } else {
*/
                $location   = $leader['campaign_location'];
//            }
            
            $donations      = $leader['donations'];
            $url            = $leader['url'];
           
            $query->values(array(
                    'name'          => $name,
                    'drupal_id'     => $leader['drupal_id'],
                    'photo'         => $photo,
                    'position'      => $position, 
                    'amount'        => $amount,
                    'raw_amount'    => $raw_amount,
                    'campaign_id'   => $campaign_id,
                    'location'      => $location,
                    'donations'     => $donations,
                    'url'           => $url,
                )); 
        }


        $query->execute();
  
$end = microtime(TRUE);
$diff = $end - $start;
//echo $diff;die;


 
        //$result = db_query("select * from {donations_as} " . tablesort_sql($headers));
	$result = db_select($table)->fields($table)->extend('PagerDefault')->limit($num_per_page)->extend('TableSort')->orderByHeader($headers)->execute();

        $rows = array();

        $convert_currency   = TRUE;
        if($type != 'walking') {
            $convert_currency   = FALSE;
        }
            $convert_currency   = FALSE;


        $i=0; 
        foreach($result as $db_row) {
            $i++;
            $our_campaign = $campaigns[$db_row->campaign_id];

            if(dw_campaigns_hide_campaign($our_campaign)) {
                continue;
            }

// start get user photo
            $image_match    = '';  
            $image_params   = array(
                'w'                 => 100,
                'contribution'      => true,
            );

//echo $db_row->drupal_id  . '<br>';
            $photo          = _dw_campaigns_get_photo($db_row->drupal_id, $image_params, 'user-photo', NULL, $image_match);
// end get user photo

            $rows[] = array(
                'data' => array(
                    array('data' => $db_row->position, 'class' => array('position')),
                    array('data' => '<img src="' . $photo . '" width="25"> <a href="' . $db_row->url . '">' . $db_row->name . '</a>', 'class' => array('name') ),
                    array('data' => $db_row->location),
                    array('data' => $db_row->donations),
                    array('data' => dw_campaigns_force_decimal($db_row->raw_amount, $our_campaign->field_dw_currency['und']['0']['value'], $convert_currency)),
                )    
            );

// lets exit it we have as many as we're going to show
            if($i==$num_per_page) {
                break;
            }
        }
    
        if($type == 'walking') {
            if(is_null($campaign)) {
                $extra = t('(All Locations)');
            } else {
                $extra = '(<a href="/dw/walking/location/' . $campaign->nid . '">' . $campaign->field_dw_campaign_location['und']['0']['value'] . '</a>)';
            }
        }
    }
    //echo theme('dw_campaigns_derby_statistics', $campaign, TRUE);
    echo "<h2>" . t('Leader Board') . " $extra</h2>";
/*
    echo theme('table', $headers, $rows);
    echo theme('pager', NULL, $num_per_page, 0);
*/

    echo theme('table', array('header' => $headers, 'rows' => $rows));
    echo theme('pager');
