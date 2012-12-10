<?php
    $res = dw_campaigns_get_contribution_total_for_pcp($pcp);
    $pcpTotal     = $res['total'];
    $count        = $res['count'];

    $goalTotal    = $pcp->goal_amount;
    $goalProgress = $pcpTotal;

    if($goalTotal == 0) {
        return;
    }

    // I assume we want to round 99.9 down so that we don't say 100% too soon
    $goalPercent	= floor($goalProgress/$goalTotal * 100);
    
    if($goalPercent > 100) {
        $goalPercent    = 100;
    }
    
    $goalRemaining  = dw_campaigns_force_decimal($goalTotal - $goalProgress, $campaign->field_dw_currency['und']['0']['value']);
    $goalTotal      = dw_campaigns_force_decimal($goalTotal);
    $goalProgress   = dw_campaigns_force_decimal($goalProgress);

    if($goalRemaining < 0) {
        $goalRemaining		= '0.00';
    }

    $goal_data = array();
    $goal_data['goalRemaining'] = $goalRemaining;
    $goal_data['goalTotal']     = $goalTotal;
    $goal_data['goalProgress']  = $goalProgress;
    $goal_data['goalPercent']   = $goalPercent;


    dw_campaigns_set_goal_results($goal_data);
?>

<div class="thermoEmpty">
	<div class="thermoFull" style="height: <?php echo $goalPercent; ?>%">

	</div>
</div>


<ul class="stats">
<?php
if(!is_null($campaign)) {
?>
	<li class="location-preamble">
		<span class="preamble-label"></span>
	</li>
	<li class="location-raised">
		<span class="location-label"><?php //echo $campaign->field_dw_campaign_location['und']['0']['value']; ?></span>
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
