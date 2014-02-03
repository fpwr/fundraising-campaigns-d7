<a href="/admin/reports/dw_eventinsurance/csv">Download CSV</a>
<?php

    $headers = array();
    $nodes = array();

    $headers        = array(
        array(
            'data'  => t('Host Name'),
        ),
        array(
           'data'  => t('Location'),
        ),
        array(
            'data'  => t('Event Date'),
        ),
        array(
            'data'  => t('# PCPs'),
        ),
        array(
            'data'  => t('Name of Additional Insured'),
        ),
        array(
            'data'  => t('Address of Additional Insured'),
        ),
        array(
            'data'  => t('Status'),
        )
    );

    $results = $rows;

    $rows = array();

    foreach($results as $db_row) {

        $status = 'Pending';
/*
        if($db_row->shipping_status == 0) {
            $status = t('Pending') . ' <a onclick="return confirm(\'Are you sure you want to mark this as ordered?\');" href="/admin/reports/dw_shirtorders/' . $db_row->nid . '/set-ordered">Mark As Ordered</a>';
        } else {
            $status = t('Ordered');
        }
*/

        $record_class = ($db_row->has_record) ? "has_record" : "no_record";
        $location = !empty($db_row->ins_location) ? $db_row->ins_location : $db_row->node_title;
//var_dump($db_row);die;
        $rows[] = array(
            'data' => array(
                array('data' => $db_row->host_names,     'class' => array('host_names')),
                array('data' => $location,               'class' => array('location')),
                array('data' => $db_row->ins_event_date, 'class' => array('event_date')),
                array('data' => $db_row->pcp_count,      'class' => array('pcp_count')),
                array('data' => $db_row->ins_ai_name,    'class' => array('ai_name')),
                array('data' => $db_row->ins_ai_address, 'class' => array('ai_address')),
                array('data' => $status,                 'class' => array('insurance_status')),
            ), 
            'class' => array($record_class)
        );

    }

    echo theme('table', array('header' => $headers, 'rows' => $rows));
