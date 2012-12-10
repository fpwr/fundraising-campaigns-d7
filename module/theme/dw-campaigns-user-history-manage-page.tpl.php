<h2>
<?php 
$mode_type   = dw_campaigns_get_selected_type();
if($mode_type != 'walking') {
    echo $campaign->title;
} else {
    echo $pcp->title; 
}
?>
</h2>
<?php
global $user;

$owner_user = $user;

theme('dw_campaigns_derby_pcp_statistics', array('thisUser' => $owner_user, 'campaign' => $campaign, 'pcp' => $pcp));

$goal_data = dw_campaigns_get_goal_results();
?>

<div class="old-goals">
    <ul>
        <li><?php echo $pcp->title; ?></li>
        <li><?php echo t('Goal: !goal', array('!goal' => $goal_data['goalTotal'])); ?></li>
        <li><?php echo t('Total Raised: !progress', array('!progress' => $goal_data['goalProgress'])); ?></li>
        <li><?php echo t('Percent: !percent %', array('!percent' => $goal_data['goalPercent'])); ?></li>
        <li>: <?php // echo $goal_data['goalRemaining']; ?></li>
    </ul>
</div>
<?php
echo theme('dw_campaigns_derby_pcp_supporters', array('thisUser' => $owner_user, 'campaign' => $campaign, 'pcp' => $pcp, 'num_per_page' => 50, 'show_return' => FALSE));
