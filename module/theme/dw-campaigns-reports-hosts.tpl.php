<?php

    $headers = array();
    $nodes = array();
    $rows = array();

    $headers        = array(
        array(
            'data'  => t('Name'),
            'field' => 'name',
        ),
        array(
           'data'  => t('email'),
            'field' => 'email',
        ),
        array(
            'data'  => t('State'),
            'field' => 'state',
        ),
        array(
            'data'  => t('Event Name'),
            'field' => 'event_name',
            'sort'  => 'asc',
        ),
        array(
            'data'  => t('country group'),
            'field' => 'country_group',
        )
    );

    $num_per_page   = 10000;
    $table          = "hosts_as_$num_per_page";

    $query = "
        CREATE TEMPORARY TABLE
            $table
            (
                name char(255),
                email char(255),
                state char(255),
                event_name char(255),
                country_group char(255)
            ) ENGINE = MYISAM CHARACTER SET utf8 COLLATE utf8_unicode_ci
    ";
    db_query($query);

    $query = db_insert($table)->fields(array('name', 'email', 'state', 'event_name', 'country_group'));

    foreach($hosts as $host) {
        $campaigns = dw_campaigns_host_find_campaigns($host->uid, FALSE);

        //$name       = $host->title;

        $name       = isset($host->field_dw_host_name['und']['0']['value']) ? $host->field_dw_host_name['und']['0']['value'] : '';
        $email      = isset($host->field_dw_host_email['und']['0']['value']) ? $host->field_dw_host_email['und']['0']['value'] : '';
        $city       = isset($host->field_dw_host_city['und']['0']['value']) ? $host->field_dw_host_city['und']['0']['value'] : '';
        $state      = isset($host->field_dw_host_province['und']['0']['value']) ? $host->field_dw_host_province['und']['0']['value'] : '';
        

        foreach($campaigns as $campaign_id) {
            if(!isset($nodes[$campaign_id])) {
                $nodes[$campaign_id] = node_load($campaign_id);
            }

            $node       = $nodes[$campaign_id];
            $country    = isset($node->field_dw_country_grouping['und']['0']['value']) ? $node->field_dw_country_grouping['und']['0']['value'] : '';

            $query->values(array(
                'name'              => $name,
                'email'             => $email,
                'state'             => $state,
                'event_name'        => $node->title,
                'country_group'     => $country,
            ));
        }
    }

    $query->execute();

    $result = db_select($table)->fields($table)->extend('PagerDefault')->limit($num_per_page)->extend('TableSort')->orderByHeader($headers)->execute();

    $i  = 0;

    foreach($result as $db_row) {
        $i++;

        $rows[] = array(
            'data' => array(
                array('data' => $db_row->name, 'class' => array('name')),
                array('data' => $db_row->email, 'class' => array('email')),
                array('data' => $db_row->state, 'class' => array('state')),
                array('data' => $db_row->event_name, 'class' => array('event_name')),
                array('data' => $db_row->country_group, 'class' => array('country')),
            )    
        );

        // no point in looping if we've had enough
        if($i==$num_per_page) {
            break;
        }
    }

    echo theme('table', array('header' => $headers, 'rows' => $rows));
