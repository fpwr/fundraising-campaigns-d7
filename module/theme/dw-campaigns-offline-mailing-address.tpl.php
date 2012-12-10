<?php
$country = isset($campaign->field_dw_country_grouping['und']['0']['value']) ? $campaign->field_dw_country_grouping['und']['0']['value'] : '';
if(empty($country)) {
    $country = 'us';
    echo "we defaulted to US<br>";
}

$mailing_address    = dw_campaigns_event_countries_resolve_selection($country);

$mailing_body       = isset($mailing_address->field_dw_country_mailing_address['und']['0']['safe_value']) ? $mailing_address->field_dw_country_mailing_address['und']['0']['safe_value'] : 'No current mailing information for this country, please contact an administrator';
?>
<div class="offline-mailing-address">
    <h2 class="please-mail">Please mail checks to:</h2>
    <h2><?php echo $mailing_address->title; ?>:</h2>
    <div class="offline-mailing-address-content">
        <?php echo $mailing_body; ?>    
    </div>
</div>
