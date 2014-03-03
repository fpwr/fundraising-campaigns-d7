<?php
    if(arg(0)!='dw') {
        return;
    }
?>
<script type="text/javascript">
(function($) {
$(document).ready(function() { 
    $('.rot13').each(undoRot);

    function undoRot(n) { 
        var letl = "abcdefghijklmnopqrstuvwxyz";
        var letc = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	var temp = $(this).attr('href');
        var len  = temp.length;
        var onec = "";
        var res  = "";
        var out  = "";
        var pos  = "";

        for(i = 0; i < len ; i++) {
            onec = temp.substr(i, 1);
            isup = letc.indexOf(onec);
            if(isup != -1) {
                onec = letl.substr(isup, 1);
            }
            res = letl.indexOf(onec);
            if (res != -1) {
                pos = (res + 13) % 26;
                if(isup == -1) {
                    out = out + letl.substr(pos, 1);
                } else {
                    out = out + letc.substr(pos, 1);
                }
            } else {
                out = out + onec;
            }
        }
        $(this).attr('href', out);
    };
});
})(jQuery);
</script>
<?php

    $node       = dw_campaigns_get_selected_campaign();
    $mode_type	= dw_campaigns_get_selected_type();
	
    if(is_null($node) || $node->nid <= 0) {
        return;
    }
    $url        = NULL;
    $event_id   = dw_campaigns_get_event_page_id_from_campaign($node);

    if(is_numeric($event_id) && $event_id > 0) {
        $baseurl    = variable_get('dw_campaigns_cfg_base_url', '');
        if(!empty($baseurl)) {
            $url    = sprintf("%s/civicrm/event/info?id=%s&reset=1", $baseurl, $event_id);
            $url    = sprintf("%s/civicrm/event/register?id=%s&reset=1", $baseurl, $event_id);
        }
    }

    $shortened      = FALSE;

    //$details = isset($node->teaser) ? $node->teaser : '';
    $details = isset($node->body['und']['0']['value']) ? $node->body['und']['0']['value'] : '';

    $details = strip_tags($details);

    if(empty($details)) {
        $details = 'Details coming soon';
    }

    if(strlen($details)>150) {
        $details    = substr($details, 0, 150) . '...';
	$shortened  = TRUE;
    }

    $contact_data   = '';
    $contact_phone  = '';

    $extended = NULL;

    if($mode_type == 'walking') {

        $contact_phone = isset($node->field_dw_contact_phone['und']['0']['value']) ? $node->field_dw_contact_phone['und']['0']['value'] : ''; 
        $contact_name  = isset($node->field_dw_contact_name['und']['0']['value']) ? $node->field_dw_contact_name['und']['0']['value'] : ''; 
        $contact_email = isset($node->field_dw_contact_email['und']['0']['value']) ? $node->field_dw_contact_email['und']['0']['value'] : ''; 

        if(!empty($contact_name)) {
 
	    $contact_data = $contact_name;
	
       	    if(!empty($contact_email)) { 
	        $contact_m     = str_rot13("mailto:$contact_email");
	        $contact_data = '<a href="' . $contact_m . '" class="rot13">' . $contact_name . '</a>';
	    }
        }
        //$result     = db_query("select * from content_type_dw_campaigns_event_page where field_dw_eventdetails_node_value = :nid", array(':nid' => $node->nid));
        $result     = db_query("select * from {field_data_field_dw_eventdetails_node} where field_dw_eventdetails_node_value = :nid", array(':nid' => $node->nid));
	$extended   = $result->fetchObject();

        if($extended && !is_null($extended)) {
            //$extended_data = node_load($extended->nid);
            $extended_data = node_load($extended->entity_id);
        }

        include_once('dw-campaigns-map.tpl.php');
    }


    $date = isset($node->field_dw_event_date['und']['0']['value']) ? $node->field_dw_event_date['und']['0']['value'] : NULL;
    $time = isset($node->field_dw_event_time['und']['0']['value']) ? $node->field_dw_event_time['und']['0']['value'] : NULL;

    $ev_loc = isset($node->field_dw_event_location['und']['0']['value']) ? $node->field_dw_event_location['und']['0']['value'] : NULL;

    $docs = array();
       
    for($i=0;$i<=4;$i++) {
        $orig_filename = isset($extended_data->field_dw_eventdetails_dl_documen['und'][$i]['filename']) ? $extended_data->field_dw_eventdetails_dl_documen['und'][$i]['filename'] : NULL;
        if(is_null($orig_filename)) {
            continue;
        }

        $uri = isset($extended_data->field_dw_eventdetails_dl_documen['und'][$i]['uri']) ? $extended_data->field_dw_eventdetails_dl_documen['und'][$i]['uri'] : NULL;
        $download_url = file_create_url($uri);

        if(!is_null($download_url)) {
            $download_title = isset($extended_data->field_dw_eventdetails_dl_documen['und'][$i]['description']) ? $extended_data->field_dw_eventdetails_dl_documen['und'][$i]['description'] : NULL;

            if(is_null($download_title) || empty($download_title)) {
                $download_title = t('Download');
            }

            $docs[] = array('title' => $download_title, 'url' => $download_url, 'filename' => $orig_filename);
        }
    }

    $hosts          = dw_campaigns_get_campaign_hosts($node);
    $host           = implode(', ', $hosts);

?>
<div class="derby-event-details"><div class="inside">
<h2 class="block-title clearfix">Event Details</h2>
    <table>
<?php
    if(!is_null($date)) {
?>
        <tr class="date">
            <th><?php echo t('When'); ?></th><td><?php echo $date;?> <?php echo $time; ?></td>
        </tr>
<?php
    }

    if(!is_null($ev_loc)) {
?>
        <tr class="location">
            <th><?php echo t('Where'); ?></th><td><?php echo $ev_loc; ?></td>
        </tr>
<?php
    }
?>
        <tr class="details">
            <th><?php echo t('Details'); ?></th><td><?php echo $details; ?>
        <?php
            if(!is_null($url) && $mode_type != 'walking') {
        ?>
            <br><a href="<?php echo $url;?>" target="_blank"><?php echo t('Register for Event'); ?></a>
	<?php
            }
	?>
        <?php
           // we also show this for walking...
           if($extended && (!is_null($extended) || $shortened || $mode_type == 'walking')) {
        ?>
            <br><a class="fb_medium" href="/dw/walking/event-extended/<?php echo $node->nid;?>?ajax=1"><?php echo t('Get more details');?></a>
        <?php 
           }
        ?>
            </td>
        </tr>
<?php
        if($mode_type == 'walking' && !empty($contact_data)) {
?>
        <tr class="contact">
            <th><?php echo t('Contact'); ?></th>
            <td><?php echo $contact_data;?>
                <?php if(!empty($contact_phone)) { 
                          echo "$contact_phone"; 
                      }
                ?></td>
        </tr>
<?php
        }

        if(!empty($host)) {
?>
        <tr class="contact">
            <th><?php echo t('Host'); ?></th>
            <td><?php echo $host; ?></td>
        </tr>
<?php
        }
?>

<?php      
        if(count($docs)>0) {
            foreach($docs as $doc) {
?>
                <tr class="document">
                    <th><?php echo $doc['title']; ?></th>
                    <td><a href="<?php echo $doc['url']; ?>"><?php echo t('Download');?></a></td>
                </tr>

<?php
            }
        }

        if($mode_type == 'walking') {
            $country = dw_campaigns_get_event_country_by_campaign($campaign);
            $pledge_document = isset($country->field_dw_country_pledge_form['und']['0']['uri']) ? $country->field_dw_country_pledge_form['und']['0']['uri'] : NULL;

            if(!is_null($pledge_document)) {
                $pledge_document = str_replace("public://", '/sites/default/files/', $pledge_document);
            }

            if(!is_null($pledge_document)) {
?>
            <tr class="pledge"><th><?php echo t('Pledge');?></th><td><a href="<?php echo $pledge_document; ?>"><?php echo t('Pledge Form');?></td></tr>
<?php
            }
        }

        if($mode_type == 'walking' && !empty($event_map)) {
?>
            <tr class="map">
                <th><?php echo t('Map'); ?></th>
                <td> <?php echo $event_map; ?> </td>
            </tr>
<?php
        }
?>
            <?php
                $flickr_user_id = $node->field_flickr_gallery_id['und'][0]['value'];//flickr_user_find_by_identifier( $host_flickrId );
                $flickr_photoset_id = $node->field_flickr_gallery_photoset_id['und'][0]['value'];//user_load($user->uid)->field_flickrphotosetid['und'][0]['value'];

                if( $flickr_user_id && $flickr_photoset_id ){
                    echo('<tr><th>Event Gallery</th><td class="event-gallery-container">');

                    $block = _flickr_block_photoset_random($flickr_user_id, 3, 's', 'all', $flickr_photoset_id);

                    echo( $block );

                    echo('</td></tr>');
                }
            ?>
    </table>
</div>
</div>
