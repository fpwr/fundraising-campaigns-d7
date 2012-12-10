<?php
    global $selected;

    $res = _dw_campaigns_campaigns_campaign_total();
    extract($res); // gives $totals, $usd_totals and $campaigns
  
   // if its not a lot, we assume its for the bottom of the homepage 
   if($show_cnt < 100) {
?>    

<div class="top-locations">
    <h2><?php echo t('Top Locations'); ?></h2>
    <?php
        if($show_cnt<=10) {
    ?>
    <a href="/dw/walking/toplocations" class="see-all"><?php echo t('see all'); ?></a>
    <?php
        }
    ?>
    <ul>
        <?php
            if(!isset($selected)) {
                $selected = NULL;
            }
            
            $i  = 0;
            foreach($usd_totals as $nid => $usd_total) {
                if(dw_campaigns_hide_campaign($campaigns[$nid])) {
                    continue;
                } else {
                    $our_campaign = $campaigns[$nid];
                    $i++;
        ?>
            <li<?php if($nid==$selected) echo ' class="location-selected"'; ?>>
                <div class="left"><span class="position"><?php echo $i;?></span><a href="/dw/walking/location/<?php echo $nid; ?>" class="location-label"><?php echo $campaigns[$nid]->field_dw_campaign_location['und']['0']['value']; ?></a></div>
                <div class="right"><a href="/dw/walking/location/<?php echo $nid; ?>" class="dollar-amount"><?php echo dw_campaigns_force_decimal($totals[$nid], $our_campaign->field_dw_currency['und']['0']['value']); ?></a></div>
            </li>
        <?php
                    if($i == $show_cnt) {
                        break;
                    }
                }
            }
        ?>
    </ul>
</div>
<?php
    } else {

    $num_per_page = $show_cnt; // 999999

    $table = "leader_as_$num_per_page";

    $headers        = array(
        array(
            'data'  => t('Position'),
            'field' => 'position',
        ),
        array(
            'data'  => t('Location'),
            'field' => 'location',
        ),
        array(
            'data'  => t('Fundraisers'),
            'field' => 'fundraisers',
        ),
        array(
            'data'  => t('Total Raised'),
            'field' => 'amount',
            'sort'  => 'desc'
        )
    );


    $query = "
        CREATE TEMPORARY TABLE
            $table
            (
                name char(255),
                photo char(255),
                position int,
                amount float,
                raw_amount float,
                location char(255),
                fundraisers int,
                url char(255),
                campaign_id int
            )
    ";
    db_query($query);

    $position   = 0;
    $rows       = array();

    foreach($usd_totals as $nid => $usd_total) {
        $campaign       = $campaigns[$nid];

        if(dw_campaigns_hide_campaign($campaign)) {
           continue;
        }


        $position++;

        $image_match    = '';
        $image_params   = array(
            'w'                 => 100,
            'contribution'      => true,
        );

        $name           = $campaign->title;
        $photo_file     = 'sites/all/themes/dw_campaigns_walking/images/no-image.gif';

        if(isset($campaign->field_dw_campaign_image['und']['0']['uri'])) {

            $temp_filename = str_replace("public://", 'sites/default/files/', $campaign->field_dw_campaign_image['und']['0']['uri']);
            if(file_exists($temp_filename)) {
                $photo_file = $temp_filename;
            }
        }

        $photo  = _dw_campaigns_thumb($photo_file, $image_params);

        $amount         = dw_campaigns_convert_to_usd($campaign->field_dw_currency['und']['0']['value'], $totals[$nid]);
        $raw_amount     = $totals[$nid];
        $location       = $campaign->field_dw_campaign_location['und']['0']['value'];
        $fundraisers    = $pcp_counts[$nid];
        $url            = '/dw/walking/location/' . $nid;
        
        
        //db_query("insert into {$table} (name, photo, position, amount, raw_amount, location, fundraisers, url, campaign_id) VALUES ('%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s')", $name, $photo, $position, $amount, $raw_amount, $location, $fundraisers, $url, $campaign->nid);
        db_query("insert into {leader_as_$num_per_page} (name, photo, position, amount, raw_amount, location, fundraisers, url, campaign_id) VALUES (:name, :photo, :position, :amount, :rawamount, :location, :fundraisers, :url, :campaignid)", array(
            ':name'         => $name, 
            ':photo'        => $photo, 
            ':position'     => $position, 
            ':amount'       => $amount, 
            ':rawamount'    => $raw_amount, 
            ':location'     => $location, 
            ':fundraisers'  => $fundraisers, 
            ':url'          => $url, 
            'campaignid'    => $campaign->nid));
    }

    $sql_count = "select count(*) from $table";

    //$result = db_query("select * from donations_as " . tablesort_sql($headers));
    //$result = pager_query("select * from leader_as_$num_per_page " . tablesort_sql($headers), $num_per_page, 0, $sql_count);
    $result = db_select($table)->fields($table)->extend('PagerDefault')->limit($num_per_page)->extend('TableSort')->orderByHeader($headers)->execute();

    $rows = array();
    foreach($result as $db_row) {
        $our_campaign = $campaigns[$db_row->campaign_id];
        $rows[] = array(
            'data' => array(
                array('data' => $db_row->position, 'class' => array('position')),
                array('data' => '<a href="' . $db_row->url . '"> <img src="' . $db_row->photo . '" width="50"> <span class="location">' . $db_row->location . '</span></a>', 'class' => array('photo') ),
                array('data' => $db_row->fundraisers, 'class' => array('fundraisers')),
                array('data' => dw_campaigns_force_decimal($db_row->raw_amount, $our_campaign->field_dw_currency['und']['0']['value']), 'class' => array('amount')),
            )    
        );
    }

    echo "<h2>" . t('One Small Step Locations') . "</h2>";
    echo theme('table', array('header' => $headers, 'rows' => $rows));
    echo theme('pager');

   }
?>
