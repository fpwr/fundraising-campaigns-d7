<?php

$mode_type = dw_campaigns_get_selected_type();

if($mode_type != 'walking')
    return;

$default_image = '/sites/all/themes/dw_campaigns_' . $mode_type . '/images/no-image.gif';

$hide_fireworks = FALSE;
if(isset($_SERVER['HTTP_USER_AGENT']) && strstr($_SERVER['HTTP_USER_AGENT'], "facebookexternalhit") !== false) {
    $hide_fireworks = TRUE;
}

global $dw_campaign_module_path;
?>

<div class="defaultfbpic">
<img src="<?php echo $default_image;?>">
</div>
<div class="walking-footer">
    <ul>
        <li class="copyright">
        <?php echo t('(c)  2011 Foundation for Prader-Willi Research.  All Rights Reserved'); ?>
        </li>
        <li class="contact-us">
            <a href="/node/61"><?php echo t('Contact Us'); ?></a>
        </li>
        <li class="facebook">
            <a href="http://www.facebook.com/pages/The-Foundation-for-Prader-Willi-Research/78626677947"><img src="/sites/all/themes/dw_campaigns_walking/images/footerFacebook.jpg"><span><?php echo t('Connect with us on Facebook'); ?></span></a>
        </li>
        <li class="twitter">
            <a href="http://twitter.com/#!/fpwr"><img src="/sites/all/themes/dw_campaigns_walking/images/footerTwitter.jpg"><span><?php echo t('Follow us on Twitter'); ?></span></a>
        </li>
    </ul>
</div>
</div>
<?php  if(!$hide_fireworks) { ?>
<link rel="stylesheet" type="text/css" href="/<?php echo $dw_campaign_module_path;?>fireworks/style/fireworks.css" media="screen" />
<div id="fireworks-template">
 <div id="fw" class="firework"></div>
 <div id="fp" class="fireworkParticle"><img src="/<?php echo $dw_campaign_module_path;?>fireworks/image/particles.gif" alt="" /></div>
</div>
<?php } ?>

