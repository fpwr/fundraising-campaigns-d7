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

   $a=variable_get('dw_campaign_host_panel_message', 'Welcome to the Host Panel'); 

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

<div class="form_menu">
<ul>
<!-- <li class="first selected"><a class="clicker" nohref="#event_statistics">Statistics<br></a></li> -->
<li class="first selecte"><a class="clicker" nohref="#event_details">Event Details<br>Settings</a></li>
<li class=""><a class="clicker" nohref="#event_fundraisers">Fundraiser<br>Management</a></li>
<li class=""><a class="clicker" nohref="#event_request">Request<br>Services</a></li>
<li class=""><a class="clicker" nohref="#event_reports">Reports<br>&nbsp;</a></li>
<li class="last"><a class="clicker" nohref="#event_documentdb">Document<br>Database</a></li>
</div>

<div class="forms">
    <div class="hidmenu menu-event_fundraisers">
<?php
        echo drupal_render($fundraisers_form);
?>
    </div>
    <div class="hidmenu menu-event_details">
<?php
        echo drupal_render($manage_form);
?>
    </div>
    <div class="hidmenu menu-event_request">

<?php
        echo drupal_render($request_form);
?>
    </div>
    <div class="hidmenu menu-event_reports">
        <ul>
            <li> <a href="/dw/user/host/<?php echo $event_id;?>/reports/registration/SCREEN">Event Registration Report (SCREEN)</a> </li>
            <li> <a href="/dw/user/host/<?php echo $event_id;?>/reports/registration/CSV">Event Registration Report (download CSV)</a> </li>
        </ul>
<?php
        echo drupal_render($reports_form);
?>
    </div>
    <div class="hidmenu menu-event_documentdb">
        <p>
        <?php echo $message;?>
        </p>
    </div>
</div>
