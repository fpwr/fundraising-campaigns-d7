<a href="/admin/reports/dw_event_emails/<?= $selected ?>/CSV">Download CSV</a>
<?php

    $raw_headers    = $headers;
    $raw_rows       = $rows;

    if(count($raw_rows) < 0) {
        echo 'No emails found';
        return;
    
    } else {
    
        $headers        = array(
            array(
                'data'  => t('First Name'),
            ),
            array(
                'data'  => t('Last Name'),
            ),
            array(
                'data'  => t('Email'),
            ),
        );
    
        echo drupal_render($form);
        echo theme('table', array('header' => $headers, 'rows' => $rows));
    }
