<?php
     $image_params   = array(
        'w'                 => 100,
    );
    $matched_image  = '';
    //get the user and drop it in for a preview
    $imageSrc       = _dw_campaigns_get_photo($thisUser,  $image_params, 'pcp-photo', $pcp->id, $matched_image);
    $dims           =  getimagesize("./" . $imageSrc);

    $imgwidth       = $dims[0];
    $imgheight      = $dims[1];

    $mode_type   = dw_campaigns_get_selected_type();


    $margin_left = (120 - $imgwidth) /2;

    if(arg(2) == 'give' && arg(3) == 'general-donation') {
        echo '<div class="blurb">To make a donation to an individuals fundraising page, please return to the home page and select the location in which the individual is participating.</div>';
    }

    $form_title = t('Donate to @title', array('@title' => $pcp->title));
    if($mode_type != 'walking') {
?>
    <h2><?php echo $form_title;?></h2>
    <img src="<?php echo $imageSrc;?>">

<?php
    } else {
?>
    <div class="donate-form-header" style="padding-top:1px;">
        <div class="image-box" style="inline-block">
        <div class="image-box-inner">
            <img src="<?php echo $imageSrc;?>">
        </div>
        </div>
        <h2><?php echo $form_title;?></h2>
    </div>
<?php
    }

    $include_donation_conf = ($mode_type == 'walking') ? TRUE : FALSE;
    $include_donation_conf = TRUE;

    $form   = drupal_get_form('dw_campaigns_users_donate_co_page_form', $campaign, $pcp, $include_donation_conf);

    echo drupal_render($form); 
?>
<div style="display:none">
	<a href="#hidden-words" id="show-words"></a>
	<div id="hidden-words">
		<span class="please-wait"><?php echo t('Please wait, we are processing your donation.'); ?></span>
		<br>
		<span class="please-wait-extra"><?php echo t('Reloading or navigating away from this page may cause multiple donations'); ?></span>
	</div>
</div>
