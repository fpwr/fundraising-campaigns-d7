<?php
    if(arg(1) != 'users' || !is_null(arg(4))) {
        return;
    }

    if(is_null($pcp) || $pcp->id==0) {
        return;    
    }
echo "<!-- PCP_ID: {$pcp->id} -->";

    $mode_type      = dw_campaigns_get_selected_type();

    $image_params   = array(
        'w'                 => 260,
    );
    $matched_image  = '';
    //get the user and drop it in for a preview
    $imageSrc       = _dw_campaigns_get_photo($thisUser,  $image_params, 'pcp-photo', $pcp->id, $matched_image);

    global $base_url;

    $element = array(
        '#tag' => 'meta',
        '#attributes' => array(
          "property" => "og:image",
          "content" => $base_url . $imageSrc,
        ),
    );

    drupal_add_html_head($element,'facebook_share_image');


    
    
    $donate_url     = dw_campaigns_get_donate_url($thisUser, $campaign);

    $params = dw_campaigns_get_merge_object($campaign, $pcp);
    $rawurl = dw_campaigns_user_get_pcp_url($thisUser, $campaign, TRUE);

    $url = urlencode($rawurl);
    $text = urlencode( t("Help me support !title", array('!title' => $campaign->title)) );
    $twitter_text = variable_get('dw-campaigns-twitter-text', $text);
    $facebook_text = variable_get('dw-campaigns-facebook-text', $text);

    dw_campaigns_do_merge($twitter_text, $params);
    dw_campaigns_do_merge($facebook_text, $params);
    dw_campaigns_do_merge($text, $params);
    dw_campaigns_do_merge($form['invitation-text'], $params);

    $vid            = NULL;

    $extra          = _dw_campaigns_get_pcp_extra($pcp->id);
    if(isset($extra->youtube_url)) {
        $vid            = dw_campaigns_get_youtube($extra->youtube_url);
    }


    $twitter_pic    = 'http://twitter.com/images/goodies/tweetn.png';
    $facebook_pic   = 'http://facebook.com/images/connect_favicon.png';

    $image_path     = drupal_get_path('module','dw_campaigns');

    $twitter_pic    = '/' . $image_path . "/images/tw_48x48.png";
    $facebook_pic   = '/' . $image_path . "/images/fb_48x48.png";
?>
<div class="users-pcp-page-left">
    <h2><?php echo $pcp->title;?></h2>
    <div class="image-box">
    <div class="image-box-inner">
<?php
    $image_style = '';

    if(!empty($vid)) {

        $skip   = false;

        if($mode_type != 'walking') {
            if($extra->force_photo_show == 1) {
                $skip   = TRUE;
            }

        }
       
        if(!$skip) { 
            printf('<iframe class="youtube-video" width="260" height="225" src="http://www.youtube.com/embed/%s?rel=0" frameborder="0" allowfullscreen></iframe>', $vid);
            $image_style = 'have-video';
        }
    }

    printf('<img src="%s" class="%s">', $imageSrc, $image_style);
?>
    </div>
    </div>
<?php
    if($mode_type == 'walking') {   
        echo theme('dw_campaigns_derby_event_details', array('node' => $campaign));
    }
?>
</div>
<div class="users-pcp-page-right">
    <div class="share-this-cause">
        <!-- <div class="title"><?php echo t('Share this cause'); ?></div> -->
        <ul>
            <li><?php echo t('Share'); ?></li>
            <li>
                <a class="twitter_share" target="_blank" href="http://twitter.com/share?url=<?php echo $url; ?>&text=<?php echo $twitter_text; ?>"> <img src="<?php echo $twitter_pic; ?>"></a>
            </li>
            <li>
                <a class="facebook_share" target="_blank" href="http://www.facebook.com/sharer.php?u=<?php echo $url; ?>&t=<?php echo $facebook_text; ?>"> <img src="<?php echo $facebook_pic; ?>"></a>
            </li>
<?php
$facebook = 1; 
    if($facebook == 0) {
?>
            <li>
                <iframe style="overflow: hidden; border: 0px none; width: 82px; height: 25px;" frameBorder="0" src="//www.facebook.com/plugins/like.php?href=<?php echo $url; ?>&amp;layout=button_count&amp;show_faces=false&amp;width=100&amp;action=like&amp;font=arial&amp;layout=button_count"></iframe>
            </li>
<?php 
    } else {
?>
        <li><fb:like send="true" layout="button_count" width="150" show_faces="true"></fb:like></li>
<?php
    }
?>
        </ul>
<!--
        <script type="text/javascript" src="http://s7.addthis.com/js/250/addthis_widget.js#username=xa-4d587a2e68371ba0"></script>
-->
    </div>
    <div class="intro-text"><?php echo $pcp->intro_text;?></div>
<?php
    if($mode_type!='walking') { 
?>
    <div class="progress-bars">
        <div class="left">
            <?php 
               echo theme('dw_campaigns_derby_pcp_statistics', array('thisUser' => $thisUser, 'campaign' => $campaign, 'pcp' => $pcp)); 
            ?>
        </div>
        <div class="right">
            <a href="/<?php echo $donate_url;?>" class="btn btn-yellow"><?php echo t('Donate Now!'); ?></a>
        </div>
    </div>
<?php
    }
?>
</div>
