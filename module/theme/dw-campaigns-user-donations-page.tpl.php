<h2><?php echo t('Donations');?></h2>
<?php
    $num_per_page   = 40;

    $is_owner       = ($thisUser->uid == $user->uid);

    if(is_null($campaign)) {
        echo ('you do not have a personal campaign page for this campaign');
        return;
    }
 
    $pcp = dw_campaigns_user_get_pcp_for_campaign($thisUser, $campaign);

    if(!isset($pcp->id)) {
        echo ('you do not have a personal campaign page for this campaign');
        return;
    }

    $theme_type     = dw_campaigns_get_selected_type();

// TODO - fix this?
    $enabled     = ($campaign->field_dw_campaign_status['und']['0']['value'] == 'enabled') ? '1' : '0';

    $supporters  = dw_campaigns_get_contributions_for_pcp($pcp);

    $privacy        = dw_campaign_get_privacy($pcp);
    $headers        = array(
        array(
            'data'  => t('Thank You'),
            'class' => array('thank-yous')
        ),
        array(
            'data'  => t('Name'),
            'field' => 'name',
        ),
        array(
            'data'  => t('Amount'),
            'field' => 'amount',
        ),
        array(
            'data'  => t('Location'),
            'field' => 'location',
        ),
        array(
            'data'  => t('date'),
            'field' => 'donationdate',
            'sort'  => 'desc',
        ),
        array(
            'data'  => t(' '),
            'field' => 'note',
        )
    );

if($theme_type == 'walking') {
    unset($headers[3]);
}

    $table = 'donations_as';

    $query = "
        CREATE TEMPORARY TABLE
            $table
            (
                name char(255),
                contribution_id INT( 11 ) NOT NULL DEFAULT '-1' ,
                photo char(255),
                email char(255),
                amount float,
                location char(255),
                donationdate datetime,
                note char(255),
                is_pay_later char(255)
            ) CHARACTER SET utf8 COLLATE utf8_bin
    ";
    db_query($query);

    $rows   = array();
    foreach($supporters as $key => $supporter) {
        $image_match = '';
        
        $params                     = array();
        $params['returnFirst']      = 1;
        $params['contribution_id']  = $supporter->contribution_id;
        $contribution               = _dw_civicrm_contribution_get($params);

        $params                     = array();
        $params['contact_id']       = $supporter->contact_id;
        $params['returnFirst']      = 1;
        $contact                    = _dw_civicrm_contact_get($params);

        $row = array();

        $name       = $contact->display_name;

        
        if(!empty($supporter->pcp_roll_nickname)) {
            if($supporter->pcp_display_in_roll == 1) {
                $name .= " (" . $supporter->pcp_roll_nickname . ")";
            } else {
                $name .= " (Anonymous)";
            }
        }

        $photo      = _dw_campaigns_get_photo(array(), array(), 'donation-photo', $supporter->id, $image_match);
        $email      = isset($contact->email) ? $contact->email : '';
        $amount     = $supporter->amount;
        $location   = (isset($contact->city) ? $contact->city . ", " : '') . $contact->state_province;
        //$date       = substr($contribution->receive_date, 0, 10);
        $date       = $contribution->receive_date;

        $note       = '';
        if(isset($supporter->pcp_personal_note)) {
            if(is_object($supporter->pcp_personal_note)) {
                $note = '';
            } else {
                $note = $supporter->pcp_personal_note;
            }
        }

        //$pay_later  = $supporter->is_pay_later;
        $pay_later  = 0; // REV2 - we need to load this from the actual offline donation list
        
        db_query("insert into {$table} (name, contribution_id, photo, email, amount, location, donationdate, note, is_pay_later) VALUES (:name, :contribution_id, :photo, :email, :amount, :location, :donationdate, :note, :pay_later)", array(
            ':name'             => $name, 
            ':contribution_id'  => $supporter->contribution_id, 
            ':photo'            => $photo, 
            ':email'            => $email, 
            ':amount'           => $amount, 
            ':location'         => $location, 
            ':donationdate'     => $date, 
            ':note'             => $note, 
            ':pay_later'        => $pay_later));
    }

// MARK
    $offlines = dw_campaigns_get_offline_donations_for_pcp($pcp->id);

    $no_image       = '/sites/all/themes/dw_campaigns_' . $theme_type . '/images/no-image.gif';

    if(count($offlines) > 0) {
        $states             = _dw_civicrm_pseudoconstant_stateprovince();
    
        foreach($offlines as $offline) {
            if($offline->contribution_id != -1) {
                continue;
            }
    
            if($offline->include_in_honor_roll == 1) {
                $pcp_roll_nickname = ucfirst($offline->first_name) . substr(ucfirst($offline->last_name), 0, 1) . ".";
            } else {
                $pcp_roll_nickname = 'Anonymous';
            }
    
            $name       = $offline->first_name . " " . $offline->last_name;
    
            
            $name       .= " ($pcp_roll_nickname)";
    
            $state      = isset($states[$offline->state]) ? $states[$offline->state] : '';
    
            $photo      = $no_image;
            $email      = isset($offline->email) ? $offline->email : '';
            $amount     = (float)$offline->donation_amount;
    
            $location   = (isset($offline->city) ? $offline->city . ", " : '') . $state;
    
            $date       = $offline->receive_date . ' 00:00:00';
    
            $note       = '';
    
            if($offline->contribution_id == -1) {
                $note       = sprintf('<a href="/dw/offline/%d/edit?destination=dw/user/donations">Edit</a> <a href="/dw/offline/%d/delete?destination=dw/user/donations">Delete</a>', $offline->offline_id, $offline->offline_id);
            }
    
            $pay_later  = 1;
    
            if(empty($donation_date)) {
                $date = date('Y-m-d') . ' 00:00:00';
            }
    
            db_query("insert into {$table} (name, contribution_id, photo, email, amount, location, donationdate, note, is_pay_later) VALUES (:name, :contribution_id, :photo, :email, :amount, :location, :donationdate, :note, :pay_later)", array(
                ':name'         => $name, 
                ':contribution_id'  => $offline->contribution_id, 
                ':photo'        => $photo, 
                ':email'        => $email, 
                ':amount'       => $amount, 
                ':location'     => $location, 
                ':donationdate' => $date, 
                ':note'         => $note, 
                ':pay_later'    => $pay_later)
            );
    
        }
    }

    $sql_count = "select count(*) from $table";

    //$result = db_query("select * from donations_as " . tablesort_sql($headers));
    //$result = pager_query("select * from donations_as " . tablesort_sql($headers), $num_per_page, 0, $sql_count);
    $result = db_select($table)->fields($table)->extend('PagerDefault')->limit($num_per_page)->extend('TableSort')->orderByHeader($headers)->execute();

    $rows = array();
    //while ($db_row = db_fetch_object($result)) {

    $addresses = dw_campaigns_get_address_book($thisUser, $pcp, TRUE);
    $thankyous = dw_campaigns_get_thankyous($pcp->id); 

    foreach($result as $db_row) {
        $tr_class = '';

        if($db_row->is_pay_later == 1) {
            if($db_row->contribution_id != -1) {
                $tr_class = 'is_pay_later_paid';
            } else {
                $tr_class = 'is_pay_later';
            }
        }

        $email_exists_flag  = isset($addresses[$db_row->email]) ? TRUE : FALSE;

        $cont_id  = $db_row->contribution_id;
        if($db_row->location == "da, California") {
            $cont_id = 1;
        }
 
        // is this an offline donation, or a donation which doesn't have an entry in the address book?
        if(!$email_exists_flag || !empty($tr_class) && 2==1) {
                $email_data     = '<div class="thank-yous-na"><span>N/A</span></div>';
        } else {
            if(isset($thankyous[$cont_id])) {
                $email_data     = sprintf('<div class="thank-yous-sent %s"><span>%s</span></div>', $thankyous[$cont_id]->status, date('m/d/y', strtotime($thankyous[$cont_id]->last_modified)));
            } else {
    
                // don't let hosts send thank yous for a user!
                if(!$is_owner) {
                    $link   = '';
                    $class  = '';
                } else {
                    $link   = '/dw/user/send_thankyou/' . $addresses[$db_row->email]->address_id . '/' . $cont_id . "?ajax=1";
                    $class  = 'fb';
                }
    
                $email_data     = '<a href="' . $link . '" class="thank-yous-notsent ' . $class . '">Send</a>';
            }
        }


        $row = array(
            'data' => array(
                array('data' => "$email_data", 'class' => array('thank-yous') ),
                array('data' => '<img src="' . $db_row->photo . '" width="25">' . $db_row->name, 'class' => array('name') ),
                array('data' => dw_campaigns_force_decimal($db_row->amount, $campaign->field_dw_currency['und']['0']['value']), 'class' => array('amount')),
                array('data' => $db_row->location, 'class' => array('location')),
                array('data' => dw_campaigns_format_date($db_row->donationdate), 'class' => array('date')),
                array('data' => $db_row->note, 'class' => array('note')),
            ),
            'class' => array($tr_class)
        );

        if($theme_type=='walking') {
            unset($row['data'][3]); 
        }

        $rows[] = $row;
    }
if($enabled && $theme_type != 'derby') {
?>
<div class="offline" style="text-align:right">
    <a href="<?php echo request_uri();?>/add?ajax=1" class="fb_tall">Add Offline Donation</a>
</div>

<?php
}
?>
<div class="donation_values">
<div class="key">
    <table class="donation_key">
        <tr class="heading"><td class="lable">Offline Donation Key:</td></tr>
        <tr class="is_pay_later"><td class="amount">Offline Donation - Pending</td></tr>
        <tr class="is_pay_later_paid"><td class="amount">Offline Donation - Received and Deposited by <?php echo variable_get('dw_campaigns_organization_name', 'our cause'); ?></td></tr>
    <table>
</div>
<!-- style="width:765px;margin-left:-135px;"> -->
<?php
    echo theme('table', array('header' => $headers, 'rows' => $rows, 'empty' => t('No donations found for this Personal Campaign Page')));
    echo theme('pager');
?>
</div>
