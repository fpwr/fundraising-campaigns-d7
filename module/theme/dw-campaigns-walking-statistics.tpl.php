<?php

    if(dw_campaigns_get_selected_type()!='walking')
        return;

    $campaign = dw_campaigns_get_selected_campaign();
    
    if(!is_null($campaign)) {
        $res            = _dw_campaigns_campaign_total($campaign);   
        $currency       = $campaign->field_dw_currency['und']['0']['value'];
    } else {
        $res            = _dw_campaigns_campaigns_total();
        $currency	= 'USD';
    }

    $goalTotal      = $res['goal'];

    if(arg(2) != 'location') {    
        $goalAlt        = variable_get('dw_campaigns_fundraising_goal_override', '0');
        if($goalAlt > 0) { 
           $goalTotal   = $goalAlt;
        }
    }

    if($currency == 'MULTI') {
        $convert_to = isset($campaign->field_dw_therm_currency['und']['0']['value']) ? isset($campaign->field_dw_therm_currency['und']['0']['value']) : 'USD';

        if($convert_to != 'USD') {
            $goalProgress   = dw_campaigns_convert_from_usd($convert_to, $res['raised']);
            $currency = $convert_to;
        } else {
            $goalProgress   = $res['raised'];
            $currency = 'USD';
        } 
    } else {

        $goalProgress   = $res['raised'];

    }

    // currently USD only is calculated on the homepage
    if(isset($res['raised_usd'])) {
        $goalProgress   = $res['raised_usd'];
    }

    if(is_null($campaign)) {
        $goalExtra      = variable_get('dw_campaigns_fundraising_goal_start_value', '0');
        $goalProgress   += $goalExtra;
    }

    if(!is_numeric($goalProgress)) {
        $goalProgress = '0.0';
    }

    if($goalTotal === 0) {
        drupal_set_message(t('Unable to load "campaigns" goals'), 'error');
        return;
    }

    if($goalTotal <= 0 || !is_numeric($goalTotal)) {

        $goalPercent = 100;

    } else {
        // I assume we want to round 99.9 down so that we don't say 100% too soon
        $goalPercent                    = floor($goalProgress/$goalTotal * 100);
        if($goalPercent > 100) {
            $goalPercent = 100;
        }
    } 

    $goalRemaining                  = dw_campaigns_force_decimal($goalTotal - $goalProgress);
    //$goalTotal                      = dw_campaigns_force_k($goalTotal);
    $goalTotal_raw                  = $goalTotal;
    $goalTotal                      = dw_campaigns_force_decimal($goalTotal, $currency);
    $goalProgress_raw               = $goalProgress;
    $goalProgress                   = dw_campaigns_force_decimal($goalProgress, $currency);
    
    if($goalRemaining < 0) {
        $goalRemaining              = 0;
    }


    //if($goalTotal > 500000 || $_SERVER['REMOTE_ADDR'] == '67.177.136.8') {
    if(1==1 && ($goalProgress_raw > $goalTotal_raw || isset($_REQUEST['t']))) {
        drupal_add_js(drupal_get_path('module', 'dw_campaigns') . '/fireworks/script/fireworks.js');
        drupal_add_js('
          var runCnt = 0;
          function doFW() {
              runCnt++;
              if(runCnt<30) {
                  createFirework(66,191,6,4,null,null,null,null,false,true);
              }
          }
          //setInterval("createFirework(66,191,6,4,null,null,null,null,false,true)", 2000);
          //setInterval("createFirework(66,191,6,4,null,null,null,null,false,true)", 4500);
          setInterval("doFW()", 2000);
          setInterval("doFW()", 4500);
        ', 'inline');
    }
?>
<div class="thermoEmpty">
	<div class="thermoFull" style="height: <?php echo $goalPercent; ?>%">

	</div>
</div>


<ul class="stats">
<?php
if(!is_null($campaign)) {
?>
	<li class="location-raised">
		<span class="location-label"><?php echo $campaign->field_dw_campaign_location['und']['0']['value']; ?></span>
	</li>
<?php
}
?>
	<li class="have-raised">
		<span class="dollar-label"><?php echo t('We have raised'); ?></span>
		<span class="dollar-amount"><?php echo $goalProgress; ?></span>
	</li>
	<li class="toward-goal">
		<span class="dollar-label"><?php echo t('Toward Our Goal of:'); ?></span>
		<span class="dollar-amount"><?php echo substr($goalTotal, 0, strlen($goalTotal) -3); ?></span>
	</li>
</ul>
<?php if(arg(2) == 'location') { ?>
<div class="sponsors" style="margin-top:218px;width:258px;position:relative;left:-8px;padding-left:8px;padding-top:14px;padding-bottom:14px;background:#ffffff;">
    <div class="sponsor-top">
        <img  alt="" width="232" src="/sites/default/files/scotia.png" style="margin-left:8px"/>
    </div>
    <div class="sponsor-left" style="display:inline-block;width:120px;vertical-align:top;">
        <img  alt="" width="116" src="/sites/default/files/RFHK_Logo_Vert.png" />
        <img  alt="" width="116" src="/sites/default/files/kongsberg_logo.png" />
    </div>
    <div class="sponsor-right" style="display:inline-block;width:120px;vertical-align:top;padding-left:14px;">
        <img  alt="" width="116" src="/sites/default/files/WIGI_Logo_Master_P.png" />
        <img  alt="" width="116" src="/sites/default/files/print_paperStacked.png" />
        <img  alt="" width="116" src="/sites/default/files/arc_financial_updated.png" />
        <img  alt="" width="116" src="/sites/default/files/wu_logo_mmfb.png" />
        <img  alt="" width="116" src="/sites/default/files/global_rehab.png" />
    </div>
</div>
<?php } ?>
