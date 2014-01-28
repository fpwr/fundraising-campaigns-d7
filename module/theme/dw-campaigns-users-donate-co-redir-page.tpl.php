<html>
<body>
<?php

$q = $_GET['q'];
$pieces = explode("/", $q);

$hash = $data;
$result = db_query("select * from {dw_campaigns_checkout_swap} where hash = :hash", array(':hash' => $hash));

if($result->rowCount() == 0) {
    drupal_set_message(t("Unable to find user information for this donation, please try again"), "error");

    array_pop($pieces);
    $res = implode("/", $pieces);
    drupal_goto($res);
    return;

}
    
array_pop($pieces);
$cancel = implode("/", $pieces);
array_pop($pieces);
$return = implode("/", $pieces) . '/donated';

$row = $result->fetchObject();
$values = unserialize($row->data);

$pp = array(
    'currency' => 'EUR',
    'business' => 'L8XWDK9MLMWME',
    'title'    => 'FPWR France Donation',
);

// FIXME - this should not be a static list - paypal is no longer a one even one off so refactor the solution
$french = array(
    '197' => '197',
    '340' => '340',
    '753' => '753',
    '762' => '762',
    '765' => '765',
    '768' => '768',
    '774' => '774',
    '783' => '783',
    '819' => '819'
);

$uk = array(
  '871' => '871',
  '874' => '874'
);

$campaign = $values['campaign'];

if(!isset($french[$campaign])) {
    if(!isset($uk[$campaign])) {
        $pp = array(
            'currency' => 'NZD',
            'business' => '8NHDV9M9JNUM2',
            'title'    => 'PWS NZ Donation',
        );
    } else {
        $pp = array(
            'currency' => 'GBP',
            'business' => 'XPTLFJZBUTHFW',
            'title'    => 'PWS UK Donation',
        );
    }
}

/*

    drupal_add_js('
      (function($) {
        $(document).ready(function() {
          $("#paypal_button").click();
        });
     })(jQuery);
    ', 'inline');
*/
    $words = t("You will be redirected to PayPal to complete your donation, if you are not redirected in the next 5 seconds please click the button below");
?>	
<h2><?php echo $words;?></h2>
<!-- <form id="paypal" method="POST" action="https://www.sandbox.paypal.com/cgi-bin/webscr"> -->
<form id="paypal" method="POST" action="https://www.paypal.com/cgi-bin/webscr">
	<input type="hidden" value="<?php echo $values['donation-amount'];?>" name="amount">
	<input type="hidden" value="_donations" name="cmd">
	<!-- <input type="hidden" value="6FYY3SJWRGFNJ " name="business"> -->
	<input type="hidden" value="<?php echo $pp['business'];?>" name="business">
	<input type="hidden" value="<?php echo $pp['title'];?>" name="item_name">
	<input type="hidden" value="<?php echo $pp['currency'];?>" name="currency_code">
	<input type="hidden" value="1" name="no_shipping">
	<input type="hidden" value="0" name="tax">
	<input type="hidden" value="2" name="rm">
	<input type="hidden" value="http://onesmallstep.fpwr.org/<?php echo $return;?>" name="return">
	<input type="hidden" value="http://onesmallstep.fpwr.org/<?php echo $cancel;?>" name="cancel_return">
	<input type="hidden" value="http://onesmallstep.fpwr.org/dw/co/pp_ipncatch" name="notify_url">
	<input type="hidden" name="custom" value="<?php echo $hash;?>">
	<input type="hidden" value="co_transaction_code" name="on1">
	<input type="hidden" value="<?php echo $hash;?>" name="os1">
	<!-- <input type="image" id="paypal_button" border="0" alt="PayPal - The safer, easier way to pay online" src="https://www.paypal.com/en_US/i/btn/btn_donateCC_LG.gif" name="submit"> -->
	<input type="submit" id="paypal_button" border="0" name="submitbtn" value="Go to Paypal">
</form>

<script type="text/javascript"> document.forms["paypal"].submit(); </script> 
</body>
</html>
