<?php
if(dw_campaigns_get_selected_type()!='derby')
    return;
?>
<div class="derby-header-right">
    <div class="account-box">
    <?php
        if($user->uid>0) {
    ?>
            <?php echo t('Signed in as !name', array('!name' => $user->name)); ?> <a href="/user/logout?destination=dw" class="btn"><?php echo t('Logout'); ?></a>
    <?php
            $campaignId = dw_campaigns_get_default_campaign();
            $campaign   = node_load($campaignId);

            $pcps       = _dw_campaigns_get_pcps_for_campaign_keyed_by_contact_ids($campaign);
            $contact_id = _dw_campaigns_user_get_contact_id($user);
            if(isset($pcps[$contact_id])) {
                // drupal 'l' doesn't like leading /'s
                $url        = dw_campaigns_user_get_pcp_url($user, $campaign);
                echo '<a href="' . $url . '" class="goto-page">' . t('Go to my page') . '</a>';
            }
        } else {
            echo l(t('Sign In'),"dw/user/login");        
            echo l(t('Sign Up'),"dw/user/register");
        }
    ?>
    </div>
</div>
