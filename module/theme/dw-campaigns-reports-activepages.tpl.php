<?php

function pcp_sort($a, $b) {
    return strcmp($a->title, $b->title);
}

//  'variables' => array('selform' => NULL, 'headers' => array(), 'data' => array())

    $rows = array();

    foreach($data as $pcp) {
        $campaign = $campaigns[$pcp->contribution_page_id];

        $params = array(
            'contact_id'    => $pcp->contact_id,
            'returnFirst'   => 1
        );

        $pcp_contact    = _dw_civicrm_contact_get($params);

        $drupal_id      = _dw_campaigns_contact_id_get_user($pcp->contact_id);
        $fake_user      = dw_campaigns_cache_simple_user_data($drupal_id);
        $url            = dw_campaigns_user_get_pcp_url($fake_user, $campaign);

        $link = sprintf('<a href="%s">%s, %s</a>', $url, $pcp_contact->display_name, $campaign->title);

        $rows[] = array(
            'data' => array(
                array('data' => $pcp->title, 'class' => array('pcp-title')),
                array('data' => $link, 'class' => array('fundraiser-link')),
            )
        );
    } 

    uasort($rows, 'pcp_sort');

    echo drupal_render($selform); 
    echo theme('table', array('header' => $headers, 'rows' => $rows));
