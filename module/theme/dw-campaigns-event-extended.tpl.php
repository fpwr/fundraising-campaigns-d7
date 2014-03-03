<?php
    if(is_null($campaign) || !$campaign) {
        echo t("Could not locate extended event information");
        return;
    }

    $node       = $campaign;
    $node       = (is_object($node)) ? $node : node_load($node);

//    $result     = db_query("select * from content_type_dw_campaigns_event_page where field_dw_eventdetails_node_value = :nid", array(':nid' => $node->nid));

//    $extended   = $result->fetchObject();

    $result     = db_query("select * from {field_data_field_dw_eventdetails_node} where field_dw_eventdetails_node_value = :nid", array(':nid' => $node->nid));
    $extended   = $result->fetchObject();

    $extended_data      = NULL;
    $extended_details   = '';

    if($extended && !is_null($extended)) {
        $extended_data = node_load($extended->entity_id);
       // $extended_data      = node_load($extended->nid);
        $extended_details   = $extended_data->body['und']['0']['value'];
    }

    //$details = $node->teaser;
    $details = isset($node->body['und']['0']['value']) ? $node->body['und']['0']['value'] : '';

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


    include_once('dw-campaigns-map.tpl.php');
?>
    <div id="extended" class="derby-event-details">
        <h2>Event Details</h2>
        <table>
            <tr class="details">
                <th><?php echo t('Details'); ?></th><td><?php echo $details; ?>
            </tr>
<?php
        if(strlen($extended_details) > 0) {
?>
            <tr class="details">
                <th><?php echo t('More Details'); ?></th><td><?php echo $extended_details; ?>
            </tr>
<?php
        }
?>
<?php 
        if(!is_null($extended_data) && isset($extended_data->field_dw_eventdetails_prizes['und']['0']['value'])) {
?>
            <tr class="prizes">
            <th><?php echo t('Prizes'); ?></th>
            <td><?php echo $extended_data->field_dw_eventdetails_prizes['und']['0']['value']; ?></td>
            </tr>
<?php 
        }

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
?>
            <tr class="map">
                <th><?php echo t('Map'); ?></th>
                <td> <?php echo $event_map; ?> </td>
            </tr>
        </table>
    </div>
