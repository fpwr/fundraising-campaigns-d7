<ul>
<li> <a href="/dw/user/host/<?php echo $event_id;?>/contact_fundraisers">Contact Fundraisers</a> </li>
<li> <a href="/dw/user/host/<?php echo $event_id;?>/reports/registration/SCREEN">Event Registration Report (SCREEN)</a> </li>
<li> <a href="/dw/user/host/<?php echo $event_id;?>/reports/registration/CSV">Event Registration Report (download CSV)</a> </li>
</ul>
<?php
echo drupal_render($totals_form);
echo drupal_render($fundraisers_form);
echo drupal_render($manage_form);
echo drupal_render($request_form);
echo drupal_render($reports_form);
