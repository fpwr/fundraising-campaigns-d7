<?php
    $headers        = array(
        array(
            'data'  => t('Delete?'),
        ),
        array(
            'data'  => t('Current File'),
        ),
        array(
            'data'  => t('Description'),
        ),
        array(
            'data'  => t('Select New File'),
        ),
    );

    $form_id                    = 'dw_campaigns_user_host_manage_files_form';
    $form_build_id              = 'form-'. md5(uniqid(mt_rand(), true));
    $form['#build_id']          = $form_build_id;
    if(count($_POST) > 0) {
        $form['#post']          = $_POST;
    }

    //$form_state                 = array('storage' => NULL, 'submitted' => FALSE);
    $form_state                 = array('storage' => NULL, 'submitted' => FALSE, 'rebuild' => FALSE, 'cache' => FALSE);

    drupal_prepare_form($form_id, $form, $form_state);
    drupal_process_form($form_id, $form, $form_state);

    $rows = array();
    for($delta=0; $delta<5; $delta++) {

        foreach($form['event_files'][$delta] as $key => $item) {
            if(isset($item['#title'])) {
                $form['event_files'][$delta][$key]['#title'] = ''; 
            }
        }

        $chkbox         = drupal_render($form['event_files'][$delta]['do_delete']);
        $current        = drupal_render($form['event_files'][$delta]['old_file']);
        $description    = drupal_render($form['event_files'][$delta]['description']);
        $new_file       = drupal_render($form['event_files'][$delta]['new_file_' . $delta]);


        $rows[] = array(
            'data' => array(
                array('data' => $chkbox,      'class' => array('checkbox')),
                array('data' => $current,     'class' => array('current')),
                array('data' => $description, 'class' => array('description')),
                array('data' => $new_file,    'class' => array('new_file')),
            )    
        );
    } 

?>

     <br><br>
    <form class="host-manage-files" action="<?php echo request_uri(); ?>" method="post" enctype="multipart/form-data">
        <?php
            echo theme('table', array('header' => $headers, 'rows' => $rows));
            echo drupal_render_children($form);
        ?>
    </form>
