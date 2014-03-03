<a href="/admin/reports/active_campaigns/csv">Download CSV</a><br>
<table class="active-campaigns-report">
<tr><th>Campaign Name</th><th>Action</th></tr>
<?php
usort($campaigns, 'by_title');

function by_title($a, $b) {
    return strcmp($a->title, $b->title);
}

foreach($campaigns as $campaign) {
    echo '<tr><td class="campaign-name">' . $campaign->title . '</td><td class="action"><a href="/node/' . $campaign->nid . '/edit" class="btn">Edit</a></td></tr>';

}
?>
</table>
