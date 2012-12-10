<?php
$mode_type = dw_campaigns_get_selected_type();

if($mode_type != 'derby')
    return;

$default_image = '/sites/all/themes/dw_campaigns_' . $mode_type . '/images/no-image.gif';
?>
<div class="derby-footer">
    <ul>
        <li>
        <?php 
            $startyear  = 2011;
            $endyear    = '';
            $t_endyear  =date('Y');
            if($startyear != $t_endyear) {
                $endyear = '-' . $t_endyear;
            } 

            echo t('(c)  !startyear!endyear PWSACO Benefitting Prader-Willi Syndrome Association of Colorado', array('!startyear' => $startyear, '!endyear' => $endyear)); 
        ?>
        </li>
        <li class="contact-us">
            <a href="#"><?php echo t('Contact Us'); ?></a>
        </li>
        <li class="facebook">
            <a href="http://facebook.com"><img src="/sites/all/themes/dw_campaigns_derby/images/footerFacebook.jpg"><span><?php echo t('Connect with us on Facebook'); ?></span></a>
        </li>
        <li class="twitter">
            <a href="http://twitter.com"><img src="/sites/all/themes/dw_campaigns_derby/images/footerTwitter.jpg"><span><?php echo t('Follow us on Twitter'); ?></span></a>
        </li>
    </ul>
</div>
<div class="defaultfbpic">
<img src="<?php echo $default_image;?>">
</div>
