<?php

function __dw_campaigns_sort_by_usd_total($a_in, $b_in)
{
    $a  = $a_in['usd_total'];
    $b  = $b_in['usd_total'];

    if ($a == $b) {
        return 0;
    }

    return ($a > $b) ? -1 : 1;
}



    global $selected;

    $res = _dw_campaigns_campaigns_campaign_total();
    extract($res); // gives $totals, $usd_totals and $campaigns
  

    $num_per_page = $show_cnt; // 999999

    $table = "leader_as_$num_per_page";

    $headers        = array(
        array(
            'data'  => t('Position'),
            'field' => 'position',
            'class' => array('position')
        ),
        array(
            'data'  => t('Name'),
            'field' => 'name',
            'class' => array('name')
        ),
        array(
            'data'  => t('Fundraisers'),
            'field' => 'fundraisers',
            'class' => array('fundraisers')
        ),
        array(
            'data'  => t('Total Raised (USD)'),
            'field' => 'amount',
            'sort'  => 'desc',
            'class' => array('amount')
        )
    );


    $query = "
        CREATE TEMPORARY TABLE
            $table
            (
                name char(255),
                position int,
                amount float,
                raw_amount float,
                fundraisers int
            )
    ";
    db_query($query);


    $countries  =  dw_campaigns_event_countries_get_options();

    $buckets    =  array();

    foreach($countries as $key => $junk) {
        $buckets[$key]  = array('usd_total' => 0, 'total' => 0, 'pcp_count' => 0, 'fundraisers' => 0);
    }


    foreach($usd_totals as $nid => $usd_total) {
        $campaign       = $campaigns[$nid];

        if(dw_campaigns_hide_campaign($campaign)) {
           continue;
        }
      
        // countries with no value defined fall into this category 
        $country = isset($campaign->field_dw_country_grouping['und']['0']['value']) ? $campaign->field_dw_country_grouping['und']['0']['value'] : 'other'; 
       
        $buckets[$country]['usd_total']    += $usd_total; 
        $buckets[$country]['total']        += $totals[$nid]; 
        $buckets[$country]['fundraisers']  += $pcp_counts[$nid]; 
   }
   uasort($buckets, '__dw_campaigns_sort_by_usd_total');

   $position   = 0;
   $rows       = array();

   foreach($buckets as $key => $country) {

        $position++;

        $name           = $countries[$key];

        $amount         = $country['usd_total'];
        $raw_amount     = $country['total'];
        $fundraisers    = $country['fundraisers'];
        
        
        db_query("insert into {leader_as_$num_per_page} (name, position, amount, raw_amount, fundraisers) VALUES (:name, :position, :amount, :rawamount, :fundraisers)", array(
            ':name'         => $name, 
            ':position'     => $position, 
            ':amount'       => $amount, 
            ':rawamount'    => $raw_amount, 
            ':fundraisers'  => $fundraisers, 
        )); 
    }

    $sql_count = "select count(*) from $table";

    $result = db_select($table)->fields($table)->extend('PagerDefault')->limit($num_per_page)->extend('TableSort')->orderByHeader($headers)->execute();

    $rows = array();
    foreach($result as $db_row) {

        $rows[] = array(
            'data' => array(
                array('data' => $db_row->position, 'class' => array('position')),
                array('data' => '<span class="name">' . $db_row->name . '</span>', 'class' => array('name') ),
                array('data' => '<span>' . $db_row->fundraisers . '</span>', 'class' => array('fundraisers')),
                //array('data' => dw_campaigns_force_decimal($db_row->raw_amount, $our_campaign->field_dw_currency['und']['0']['value']), 'class' => array('amount')),
                array('data' => '<span>' . dw_campaigns_force_decimal($db_row->amount) . '</span>', 'class' => array('amount')),
            )    
        );
    }

    echo '<div class="topcountries"><h2>' . t('One Small Step Top Fundraising Countries') . "</h2>";
    echo theme('table', array('header' => $headers, 'rows' => $rows));
    echo theme('pager');
    echo '</div>';

?>
