<?php
    $event = node_load($event_id);
?>

<div>
    <span class="host-title">Host Management</span>
    <span class="contact-fundraisers"><a href="/dw/user/host/<?php echo $event_id;?>/contact_fundraisers">Contact Fundraisers</a> </span>
    <span class="host-location"><?php echo $event->title;?></span>
</div>

<?php 
   drupal_add_js('
      (function($) {
        $(document).ready(function() {
          dw_campaigns.initHostsMenu();
        });
     })(jQuery);
    ', 'inline');

   $a = variable_get('dw_campaign_host_panel_message', 'Welcome to the Host Panel');

   if(is_array($a)) {
       if(isset($a['value'])) {
           $message = $a['value'];

       } else {
           $message = t('No documents are currently available');

       }

   } else {
       $message = $a;

   } 

?>


<div class="newitems">
    <ul>
    </ul>
</div>

<div class="menu-event_statistics">
    <?php
        echo drupal_render($totals_form);
    ?>
</div>

<!-- //TabList -->
<div class="form_menu">

    <ul>

        <!-- <li class="first selected"><a class="clicker" nohref="#event_statistics">Statistics<br></a></li> -->
        <li class="first selecte">
            <a class="clicker" nohref="#event_details">Event Details<br>Settings</a>
        </li>

        <li class="">
            <a class="clicker" nohref="#event_fundraisers">Fundraiser<br>Management</a>
        </li>

        <li class="">
            <a class="clicker" nohref="#event_request">Request<br>Services</a>
        </li>

        <li class="">
            <a class="clicker" nohref="#event_reports">Reports<br>&nbsp;</a>
        </li>

        <li class="">
            <a class="clicker" nohref="#event_documentdb">Document<br>Samples</a>
        </li>

        <li class="last">
            <a class="clicker" nohref="#event_donors">Donors<br>&nbsp;</a>
        </li>
    </ul>

</div>


<div class="forms">
    <div class="hidmenu menu-event_fundraisers">

        <?php //Event Details Settings
            echo drupal_render($fundraisers_form);
        ?>

    </div>

    <div class="hidmenu menu-event_details">

        <?php //Fundraiser Management
            echo drupal_render($manage_form);
        ?>

    </div>

    <div class="hidmenu menu-event_request">

        <?php //Request Services
            $tshirts_and_services_node = explode('/', drupal_lookup_path("source", 'tshirts_and_services') );       //this is horrible!
            $RequestServicesNode = node_load( $tshirts_and_services_node );

            echo $RequestServicesNode->{'body'}['und'][0]['value'];
        ?>

    </div>

    <div class="hidmenu menu-event_reports">
        <ul>
            <li> <a href="/dw/user/host/<?php echo $event_id;?>/reports/registration/SCREEN">Event Registration Report (SCREEN)</a> </li>
            <li> <a href="/dw/user/host/<?php echo $event_id;?>/reports/registration/CSV">Event Registration Report (download CSV)</a> </li>
        </ul>

        <?php //Reports
            echo drupal_render($reports_form);
        ?>

    </div>

    <div class="hidmenu menu-event_documentdb">
        <p>

            <?php //Document Database
                echo $message;
            ?>

        </p>
    </div>

    <div class="hidmenu menu-event_donors">
        <p>
            <a href="/dw/user/host/<?= $event_id ?>/donors/csv">Download CSV</a>
            <div class="table">
                <div class="tr header">
                    <div class="td">name</div>
                    <div class="td">email</div>
                    <div class="td">street</div>
                    <div class="td">city</div>
                    <div class="td">state</div>
                    <div class="td">postal</div>
                </div>

            <?php //Donors
                $pcps = _dw_campaigns_get_pcps_for_campaign( $event );

                foreach( $pcps as $id => $pcp ){

                    $supporters = dw_campaigns_pcp_get_supporters($pcp);

                    foreach( $supporters as $supporter ){
                        echo('<div class="tr">');
                        //var_dump( $supporter);
                        $pcp_contact = _dw_civicrm_contact_get(array(
                                'contact_id'  => $supporter->contact_id,
                                'returnFirst' => 1
                            ));

                        echo( '<div class="td">' . $pcp_contact->display_name . '</div><div class="td">' . $pcp_contact->email . '</div><div class="td">' . $pcp_contact->street_address . '</div><div class="td">' . $pcp_contact->city . '</div><div class="td">' . $pcp_contact->state_province_name . '</div><div class="td">' . $pcp_contact->postal_code . '</div>');
                        echo('</div>');
                    }

                }
            ?>
            </div>
        </p>
    </div>
</div>
