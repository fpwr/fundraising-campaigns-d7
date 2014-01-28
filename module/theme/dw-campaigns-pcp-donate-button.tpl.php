<?php
$donate_url     = dw_campaigns_get_donate_url($thisUser, $campaign);
$donate_text    = t('Donate Now');

$country = dw_campaigns_get_event_country_by_campaign($campaign);
$document = isset($country->field_dw_country_donation_form['und']['0']['uri']) ? $country->field_dw_country_donation_form['und']['0']['uri'] : NULL;

if(!is_null($document)) {
    $document = str_replace("public://", '/sites/default/files/', $document);
}

?>
<!-- <a href="/<?php echo $donate_url;?>" class="donate-button btn"><?php echo $donate_text; ?></a> -->
<a href="/<?php echo $donate_url;?>" class="menu-donate">
    <div class="menu-item-word-outter"><div class="menu-item-word-inner"><?php echo $donate_text; ?></div></div>
</a>

<?php if(!is_null($document)) { ?>
    <div class="donation-form"><a href="<?php echo $document; ?>"><?php echo t('If you are unable to donate online, please print out a donation form.');?></a></div>

    <?php   //JFN - january 15 2014 1826 - [#feature "Select and upload a pdf to replace the currently linked document here.  but only if you're logged in as an admin"]

        if( in_array('site admin', array_values($user->roles)) ) {
            $myForm = drupal_get_form('dw_campaigns_upload_offline_form_form', $document );

            print drupal_render( $myForm );
            //drupal_render( $myForm );

            //drupal_render();
        //    echo '<div class="admin-link"><a href=""> admin->[replace donation form] </a></div>';
        }

    ?>
<?php } ?>