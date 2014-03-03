<?php echo render($pcpCreateForm); ?>
<div style="display:none">
        <a href="#hidden-words" id="show-words"></a>
        <div id="hidden-words">
                <span class="please-wait"><?php echo t('Please wait, we are creating your fundraising page.'); ?></span>
                <br>
                <span class="please-wait-extra"><?php echo t('Do not reload or navigate away from this page'); ?></span>
        </div>
</div>
