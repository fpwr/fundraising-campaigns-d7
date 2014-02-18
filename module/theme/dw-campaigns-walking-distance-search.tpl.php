<?php
    $local = dw_campaigns_get_visitor_location();
    if($local['known']) {
        $search_terms = $local['city'] . ', ' . $local['region'];
    } else {
        $search_terms = "United States";
    }
    
    if(isset($_REQUEST['query'])) {
        $search_terms = $_REQUEST['query'];
    }
    
    $campaigns = dw_campaigns_get_close_campaigns();
    
    $rows = '';
    $units = variable_get('dw_campaigns_walking_distance_unit', 'M');
    switch($units) {
        case 'M':
            $units_wd = t('Miles');
            break;
        case 'K':
            $units_wd = t('Kilometers');
            break;
        default:
            $units_wd = '';
            break;
    }

    $num_per_page = 40;

    $table = "distance_as_$num_per_page";

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
            'data'  => t('Distance'),
            'field' => 'distance',
            'sort'  => 'asc'
        ),
        array(
            'data'  => t('Number of Walkers'),
            'field' => 'walkers',
        ),
        array(
            'data'  => t('Total Raised'),
            'field' => 'amount',
        )
    );


    $query = "
        CREATE TEMPORARY TABLE
            $table
            (
                position int,
                location char(255),
                distance float,
                walkers int,
                amount float,
                campaign_id int
            )
    ";
    db_query($query);

    $position   = 0;
    $rows       = array();
    foreach($campaigns as $id => $campaign) {
        $pcps                = _dw_campaigns_get_pcps_for_campaign($campaign);
        
        $position++;
        $location           = $campaign->field_dw_campaign_location['und']['0']['value']; // $campaign->title;
        list($distance,)    = split('-', $id);
        $walkers            = count(get_object_vars($pcps));
        $amount             = dw_campaigns_get_contribution_total_for_campaign($campaign);
        $campaign_id        = $campaign->nid;
        
        //db_query("insert into {$table} (position, location, distance, walkers, amount, campaign_id) VALUES ('%s', '%s', '%s', '%s', '%s', '%s')", $position, $location, $distance, $walkers, $amount, $campaign_id);
        db_query("insert into {$table} (position, location, distance, walkers, amount, campaign_id) VALUES (:position, :location, :distance, :walkers, :amount, :campaignid)", array(
            ':position'     => $position, 
            ':location'     => $location, 
            ':distance'     => $distance, 
            ':walkers'      => $walkers, 
            ':amount'       => $amount, 
            ':campaignid'   => $campaign_id));

    }


    $result = db_select($table)->fields($table)->extend('PagerDefault')->limit($num_per_page)->extend('TableSort')->orderByHeader($headers)->execute();

    $rows = array();

    //while ($db_row = db_fetch_object($result)) {
    foreach($result as $db_row) {
        $our_campaign = node_load($db_row->campaign_id);

        $rows[] = array(
            'data' => array(
                array('data' => $db_row->position, 'class' => array('position')),
                array('data' => '<a href="/dw/walking/location/' . $db_row->campaign_id . '">' . $db_row->location . '</a>', 'class' => array('location')),
                array('data' => floor($db_row->distance) . ' ' . $units_wd, 'class' => array('distance')),
                array('data' => $db_row->walkers,  'class' => array('walkers')),
                array('data' => dw_campaigns_force_decimal($db_row->amount, $campaign->field_dw_currency['und']['0']['value'])),
            )    
        );
    }
    
?>

<form method="post">
    <p><?php echo t('Enter a Zip code, or a City, State below'); ?></p> 
    <?php echo t('Searching From '); ?><input type="text" name="query" class="location-search" value="<?php echo htmlentities($search_terms);?>">
    <input type="submit" value="<?php echo t('Find Distance'); ?>">
</form>

<?php
    echo theme('table', array('header' => $headers, 'rows' => $rows));
    echo theme('pager');
?>
