<?php

    $num_per_page = 99999;

    $table = "{event_registration_report_as_$num_per_page}";

    $raw_headers    = $headers;
    $raw_rows       = $rows;

if(count($raw_rows) < 0) {
    echo 'No event participants found';
    return;

} else {
    $fields         = NULL;
   
    if(isset($raw_rows[0]) && !is_null($raw_rows[0])) { 
        $fields         = array_keys($raw_rows[0]);
    }

    $headers = array();
    foreach($raw_headers as $value) {

        $column     = NULL;
        if(!is_null($fields)) {
            $column     = array_shift($fields);
        }

        $row        = array('data' => t($value), 'field' => $column);
        $headers[]  = $row; 
    }

    $query = "
        CREATE TEMPORARY TABLE
            $table
            (
                location char(255),
                first_name char(255), 
                last_name char(255), 
                display_name char(255), 
                email char(255), 
                participant_type char(255), 
                participant_children int,
                participant_adults int,
                currency char(255), 
                total_donations float,
                total_offline float
            )
    ";
    db_query($query);
  
    $query = db_insert($table)->fields(array('location', 'first_name', 'last_name', 'display_name', 'email', 'participant_type', 'participant_children', 'participant_adults', 'currency', 'total_donations', 'total_offline'));

    foreach($raw_rows as $row) {
        $query->values($row);
    }
    $query->execute();

    $result = db_select($table)->fields($table)->extend('PagerDefault')->limit($num_per_page)->extend('TableSort')->orderByHeader($headers)->execute();

    $rows = array();

    $i=0; 
    foreach($result as $db_row) {
        $i++;

        $rows[] = array(
            'data' => array(
                array('data' => $db_row->location, 'class' => array('location')),
                array('data' => $db_row->first_name, 'class' => array('first_name')),
                array('data' => $db_row->last_name, 'class' => array('last_name')),
                array('data' => $db_row->display_name, 'class' => array('display_name')),
                array('data' => $db_row->email, 'class' => array('email')),
                array('data' => $db_row->participant_type, 'class' => array('participant_type')),
                array('data' => $db_row->participant_children, 'class' => array('participant_children')),
                array('data' => $db_row->participant_adults, 'class' => array('participant_adults')),
                array('data' => $db_row->currency, 'class' => array('currency')),
                array('data' => $db_row->total_donations, 'class' => array('total_donations')),
                array('data' => $db_row->total_offline, 'class' => array('total_offline')),
            )    
        );

        if($i==$num_per_page) {
            break;
        }
    }
    
    echo drupal_render($form);
    echo theme('table', array('header' => $headers, 'rows' => $rows));
    echo theme('pager');
}
