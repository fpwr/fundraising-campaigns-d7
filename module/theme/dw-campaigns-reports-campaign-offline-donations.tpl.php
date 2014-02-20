<?php
// $campaign & $form & $donations
if($mode == 'SCREEN') {
    $form_id                    = 'dw_campaigns_reports_campaign_offline_donations_form';
    $form_build_id              = 'form-'. md5(uniqid(mt_rand(), true));
    $form['#build_id']          = $form_build_id;
    if(count($_POST) > 0) {
        $form['#post']          = $_POST;
    }

    $form_state                 = array('storage' => NULL, 'submitted' => FALSE);
    $form_state         = array('storage' => NULL, 'submitted' => FALSE, 'rebuild' => FALSE, 'cache' => FALSE, 'method' => 'post');

    drupal_prepare_form($form_id, $form, $form_state);
    drupal_process_form($form_id, $form, $form_state);
}


    $states         = _dw_civicrm_pseudoconstant_stateprovince();

    $rows = array();
    foreach($donations as $donation) {

        $address    = $donation->address_1 . " " . $donation->city . " " . $states[$donation->state] . " " . $donation->postal_code;
        $chkbox     = drupal_render($form['donations'][$donation->offline_id]);

        if($donation->contribution_id != -1) {
            $chkbox = '';
        }

        $status =  ($donation->contribution_id != -1) ? t('Imported') : ($donation->deleted != 1 ? t('Not Imported') : t('Deleted'));

        $edit_link = sprintf('<a href="/dw/offline/%d/edit?destination=%s">Edit</a>', $donation->offline_id, $_GET['q']);

        $rows[] = array(
            'data' => array(
                array('data' => $chkbox, 'class' => array('checkbox')),
                /* array('data' => $donation->first_name . ' ' . $donation->last_name, 'class' => array('donor-name')), */
                array('data' => $donation->first_name, 'class' => array('donor-first-name')),
                array('data' => $donation->last_name, 'class' => array('donorr-last-name')),
                array('data' => $donation->email, 'class' => array('email')),
                //array('data' => $address, 'class' => array('full-address')),
                array('data' => $donation->address_1, 'class' => array('street-address')),
                array('data' => $donation->city, 'class' => array('city')),
                array('data' => $states[$donation->state], 'class' => array('state')),
                array('data' => $donation->postal_code, 'class' => array('postal')),

                array('data' => $donation->payment_check_number, 'class' => array('check-number')),
                array('data' => $donation->donation_amount, 'class' => array('donation-amount')),
                array('data' => $donation->pcp_id, 'class' => array('pcp-id')),
                array('data' => ($donation->include_in_honor_roll) ? t('YES') : t('NO') , 'class' => array('honorroll')),
                /*array('data' => $status, 'class' => array('status')),*/
                array('data' => $donation->trxn_id, 'class' => array('trxnid')),
                array('data' => $donation->non_deductible , 'class' => array('nondeductible')),
                array('data' => $edit_link, 'class' => array('action')),
            )    
        );
    } 


if($mode == 'SCREEN') {
    echo drupal_render($selform);
?>

     <br><br>
    <form class="offline-donations" action="<?php echo request_uri(); ?>" method="post">
        <?php
            //echo drupal_render($form['actions']); 
            echo theme('table', array('header' => $headers, 'rows' => $rows));
            echo drupal_render_children($form);
        ?>
    </form>
<?php
} else {

// CSV
        global $dw_campaign_module_path;

        $date = date("Y-m-d_His");

        $outfile = 'report-offline_donations-' . $campaign->nid . '-' . $date . '.csv';

        $file    = $dw_campaign_module_path . "/civi_cache/" . $outfile;

        $fp = fopen($file, "w");

        $csv_header = array();

        foreach($headers as $header) {
            if(isset($header['data'])) {
                $csv_header[] = $header['data']; 
            }
        }

        array_shift($csv_header);
        array_pop($csv_header);

        fputcsv($fp, $csv_header);

        foreach($rows as $row) {

            if(isset($row['data'])) {

                $csv_data   = array();

                foreach($row['data'] as $r) {
                    if(isset($r['data'])) {
                        $csv_data[] = $r['data'];
                    }
                }

                if(count($csv_data) > 0) {
                    array_shift($csv_data);
                    array_pop($csv_data);

                    fputcsv($fp, $csv_data);
                }
            }
        }

        fclose($fp);

        $fsize   = filesize($file);

        header("Pragma: public"); // required
        header("Expires: 0");
        header("Cache-Control: must-revalidate, post-check=0, pre-check=0");
        header("Cache-Control: private",false); // required for certain browsers
        header("Content-Type: application/force-download");
        header("Content-Disposition: attachment; filename=\"$outfile\"");
        header("Content-Transfer-Encoding: binary");
        header("Content-Length: $fsize");
        header("Content-type: text/csv");
        echo file_get_contents($file);
        die;
}
