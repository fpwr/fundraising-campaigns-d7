<?php
if(!function_exists('block_class')) {
    function block_class () { }
}
// $Id: block.tpl.php,v 1.3 2007/08/07 08:39:36 goba Exp $
?>
  <div class="block block-<?php print $block->module; ?> <?php print block_class($block); ?> <?php print $classes; ?>" id="block-<?php print $block->module; ?>-<?php print $block->delta; ?>" <?php print $attributes; ?>>
    <?php if($block->subject) { ?><h2 class="title"><?php print $block->subject; ?></h2><?php } ?>
    <div class="content"<?php print $content_attributes; ?>><?php print $content; ?></div>
 </div>
