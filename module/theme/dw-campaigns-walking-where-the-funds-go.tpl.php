<?php
    global $language;

    $about_link = $language->language == 'en' ? '/content/about-us' : '/node/86';
?>
<div class="where-the-funds-go">
    <h2><?php echo t('Where the Funds Go'); ?></h2>
<p>
<?php
    //JFN - january 13 2014 1107 - [#contentChange "replaced text, removed link, modified about us link"]
    echo t('100% of proceeds from One SMALL Step events fund cutting edge Prader-Willi syndrome research. Learn more about research funded by the Foundation for Prader-Willi Research!');
?>
</p>

<br><a href="http://fpwr.org/prader-willi-syndrome-research" class="btn btn-light-blue"><?php echo t('Learn more about PWS Research'); ?></a>

</div>
