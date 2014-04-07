<?php
    if(dw_campaigns_get_selected_type()!='walking')
        return;

    global $dw_campaign_module_path;
?>

<script type="text/javascript">
    (function ($) {
$(document).ready(function() {
    var do_nav = function(cur_val) {
        if(cur_val != '') {
            if($("body").hasClass('leaderboard')) { 
                window.location.href = "/dw/walking/location/" + cur_val + '/go';
            } else {
                window.location.href = "/dw/walking/location/" + cur_val;
            }
        }
        return false;
    };

    $(".walking-header-bottom .jqTransformSelectWrapper ul li a").click(function(){
        var cur_val = $("#walk_location").val();
	do_nav(cur_val);
    });
    
    $(".walking-header-bottom #walk_location").change(function(){
        var cur_val = $("#walk_location").val();
	do_nav(cur_val);
    });

    $(".support-right .jqTransformSelectWrapper ul li a").click(function(){
        var cur_val = $("#walk_location2").val();
	do_nav(cur_val);
    });

});
})(jQuery);
</script>

<?php

    global $options;
    global $selected; // lets share this with other blocks
    global $location_string;

    $no_location = FALSE;
    
    if(arg(1) == 'user') {
        $no_location = TRUE;
    }   
 
    $locations = dw_campaigns_get_all_campaign_location();
    $location_string = '';

    $doMenu   = FALSE;
    
    if(strstr(dw_campaigns_make_body_class(), 'walking-location') !== FALSE) {
        $selected   = dw_campaigns_get_selected_location();
        $doMenu     = TRUE;
    }
   
    if(arg(1) == 'users' || arg(3) == 'leaderboard' || arg(3) == 'search') {

        $campaign  = dw_campaigns_get_selected_campaign();
        if(!is_null($campaign)) {
            $selected = $campaign->nid;
        }
    }

    if($doMenu) {
        if(is_null($selected)) {
            $selected = -1;
        } else {
            if(isset($locations[$selected])) {
                $campaign = node_load($selected);
                dw_campaigns_set_selected_campaign($campaign);
                $location_string    = $campaign->field_dw_campaign_location['und']['0']['value'];
            } else {
                if($selected != 0) {
                    //echo "invalid node ($selected)";
                }	
                dw_campaigns_set_selected_campaign(NULL);
            }
        }
    }

    $options    = '';

    $locations_by_country = array();

    foreach($locations as $nid => $location) {
        //$options .= sprintf('<option value="%d" %s>%s</option>', $nid, ($nid==$selected)?'selected=selected':'', $location);

        $node = node_load( $nid );
        $country = $node->field_dw_country_grouping['und'][0]['value'];

        if( !isset($locations_by_country[ $country ]) ){
            $locations_by_country[ $country ] = array();
        }

        $locations_by_country[ $country ][ $nid ] = $location;
    }

    ksort( $locations_by_country );


    /*JFN - april 07 2014 1743 - [#hotfix
        'By request of Susan, we are to resort the event listing combo box, and spell out the name of the event countries.
           she wants the categories to look something like:
            - United States -
            - Canada -
            - France -
            - Other -

            previously they were ksorted, and the 'other' element was dropped to the bottom. we're going to apply that pattern here.
        ']*/

    //us element to bottom of the list
    if( $locations_by_country['us'] ){
        $us_element = $locations_by_country['us'];
        unset( $locations_by_country['us'] );
        $locations_by_country['us'] = $us_element;
    }

    //ca element to bottom of the list
    if( $locations_by_country['ca'] ){
        $ca_element = $locations_by_country['ca'];
        unset( $locations_by_country['ca'] );
        $locations_by_country['ca'] = $ca_element;
    }

    if( $locations_by_country['france'] ){
        $france_element = $locations_by_country['france'];
        unset( $locations_by_country['france'] );
        $locations_by_country['france'] = $france_element;
    }

    if( $locations_by_country['other'] ){
        $other_element = $locations_by_country['other'];
        unset( $locations_by_country['other'] );
        $locations_by_country['other'] = $other_element;
    }



    foreach( $locations_by_country as $country => $events ){
        $optionText = $country; //default to the country abbreviation for regression
        switch( $country ){
            case 'us':
                $optionText = 'United States';
                break;

            case 'ca':
                $optionText = 'Canada';
                break;

            case 'france':
                $optionText = 'France';
                break;

            case 'other':
                $optionText = 'Other';
                break;
        }

        $options .= sprintf('<option disabled value="%d" %s>%s</option>', 0, '', '- '.$optionText.' -' );

        foreach( $events as $nid => $location ){
            $options .= sprintf('<option value="%d" %s>%s</option>', $nid, ($nid==$selected)?'selected=selected':'', '&nbsp;&nbsp;&nbsp;'.$location);

        }
    }
?>
<div id="fireContainer"></div>
<div class="walking-header-left">
<div class="nav-logos">
	<a href="/"><div class="header-main-logo"></div></a>
	<a href="http://www.fpwr.org"><div class="header-second-logo"></div></a>
	<a href="http://www.pwsausa.org"><div class="header-third-logo"></div></a>
</div>
<div class="walking-header-right">
    <div class="account-box">
    <?php
        if($user->uid>0) {
    ?>
        <div class="signed-in">Signed in as <?php echo $user->name;?>
    <?php
            $res = dw_campaigns_get_user_pcp_details($user);
            if(!empty($res['url'])) {
                echo '<a href="' . $res['url'] . '" class="goto-page">Go to my page</a>';
            }
    ?>
        </div><a href="/user/logout?destination=dw" class="btn"><?php echo t('Logout'); ?></a>
    <?php
        } else {
            //echo l(t('Login'),"dw/user/login");  		
            echo '<a href="/dw/user/login?ajax=1" class="fb_login">' . t('Login') . '</a>'; 
        }
    ?>
    </div>
</div>
<div class="walking-header-bottom">
    <?php
        if(!empty($location_string)) {
    ?>    
    <span class="locname"><a href="/dw/walking/location/<?php echo $selected; ?>"><?php echo $location_string; ?></a></span>
    <?php
        }
    ?>
<div class="find-a-walk-word"><?php echo t('Find A Walk'); ?></div>
<?php
if(!$no_location) {
?>
    <form method="get" action="/dw/walking/">
        <select name="city" id="walk_location">
            <option value="0"><?php echo t('All Locations'); ?></option>
            <?php echo $options; ?>
        </select>
    </form>
<?php
}
?>
    <a href="/dw/walking/distance-search" class="find-a-walk"><?php echo t('Find a Walk Near Me'); ?></a>
</div>
<div class="languages" style="float:right;">

<?php
    $path = drupal_is_front_page() ? '<front>' : $_GET['q'];
    $languages = language_list('enabled');
    $links = array();
    foreach ($languages[1] as $lang) {
        $flag_image = base_path() . path_to_theme() .'/images/flags/'.$lang->language.'.gif';

        $links[$lang->language] = array(
            'href'       => 'dw/lang/' . $lang->language . '/' . $path,
            'title'      => '<img src="' . $flag_image . '"> ' . $lang->native,
            'language'   => $lang,
            'html'       => TRUE,
            'attributes' => array(
                'id'       => $lang->language,
                'lang'     => $lang->language,
                'title'    => t('Change language to @language.', array('@language' => $lang->native)),
            ),
        );
    }

    drupal_alter('translation_link', $links, $path);

    global $language;

    $current_language = $language->language;
    unset($links[$current_language]);


    echo theme('links', array('links' => $links));

?>
</div>
<div class="facebook-header"> 
</div>
