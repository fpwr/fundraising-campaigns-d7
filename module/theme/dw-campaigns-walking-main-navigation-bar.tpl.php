<?php
    global $selected;
    global $language;

    $campaign_id = $selected;

    $derby_url          = '';

    // TODO - lookup homepage campaign - this may need to be changed to be like 'leaderboard' with current
    $toplocations_url   = '/dw/walking/toplocations';
    $leaderboard_url    = '/dw/campaign/current/leaderboard';
    $countries_url      = '/dw/walking/topcountries';

    $events_url         = '/dw/walking/event_list';

    $find_participant   = '/dw/campaign/current/search';
    $info_for_donors    = '/node/125';
    $matching_gifts     = 'http://www1.matchinggifts.com/onesmallstep_fpwr/';

    $give_url           = variable_get('dw_campaigns_derby_general_donation_link', '');
    $give_enabled       = variable_get('dw_campaigns_give_enabled', 1);

    $give_enabled = false;


    $about_url  = variable_get('dw_campaigns_derby_about_np_link', '#');
    if($language->language != 'en') {
        $about_url = '/node/86';
    }



    if($selected != -1 && $selected != '' && !(arg(2) == 'give' && arg(3) == 'general-donation')) {
   
        $leaderboard_url  = dw_campaigns_get_campaign_path($selected, '/dw/campaign', 'leaderboard');
    }


    
    $active     = 'active-path';
    
    $home_class         = '';
    $leaderboard_class  = '';
    $account_class      = '';

    $body_class         = dw_campaigns_make_body_class();
    if($body_class == 'dw-walking') {
        $home_class         = $active;
    } elseif (arg(3) == 'leaderboard') {
        $leaderboard_class  = $active;
    } elseif (arg(1) == 'user') {
        $account_class      = $active;
    } elseif ($body_class == 'dw-walking-toplocations') {
        $toplocations_class = $active;
    }
?>
<div class="nav-bar">
    <ul>
        <li class="navhome <?php echo $home_class; ?>">
            <a href="/dw/walking"><?php echo t('Home'); ?></a>
        </li>
        
        <li class="navleaderboard <?php echo $leaderboard_class; ?>">
            <a href="<?php echo $toplocations_url; ?>"><?php echo t('Leader Board'); ?></a>
            <ul>
                <li>
                    <a href="<?php echo $toplocations_url; ?>"><?php echo t('Locations'); ?></a>
                </li>
                <li>
                    <a href="<?php echo $leaderboard_url; ?>"><?php echo t('Fundraisers'); ?></a>
                </li>
                <li>
                    <a href="<?php echo $countries_url; ?>"><?php echo t('Countries'); ?></a>
                </li>
            </ul>
        </li>

        <li class="navevents">
            <a href="<?php echo $events_url; ?>"><?php echo t('Events'); ?></a>
        </li>

        <?php 
        if(1==2 && !empty($give_url) && $give_enabled) {
        ?>
        <li>
            <a href="<?php echo $give_url; ?>"><?php echo t('Give'); ?></a>
        </li>
        <?php
        }
        ?>
        <li class="navdonate">
            <a href="<?php echo $find_participant; ?>"><?php echo t('Donate'); ?></a>
            <ul>
                <li>
                    <a href="<?php echo $find_participant; ?>"><?php echo t('Find a Participant'); ?></a>
                </li>
                <li>
                    <a href="<?php echo $info_for_donors;?>"><?php echo t('Info for Donors'); ?></a>
                </li>
        <?php 
        if(!empty($matching_gifts)) { 
        ?>
                <li>
                    <a href="<?php echo $matching_gifts;?>"><?php echo t('Matching Gifts'); ?></a>
                </li>
        <?php 
        }

        if(!empty($give_url) && $give_enabled) {
        ?>
                <li>
                    <a href="<?php echo $give_url; ?>"><?php echo t('Give'); ?></a>
                </li>
        <?php
        }
        ?>
            </ul>
        </li> 
        <li class="navlearn">
            <a href="<?php echo $about_url; ?>"><?php echo t('Learn More'); ?></a>
            <ul>
                <li>
                    <a href="<?php echo $about_url; ?>"><?php echo t('About Us'); ?></a>
                </li>
                <li>
                    <a href="/inTheMedia">In the Media</a>
                </li>
                <!--<li>
                    <a href="/node/451"><?php echo t('OSS Funded Research'); ?></a>
                </li>
                <li>
                    <a href="/content/research-plan"><?php echo t('Research Plan'); ?></a>
                </li>-->
                <li>
                    <a href="/node/126"><?php echo t('Host A Walk'); ?></a>
                </li>
                <li>
                    <a href="/content/community-partnership"><?php echo t('Community Partnership'); ?></a>
                </li>
                <li>
                    <a href="/node/61"><?php echo t('Contact Us'); ?></a>
                </li>
            </ul>
        </li>
        
        <?php
        if($user->uid != 0) {
        ?>
        
        <li class="<?php echo "navlearn"//$account_class; ?>">
            <a href="/dw/user/profile"><?php echo t('My Account'); ?></a>

            <ul>
                <li><a href="test">Fundraising Tips</a></li>
            </ul>

        </li>
        <?php
        }
        if(1==2) {
        ?>
<li class="merchandise last">
	<link rel="stylesheet" type="text/css" href="https://cache.audiolife.com/widget/PoppyV1/history/history.css" />
	<script src="https://cache.audiolife.com/widget/PoppyV1/AC_OETags.js" language="javascript"></script>
	<script src="https://cache.audiolife.com/widget/PoppyV1/history/history.js" language="javascript"></script>
	<script type="text/javascript" src="https://ajax.googleapis.com/ajax/libs/jqueryui/1.8.16/jquery-ui.min.js"></script>
	<link href="https://ajax.googleapis.com/ajax/libs/jqueryui/1.7.0/themes/base/jquery-ui.css" type="text/css" media="screen" rel="stylesheet" />

	<script type="text/javascript">
		var gtUserId = 288027;
		var gtPartnerID = "FPWR";
		var gtThemeName = "NewBlueTheme";
		var gtCustomizeButtonName = "Add to Cart";
		var gtShowFeedBackLink = "false";
		var gtStoreName = 'FPWR';
		var gtStoreTitle = 'Buy Personalized Merchandise';
		var gtStoreURL = '/PoppyV1/FPWR';
		var gtButtonTitle = 'Merchandise';
		var gtStoreMode = 'PopUp';
		var gtTitleBarBgColor = "#c6c6c6";
		var gtAutoOpenStoreInPopup = false;
		var gtGoodthreadsVersion = "1";

                var $ = jQuery;
	</script>
	<!--<script src="https://cache.audiolife.com/widget/PoppyV1/gtScript.js" language="JavaScript" type="text/javascript"></script>-->
	<script src="/sites/all/modules/dw_campaigns/gtScript.js" language="JavaScript" type="text/javascript"></script>
	<div id="GoodThreadsContent" ></div>
</li>
<?php
        }
?>
        <li class="facebook_like last">
            <div> 
                <iframe src="//www.facebook.com/plugins/like.php?href=http%3A%2F%2Fonesmallstep.fpwr.org%2F&amp;send=false&amp;layout=button_count&amp;width=120&amp;show_faces=false&amp;action=like&amp;colorscheme=light&amp;font&amp;height=21" scrolling="no" frameborder="0" style="border:none; overflow:hidden; width:120px; height:21px;" allowTransparency="true"></iframe>
            </div>
        </li>
    </ul>
</div>
