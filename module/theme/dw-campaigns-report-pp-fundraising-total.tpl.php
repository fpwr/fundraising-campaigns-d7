<?php
    echo '<a href="/' . $filename . '">Download CSV</a>';   

    $result = $rows;

    $rows = array();
 
    foreach($result as $arr_row) {
        $db_row = (object) $arr_row;

        $rows[] = array(
            'data' => array(
                array('data' => $db_row->name, 'class' => array('name')),
                array('data' => $db_row->location, 'class' => array('location')),
                array('data' => $db_row->email, 'class' => array('email')),
                array('data' => $db_row->phone, 'class' => array('phone')),
                array('data' => $db_row->tags, 'class' => array('tags')),
                array('data' => $db_row->total_total, 'class' => array('total_total')),
                array('data' => $db_row->confirmed_total, 'class' => array('confirmed_total')),
            )    
        );
    }
    
    echo theme('table', array('header' => $headers, 'rows' => $rows));
