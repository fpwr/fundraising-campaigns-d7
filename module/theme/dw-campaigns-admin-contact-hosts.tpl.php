<?php

    $headers        = array(
        array(
            'data'  => t('Type'),
        ),
        array(
            'data'  => t('Target Filter'),
        ),
        array(
            'data'  => t('Date Offset'),
        ),
        array(
            'data'  => t('Action Date'),
        ),
        array(
            'data'  => t('Subject'),
        ),
        array(
            'data'  => t('From Name'),
        ),
        array(
            'data'  => t('Edit'),
        ),
    );

$result = db_query("select * from {dw_campaigns_scheduled_host_contacts} where deleted=0");
$rows = array();

foreach($result as $row) {
    if(is_null($row->target_filter) || empty($row->target_filter)) {
        $row->target_filter = 'all';
    }

    $actions = '<a href="/admin/dw/contact_hosts/' . $row->eid . '">Edit</a> <a href="/admin/dw/contact_hosts/' . $row->eid . '/delete" onclick="return confirm(\'Do you really want to delete this notification?\');">Delete</a>';
    $rows[] =   array(
                    'data' => array(
                        array('data' => $row->schedule_type, 'class' => array('type')),
                        array('data' => $row->target_filter, 'class' => array('target-filter')),
                        array('data' => $row->day_offset, 'class' => array('offset')),
                        array('data' => $row->action_date, 'class' => array('action-date')),
                        array('data' => $row->email_subject, 'class' => array('subject')),
                        array('data' => $row->from_name, 'class' => array('from-name')),
                        array('data' => $actions, 'class' => array('edit-action')),
                    )
                );
}

echo theme('table', array('header' => $headers, 'rows' => $rows));
?>
<br><br>
<h2>CONTACT</h2>
<?php
echo drupal_render($form);
?>
