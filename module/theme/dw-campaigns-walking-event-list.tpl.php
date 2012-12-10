<?php
function __dw_campaigns_sort_by_campaign_date($a_in, $b_in)
{
    $a  = isset($a_in->field_dw_date_range['und']['0']['value']) ? $a_in->field_dw_date_range['und']['0']['value'] : 0;
    $b  = isset($b_in->field_dw_date_range['und']['0']['value']) ? $b_in->field_dw_date_range['und']['0']['value'] : 0;

    if ($a == $b) {
        return 0;
    }

    return ($a < $b) ? -1 : 1;
}

    $campaigns  = dw_campaigns_get_active_campaigns();

   usort($campaigns, '__dw_campaigns_sort_by_campaign_date');
?>

<div class="upcoming-events">
<h2>Upcoming Events</h2>
<?php

    $headers        = array(
        array(
            'data'  => t('Date'),
        ),
        array(
            'data'  => t('Event'),
        ),
        array(
            'data'  => t('Contact'),
        ),
/*
        array(
            'data'  => t('Status'),
        )
*/
    );

    $now    = time();
    $rows   = array();
    $rows_o = array();

    $host_data  = array();
    
    foreach($campaigns as $campaign) {
   
        if(dw_campaigns_hide_campaign($campaign)) { 
            continue;
        } 
 
        $raw_time       = isset($campaign->field_dw_date_range['und']['0']['value']) ? $campaign->field_dw_date_range['und']['0']['value'] : 0;   
 
        $c_timestamp    = strtotime($raw_time);
    
        $date           = date("M d Y", $c_timestamp);
        if($date == 'Dec 31 1969') {
            $date = '';
        }

        $title          = isset($campaign->field_dw_campaign_location['und']['0']['value']) ? $campaign->field_dw_campaign_location['und']['0']['value'] : $campaign->title;

        $location       = '<span class="name"><a href="/dw/walking/location/' . $campaign->nid . '">' . $title . '</a></span>';
        $location       = str_replace('One SMALL Step,', '', $location);

        $hosts          = dw_campaigns_get_campaign_hosts($campaign);
        $host           = implode(', ', $hosts);
    
        if($c_timestamp < $now) {
            $rows_o[] = array(
                'data' => array(
                    array('data' => $date, 'class' => array('date')),
                    array('data' => $location, 'class' => array('location')),
                    array('data' => $host, 'class' => array('hosts')),
//                    array('data' => 'Completed', 'class' => array('status')),
                ),
               'class' => array('completed')
            );
        } else {
            $rows[] = array(
                'data' => array(
                    array('data' => $date, 'class' => array('date')),
                    array('data' => $location, 'class' => array('location')),
                    array('data' => $host, 'class' => array('hosts')),
//                    array('data' => 'Upcoming', 'class' => array('status')),
                ),
               'class' => array('upcoming')
            );
    
        }
    }

    $intro      = array();
    $upcoming   = t('Upcoming Events');

    $intro[] = array(
        'data' => array(
            array('data' => $upcoming, 'class' => array('break_row'), 'colspan' => 4),
        )
    ); 

    $break      = array();
    $completed   = t('Completed Events');
    $break[] = array(
        'data' => array(
            array('data' => $completed, 'class' => array('break_row'), 'colspan' => 4),
        )
    ); 

    $out = array_merge($intro, $rows);
    $out = array_merge($out, $break);
    $out = array_merge($out, $rows_o);

    echo theme('table', array('header' => $headers, 'rows' => $out));
   // echo theme('table', array('header' => $headers, 'rows' => $rows + $break + $rows_o));
?>
</div>
