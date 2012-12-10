<?php
   
    $result = $rows;

    $rows = array();
 
    foreach($result as $arr_row) {
        $db_row = (object) $arr_row;

        $offline = $db_row->offline_total . " p(" . $db_row->offline_pending_total . ")";

        $rows[] = array(
            'data' => array(
                array('data' => $db_row->location, 'class' => array('location')),
                array('data' => $db_row->host, 'class' => array('host')),
                array('data' => $db_row->event_date, 'class' => array('event_date')),
                array('data' => $db_row->pcp_count, 'class' => array('pcp_count')),
                array('data' => $db_row->participant_count, 'class' => array('participant_count')),
                array('data' => $db_row->donations_count, 'class' => array('donations_count')),
                array('data' => $db_row->donations_average, 'class' => array('donations_average')),
                array('data' => $db_row->online_total, 'class' => array('online_total')),
                array('data' => $offline, 'class' => array('total_offline')),
                array('data' => $db_row->offline_pending_count, 'class' => array('offline_pending_count')),
                array('data' => $db_row->total_total, 'class' => array('total_total')),
            )    
        );
    }
    
    echo theme('table', array('header' => $headers, 'rows' => $rows));
