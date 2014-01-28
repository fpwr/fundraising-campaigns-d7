<?php

    $headers = array();
    $nodes = array();

    $headers        = array(
        array(
            'data'  => t('Deliver By Date'),
        ),
        array(
           'data'  => t('Location'),
        ),
        array(
            'data'  => t('Event Date'),
        ),
        array(
            'data'  => t('Shipping Address'),
        ),
        array(
            'data'  => t('Total Number Shirts Needed'),
        ),
        array(
            'data'  => t('Size Requests'),
        ),
        array(
            'data'  => t('Order Status'),
        )
    );

    $results = $rows;

    $rows = array();

    foreach($results as $db_row) {

        if($db_row->shipping_status == 0) {
            $status = t('Pending') . ' <a onclick="return confirm(\'Are you sure you want to mark this as ordered?\');" href="/admin/reports/dw_shirtorders/' . $db_row->nid . '/set-ordered">Mark As Ordered</a>';
        } else {
            $status = t('Ordered');
        }

        $rows[] = array(
            'data' => array(
                array('data' => $db_row->shirt_latest_delivery_date, 'class' => array('deliver_date')),
                array('data' => $db_row->node_title,                 'class' => array('location')),
                array('data' => $db_row->shirt_event_date,           'class' => array('event_date')),
                array('data' => $db_row->shirt_shipping_address,     'class' => array('shipping_address')),
                array('data' => $db_row->shirt_quantity,             'class' => array('total_shirts')),
                array('data' => $db_row->shirt_special_requests,     'class' => array('size_requests')),
                array('data' => $status,                             'class' => array('order_status')),
            )    
        );

    }

    echo theme('table', array('header' => $headers, 'rows' => $rows));
