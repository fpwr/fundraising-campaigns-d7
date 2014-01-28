<?php
$node = node_load(467);
//echo $node->body['und'][0]['safe_value'];
echo $node->body['und'][0]['value'];
	echo drupal_render($form);
