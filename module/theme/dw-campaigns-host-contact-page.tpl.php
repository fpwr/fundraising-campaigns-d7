<?php
    $title = isset($host->title) ? $host->title : NULL;
    $offset = strpos($title, '-');
    if($offset !== FALSE) {
        $title = trim(substr($title, 0, $offset));
    }
    $p_title = empty($title) ? '' : 'to ' . $title;
?>
<div class="host-contact">
    <div class="intro">
        Send Message <?php echo $p_title; ?>
    </div>
<?php
echo drupal_render($form);
?>
</div>
