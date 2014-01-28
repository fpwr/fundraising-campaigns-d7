<?php
// this is a form template!
?>
<table id="group_data">
<tr><th>Name</th><th>Grouping</th></tr>
<?php
foreach($group_data as $group) {
?>
    <tr><td class="campaign-name"><?php echo $group->name; ?></td><td class="campaign-group"><?php echo $group->group; ?></td></tr>
<?php
}
?>
</table>
<?php
echo $form_submit;
?>
