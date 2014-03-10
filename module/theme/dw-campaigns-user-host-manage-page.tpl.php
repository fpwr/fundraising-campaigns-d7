<?php
    global $user;

    $event = node_load($event_id);

?>

<div>
    <span class="host-title"><?= t('Host Management') ?></span>
    <span class="contact-fundraisers"><a href="/dw/user/host/<?php echo $event_id;?>/contact_fundraisers"><?= t('Contact Fundraisers') ?></a> </span>
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
        <li class="first selected">
            <a class="clicker" nohref="#event_details"><?= t('Event Details').'<br>'.t('Settings')?></a>
        </li>

        <li class="">
            <a class="clicker" nohref="#event_fundraisers"><?= t('Fundraiser').'<br>'.t('Management')?></a>
        </li>

        <li class="">
            <a class="clicker" nohref="#event_request"><?= t('Request') . '<br>' . t('Services') ?></a>
        </li>

        <li class="">
            <a class="clicker" nohref="#event_reports"><?= t('Reports') ?><br>&nbsp;</a>
        </li>

        <li class="">
            <a class="clicker" nohref="#event_documentdb"><?= t('Document') . '<br>' . t('Samples') ?> </a>
        </li>

        <li class="">
            <a class="clicker" nohref="#event_donors_active"><?= t('Donors') ?><br>&nbsp;</a>
        </li>

        <li class="last">
            <a class="clicker" nohref="#event_participants"><?= t('Event') . '<br>' . t('Participants') ?></a>
        </li>
    </ul>

</div>


<div class="forms">

    <!-- Event Details Settings -->
    <div class="hidmenu menu-event_fundraisers">

        <?php //Event Details Settings
            echo drupal_render($fundraisers_form);
        ?>

    </div>

    <!-- Fundraiser Management -->
    <div class="hidmenu menu-event_details">
        <!-- sub menu... This really needs to be cleaned up. I'll do it on a weekend after I'm caught up on my other work -->
        <div class="form_menu micro">
            <ul>
                <li class="" style="background-color:#BFDFEE;">
                    <a class="clicker" nohref="#event_details">General</a>
                </li>

                <li class="">
                    <a class="clicker" nohref="#event_gallery">Gallery</a>
                </li>
            </ul>
        </div>

        <?php
            echo drupal_render($manage_form);
        ?>

    </div>

    <!-- Request Services -->
    <div class="hidmenu menu-event_gallery">
        <!-- sub menu... This really needs to be cleaned up. I'll do it on a weekend after I'm caught up on my other work -->
        <div class="form_menu micro">
            <ul>
                <li class="">
                    <a class="clicker" nohref="#event_details">General</a>
                </li>

                <li class="" style="background-color:#BFDFEE;">
                    <a class="clicker" nohref="#event_gallery">Gallery</a>
                </li>
            </ul>
        </div>


        <?= drupal_render( $event_gallery_form ) ?>


    </div>

    <div class="hidmenu menu-event_request">

        <?php
            $tshirts_and_services_node = explode('/', drupal_lookup_path("source", 'tshirts_and_services') );       //this is horrible!
            $RequestServicesNode = node_load( $tshirts_and_services_node );

            echo $RequestServicesNode->{'body'}['und'][0]['value'];
        ?>

    </div>


    <!-- Reports -->
    <div class="hidmenu menu-event_reports">
        <ul>
            <li> <a href="/dw/user/host/<?php echo $event_id;?>/reports/registration/SCREEN">Event Registration Report (SCREEN)</a> </li>
            <li> <a href="/dw/user/host/<?php echo $event_id;?>/reports/registration/CSV">Event Registration Report (download CSV)</a> </li>
        </ul>

        <?php
            echo drupal_render($reports_form);
        ?>

    </div>

    <!-- Document Database -->
    <div class="hidmenu menu-event_documentdb">
        <p>

            <?php
                echo $message;
            ?>

        </p>
    </div>

    <!-- Active Donors -->
    <div class="hidmenu menu-event_donors_active">
        <!-- donors active sub menu -->
        <div class="form_menu micro">
            <ul>
                <li class="" style="background-color:#BFDFEE;">
                    <a class="clicker" nohref="#event_donors_active">All</a>
                </li>

                <li class="">
                    <a class="clicker" nohref="#event_donors_pending">Pending</a>
                </li>

                <li class="">
                    <a class="clicker" nohref="$event_donors_previous_years">Previous Years</a>
                </li>
            </ul>
        </div>

        <p>
            <a href="/dw/user/host/<?= $event_id ?>/donors/active/csv">Download CSV</a>

            <div class="table">
                <div class="tr header">
                    <div class="td">name</div>
                    <div class="td">email</div>
                    <div class="td">street</div>
                    <div class="td">city</div>
                    <div class="td">state</div>
                    <div class="td">postal</div>
                </div>

            <?php
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

    <!-- Pending Donors -->
    <div class="hidmenu menu-event_donors_pending">
        <!-- donors pending sub menu -->
        <div class="form_menu micro">
            <ul>

                <li class="">
                    <a class="clicker" nohref="#event_donors_active">All</a>
                </li>

                <li class="" style="background-color:#BFDFEE;">
                    <a class="clicker" nohref="#event_donors_pending">Pending</a>
                </li>

                <li class="">
                    <a class="clicker" nohref="$event_donors_previous_years">Previous Years</a>
                </li>

            </ul>
        </div>

        <p>
            <a href="/dw/user/host/<?= $event_id ?>/donors/pending/csv">Download CSV</a>

            <div class="table">
                <div class="tr header" width="100%">
                    <div class="td">name</div>
                    <div class="td">email</div>
                    <div class="td">street</div>
                    <div class="td">city</div>
                    <div class="td">state</div>
                    <div class="td">postal</div>
                </div>

                <?php
                    $pcps = _dw_campaigns_get_pcps_for_campaign( $event );

                    foreach( $pcps as $id => $pcp ){

                        $supporters = dw_campaigns_pcp_get_supporters($pcp);

                        foreach( $supporters as $supporter ){

                            $contribution_id = $supporter->contribution_id;
                            $contribution = dw_campaigns_get_contribution_by_id( $contribution_id );

                            if( $contribution->contribution_status_id == 2 ){   //status 2 for pending
                                echo('<div class="tr">');

                                $pcp_contact = _dw_civicrm_contact_get(array(
                                        'contact_id'  => $supporter->contact_id,
                                        'returnFirst' => 1
                                    ));

                                echo( '<div class="td">' . $pcp_contact->display_name . '</div><div class="td">' . $pcp_contact->email . '</div><div class="td">' . $pcp_contact->street_address . '</div><div class="td">' . $pcp_contact->city . '</div><div class="td">' . $pcp_contact->state_province_name . '</div><div class="td">' . $pcp_contact->postal_code . '</div>');
                                echo('</div>');
                            }

                        }

                    }
                ?>
            </div>

        </p>
    </div>

    <!-- Donors For This Event On Other Years -->
    <div class="hidmenu menu-event_donors_previous_years">
        <!-- donors pending sub menu -->
        <div class="form_menu micro">
            <ul>

                <li class="">
                    <a class="clicker" nohref="#event_donors_active">All</a>
                </li>

                <li class="">
                    <a class="clicker" nohref="#event_donors_pending">Pending</a>
                </li>

                <li class="" style="background-color:#BFDFEE;">
                    <a class="clicker" nohref="$event_donors_previous_years">Previous Years</a>
                </li>

            </ul>
        </div>

        <p>

        <?php
            global $user;

            $campaigns  = array();

            $hosts = dw_campaigns_host_find_campaigns($user->uid);
            $keys = array_keys($hosts);

            foreach( $keys as $index => $nodeId ){
                $other_event = node_load( $nodeId );

                if( $event->title == $other_event->title  ){
                    $date = substr( $other_event->field_dw_date_range['und'][0]['value'], 0, 10 );
                    $links[ $date ] = '<a href="/dw/event/'.$event->title.'/donors/'.$date.'/csv">Donors for '.$date.' [csv]</a>';

                }

            }

            //output the list of links for all previous years that we've found for this event (events must share same title for this to work)
            foreach( $links as $link ){
                echo( $link ).'<br>';

            }

        ?>

        </p>
    </div>

    <!-- Event Participants -->
    <div class="hidmenu menu-event_participants">
        <p>
        <a href="/dw/event/<?= $event_id ?>/participants/csv">Download CSV</a>

        <div class="table">
            <div class="tr header" width="100%">
                <div class="td">name</div>
                <div class="td">email</div>
                <div class="td">street</div>
                <div class="td">city</div>
                <div class="td">state</div>
                <div class="td">postal</div>
            </div>

            <?php

                $participants = dw_campaign_get_event_participants_by_campaign($event_id);

                foreach( $participants as $participant ){
                    $participant = $participant['contact'];
                    $participant_name   = $participant->display_name;
                    $participant_email  = $participant->email;
                    $participant_street = $participant->street_address . ' ' .$participant->supplemental_address_1;
                    $participant_city   = $participant->city;
                    $participant_state  = $participant->state_province_name;
                    $participant_postal = $participant->postal_code;

                    echo '<div class="tr">';
                    echo '  <div class="td">'. $participant_name .'</div>';
                    echo '  <div class="td">'. $participant_email .'</div>';
                    echo '  <div class="td">'. $participant_street .'</div>';
                    echo '  <div class="td">'. $participant_city .'</div>';
                    echo '  <div class="td">'. $participant_state .'</div>';
                    echo '  <div class="td">'. $participant_postal .'</div>';
                    echo '</div>';

                }


            ?>
        </p>
    </div>
</div>
