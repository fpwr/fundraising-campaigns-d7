<div id="navBanner">
<?php if (isset($page['help']) || ($show_messages && $messages)): ?>
  <div id='console'><div class='limiter clearfix'>
    <?php print render($page['help']); ?>
    <?php if ($show_messages && $messages): print $messages; endif; ?>
  </div></div>
<?php endif; ?>

<?php if ($page['header']): ?>
  <div id='header'>
    <div class="inner">
      <div class='limiter clearfix'>
      <?php print render($page['header']); ?>
      </div>
    </div>
  </div>
<?php endif; ?>

<div id='branding'><div class='limiter clearfix'>
  <?php if ($site_name): ?><h1 class='site-name'><?php print $site_name ?></h1><?php endif; ?>
</div></div>

<div id='navigation'><div class='limiter clearfix'>
  <?php if (isset($main_menu)) : ?>
    <?php print theme('links', array('links' => $main_menu, 'attributes' => array('class' => 'links main-menu'))) ?>
  <?php endif; ?>
  <?php if (isset($secondary_menu)) : ?>
    <?php print theme('links', array('links' => $secondary_menu, 'attributes' => array('class' => 'links secondary-menu'))) ?>
  <?php endif; ?>
</div></div>

<!--
<?php if (isset($page['highlighted'])): ?>
  <div id='highlighted'><div class='limiter clearfix'>
    <?php print render($page['highlighted']); ?>
  </div></div>
<?php endif; ?>
-->

<div id='page'><div class='limiter clearfix'>
  <div id='content-top' class='clear-block'><?php print render($page['contentTop']);?></div>

  <div id='main' class='clearfix'>
    <div class="bg">

      <?php if (isset($page['left'])): ?>
      <div id='left' class='clear-block'>
        <?php print render($page['left']); ?>
      </div>
      <?php endif; ?>

      <?php if (!empty($page['right']) || !empty($tabs) || !empty($tabs2)): ?>
      <?php 
if(count($page['right']) > 0 && !isset($_REQUEST['ajax']))  {
    foreach($page['right'] as $key => $entry) {
        if(strncmp($key, 'dw_', 3) == 0) {
            dw_campaigns_set_hastabs(); 
            break;
        }
    }
}
if(!empty($tabs['#primary'])) {
	dw_campaigns_set_hastabs(); 
} 
?>
      <div id='right' class='clear-block'>
        <?php if (!empty($tabs)) print render($tabs); ?>
        <?php if (!empty($tabs2)) print render($tabs2); ?>
        <?php print render($page['right']); ?>
      </div>
      <?php endif; ?>


      <?php if ($breadcrumb) print $breadcrumb; ?>

      <?php print render($title_prefix); ?>
      <?php if ($title): ?><h1 class='page-title'><?php print $title ?></h1><?php endif; ?>
      <?php print render($title_suffix); ?>


      <div id='content' class='clear-block'>
        <?php print render($page['contentInnerTop']); ?>
        <?php print render($page['content']); ?>
        <?php print render($page['contentInnerBottom']); ?>
      </div>

    </div>
  </div>

  <div id='content-bottom' class='clear-block'><?php print render($page['contentBottom']);?></div>

</div></div>

<div id="footer"><div class='limiter clearfix'>
  <?php print $feed_icons ?>
  <?php print render($page['footer']) ?>
</div></div>
</div>
