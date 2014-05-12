<?php 
if(isset($campaign->nid) && isset($campaign->title)) {
?>

<a href="/dw/walking/location/<?php echo $campaign->nid;?>">Return to <?php echo $campaign->title;?></a>

<?php
} else {
watchdog('hmm, no campaign', $_SERVER['REQUEST_URI']);
}
?>
