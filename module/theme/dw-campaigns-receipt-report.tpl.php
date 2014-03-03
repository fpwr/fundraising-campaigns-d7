<?php
//var_dump($receiptData);
$fields = array(
    'contact-display_name'           => 'Contributor Name',
    'contact-email'                  => 'Contributor Email',
    'contribution-total_amount'      => 'Donation Amt',
    'contribution-receive_date'      => 'Date',
    'contribution-invoice_id'        => 'Long Invoice Id',
    'contribution-receipt_id'        => 'Receipt Id',
    'contribution-contribution_type' => 'Campaign',
    'pcp-title'                      => 'PCP Title',
    'pcp_contact-display_name'       => 'PCP Owner',
    'pcp_contact-email'              => 'PCP Owner Email',
);
?>
<div class="receipt_report_container">
    <ul class="fieldlist">
<?php
    foreach($fields as $fieldname => $fieldnameprint) {
        list($key, $subkey) = explode("-", $fieldname, 2);
        $tmp_fieldname = ucfirst(str_replace(array('-', '_'), array('', ''), $fieldname));
        $fieldname_wd = !is_null($fieldnameprint) ? $fieldnameprint : $tmp_fieldname;
?>
        <li class="<?php echo $fieldname; ?>"><span class="fieldname"><?php echo $fieldname_wd;?></span> : <span class="fieldvalue"><?php echo $receiptData[$key]->$subkey; ?></span>
<?php
    }
?>
    </ul>
    <ul class="links">
        <li><a href="/admin/reports/dw_receipts/<?php echo $receiptData['contribution']->contribution_id;?>/pdf">Download PDF Receipt</a></li>
        <li><a href="/admin/reports/dw_receipts/<?php echo $receiptData['contribution']->contribution_id;?>/email">Email PDF Receipt</a></li>
    </ul>
</div>
