<?php
    if(!isset($campaign) || is_null($campaign) || !$campaign) { 
        $campaign           = $node; // its named differently in the event block for some reason
    }

    $address            = dw_campaigns_build_location_addr($campaign);
    $campaignLocation   = dw_campaigns_get_location_for_address($address);
    $location           = $campaignLocation;
    $title              = addslashes($campaign->title);
    $html               = json_encode('<div class="map-infowindow"><h3>' . $title . "</h3></div>");

    $temp_addr = preg_replace('/[\ ,]/', ' ', $address);
    if(empty($temp_addr)) {
        $event_map = '<div id="events-map-wrapper"><div id="events-map">No address available</div></div>';
        return;       
    }

    $event_map = '';


    if(!isset($location['lat'])) {
        return;
    }

    drupal_add_js('http://maps.google.com/maps/api/js?sensor=false', 'external');

    $event_map = ' <div id="events-map-wrapper"> <div id="events-map"></div> </div>';

    $container = '#events-map';

    ob_start();
?>
(function($) {
$(document).ready(function() {
    var mapContainer = $('<?php echo $container;?>');

    dw_campaigns.initEventsMap({
        container: mapContainer,
        zoom: 13,
        lat: <?php echo $location['lat']; ?>,
        long: <?php echo $location['lng']; ?>
    });

    dw_campaigns.addMarkerToMap(mapContainer,<?php echo json_encode($campaignLocation); ?>,"<?php echo $title; ?>",<?php echo $html; ?>); 
});
})(jQuery);
<?php
    $js = ob_get_contents();
    ob_end_clean();
    drupal_add_js($js, 'inline');
?>
