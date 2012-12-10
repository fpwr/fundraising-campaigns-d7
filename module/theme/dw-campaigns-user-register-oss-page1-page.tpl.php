<h2>
    <span class="register-account"><?php echo t('Event Registration');?></span>
<?php
    $mode_type = dw_campaigns_get_selected_type();
    if($mode_type == 'walking') {
        $words  = t(' - Step 1 of 2');
    } else {
        $words  = '';
    }
?>
<span class="step-of"><?php echo $words; ?></span>
</h2>
<?php echo render($registerForm); ?>
<div id="otherdiv"></div>
