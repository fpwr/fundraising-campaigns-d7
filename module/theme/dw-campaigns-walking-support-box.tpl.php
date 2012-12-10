<?php
    global $options;
    global $language;

    $campaign       = dw_campaigns_get_selected_campaign();

// If we need to display different homepage / location pages this will load a different file for locations
/*
if(!is_null($campaign)) {
    $campaign_id    = $campaign->nid;
    include_once('dw-campaigns-walking-support-box.tpl.php_orig');
    return;
}
*/

    $campaign_id    = isset($campaign->nid) ? $campaign->nid : NULL;

    $hasPCP = false;   

    $my_location    = '-1';

    if($campaign == '') {
        $search_url     = '/dw/campaign/current/search';
    } else {
        $search_url     = dw_campaigns_get_campaign_path($campaign_id, '/dw/campaign', 'search');    
    }

    $pcps = _dw_campaigns_get_pcp_by_drupal_id($user);


    $campaigns  = dw_campaigns_get_active_campaigns(TRUE);
    foreach($pcps as $key => $pcp) {

       $contribution_page_id   = $pcp->contribution_page_id;
       $myCampaign             = isset($campaigns[$contribution_page_id]) ? $campaigns[$contribution_page_id] : NULL;

       if(!is_null($myCampaign)) {
           $my_location = $myCampaign->nid;
           $hasPCP = TRUE;
           break;
       }
    }

    $location_create_url = '#';
    $location_create_class  = 'location-found';
    
    if(!is_null($campaign)) {
        $location_text          = $campaign->field_dw_campaign_location['und']['0']['value'];
    } else {
        $location_text = ''; // "Select a Location from the 'Find a Walk' menu";
    }

    // default behavior    
    $location_create_url        = "/dw/walking/start/$campaign_id";
    $location_create_link_words = '<span class="register-now">' . t('Register Now') . '</span>';

    // if we don't have a user, lets guide them through the process
    if($user->uid <= 0) {
        $location_create_url    = '/dw/user/register_oss';
    } else {
        $location_create_link_words = 'Create My Page';    
    }

// 2012 change
    $location_create_url    = '/dw/user/register_oss';

    if(empty($campaign_id) && $user->uid > 0) {
       
        $location_create_class  = '';
	//$location_create_url    = '/dw/user/edit_page';
	$location_create_url    = '/dw/user/register_oss';
    }

 
    if($hasPCP && ($hasPCP && ($campaign_id == $my_location) ) ) {
        // we are on the right page
        //$location_create_url        = "/dw/user/edit_page";            
        $location_create_url        = "/dw/user/register_oss";            
        $location_create_link_words = t('Edit My Page');
    } elseif ($hasPCP && ($campaign_id != $my_location)) {

        // we are on the wrong page - we do the class in the middle of the page currently :)
        $location_create_url        = "/dw/user/edit_page";            
        $location_create_link_words = t('Edit My Page');
        $location_create_class  = "location-mismatch";    
	$location_create_class  = '';
    }

    // 350 x 245 is about the size of the box
    $background_image = '';
    if(!is_null($campaign)) {
        //$temp_filename = $campaign->field_dw_campaign_image['und']['0']['filepath'];
        $temp_filename =  isset($campaign->field_dw_campaign_image['und'][0]['uri']) ? $campaign->field_dw_campaign_image['und'][0]['uri'] : NULL;
        $temp_filename = str_replace("public://", "sites/default/files/", $temp_filename);

        if(substr($temp_filename, -1, 1) != '/' && file_exists($temp_filename)) {
            $image_params = array(
                'w' => 692,
            );
            $thumb              = _dw_campaigns_thumb($temp_filename, $image_params);
            $background_image   = "background:transparent url('$thumb') no-repeat left top;";
        }

        $vid_link   = isset($campaign->field_dw_campaign_youtube['und']['0']['value']) ? $campaign->field_dw_campaign_youtube['und']['0']['value'] : NULL;

        $vid        = dw_campaigns_get_youtube($vid_link);
    } 

    $contest_link = '/content/oss-contest-rules?ajax=1';

    if($language->language != 'en') {
        $contest_link = '/node/93?ajax=1';
    }

?>
<script type="text/javascript">
(function($) {
$(document).ready(function() {
    
    var my_loc = <?php echo $my_location;?>;
    
    $(".location-not-found").click(function(){
        alert('Please Select a Location from the Find a Walk Menu before Creating A Fundraising Page');
        return false;
    });
    
    $(".location-mismatch").click(function(e){
        var res = confirm('<?php echo t('You already have a Walking Campaign for a Different Location - Click OK to Go to your Walk Location'); ?>');
        if(res) {
            window.location = "/dw/walking/location/" + my_loc 
        }
        return false;
    });

});
})(jQuery);
</script>

<div class="support-left" style="<?php echo $background_image; ?>">
<?php 
if(!empty($vid)) {
    //printf('<iframe class="youtube-video" width="393" height="325" src="http://www.youtube.com/embed/%s?rel=0&wmode=opaque" frameborder="0" allowfullscreen></iframe>', $vid);
    printf('<iframe class="youtube-video" width="692" height="356" src="http://www.youtube.com/embed/%s?rel=0&wmode=opaque" frameborder="0" allowfullscreen></iframe>', $vid);
} else {
    if(is_null($campaign)) {
        //echo '<iframe style="padding-left:45px;" width="560" height="315" src="http://www.youtube.com/embed/oMKlHgISMB8?wmode=opaque" frameborder="0" allowfullscreen></iframe>';
        //echo '<iframe width="692" height="356" src="http://www.youtube.com/embed/LjsaxWtLPII?wmode=opaque" frameborder="0" allowfullscreen></iframe>';
       echo '<image src="/sites/all/modules/dw_campaigns/images/ossbanner.jpg">';
    }
}
?>
</div>
<div class="support-right">
  <div class="right">
    <ul style="padding-left:8px">
    <li class="register">
      <a href="<?php echo $location_create_url; ?>" class="menu-large">
      <div class="menu-item-word-outter"><div class="menu-item-word-inner"><?php echo t($location_create_link_words); ?></div></div>
      </a>
    </li>

    <li class="donate" style="margin-top:2px">
      <a href="/dw/campaign/current/search" class="menu-normal">
      <div class="menu-item-word-outter"><div class="menu-item-word-inner"><?php echo t('Donate to a Walker'); ?></div></div>
      </a>
    </li>
<!--
    <li class="request" style="margin-top:2px">
      <a href="/dw/contact?ajax=1" class="fb menu-normal">
      <div class="menu-item-word-outter"><div class="menu-item-word-inner"><?php echo t('Request More Info'); ?></div></div>
      </a>
    </li>
-->
    <li class="host" style="margin-top:2px">
      <a href="/dw/walking/host/create" class="menu-normal">
      <div class="menu-item-word-outter"><div class="menu-item-word-inner"><?php echo t('Host a Walk'); ?></div></div>
      </a>
    </li>
    <li class="prizes" style="margin-top:2px">
      <a href="<?php echo $contest_link;?>" class="fb menu-normal">
      <div class="menu-item-word-outter"><div class="menu-item-word-inner"><?php echo t('Fundraising Prizes'); ?></div></div>
      </a>
    </li>
    </ul>
  </div>
</div>
