<?php 

    $mode_type  = dw_campaigns_get_selected_type();
    $user = $thisUser;
                        
    if($mode_type == 'walking') {
        $signup_page = 'dw/user/register_oss';
    } else {
        $signup_page = 'dw/user/register';
    }

// shamelessly taken from fpwr project
/**
 * If they are not logged in, show custom login form, this will be handled in the init 
 * of our fpwr_contributions module. Opted to go with custom markup so it's easier to
 * match the psd provided by jeff (and who likes fighting with drupals form api anyway)
 */

	if($user->uid == 0) { 

    /**
     * All this is required so drupal will call dw_campaigns_user_login_submit()
     */
    $form_id                    = 'dw_campaigns_user_login_form';
    $form                       = dw_campaigns_user_login_form();
    $form_build_id              = 'form-'. md5(uniqid(mt_rand(), true));
    $form['#build_id'] 	= $form_build_id;
    if(count($_POST) > 0) {
        $form['#post']		= $_POST;
    }
    
    $form_state 		= array('storage' => NULL, 'submitted' => FALSE, 'method' => 'post', 'rebuild' => FALSE, 'cache' => FALSE);
            
    drupal_prepare_form($form_id, $form, $form_state);
    drupal_process_form($form_id, $form, $form_state);

    //remove labels
    unset($form['name']['#title']);
    unset($form['pass']['#title']); 

?>
    <h2><?php echo t('Fundraiser Login'); ?></h2>
	<?php
		if(isset($_REQUEST['create'])) {
			echo '<p>' . t('You must have an account to create a fundraising page.') . '</p>';
                        echo '<p>' . t('Please login below or create an account now ');
                        echo l(t('Sign Up'), $signup_page);
                        echo '</p>';
		}
	?>
    <form class="login" action="<?php echo request_uri(); ?>" method="post">
        <ul>
            <li>
            	<?php echo drupal_render($form['name']); ?>
            </li>
            <li>
            	<?php echo drupal_render($form['pass']); ?>
            </li>
            <li class="submit"><button type="submit" ><?php echo t('Login'); ?></button></li>
        </ul>
<?php
if(isset($_SESSION['last_failed'])) {
    unset($_SESSION['last_failed']);
?>
<div class="login-error"><?php echo t('Login failed. Please try again.');?></div>
<?php
}
?>
		<div class="forgot-signup">
			<a href="/user/password" target="_parent"><?php echo t('Password Reminder'); ?></a>
		<?php
		if(!isset($_REQUEST['create'])) {
		?>
			<?php echo t('No Account Yet?'); ?> <?php echo l(t('Sign Up'), $signup_page, array('attributes' => array('target'=>'_parent')));?>
        <?php
		} 
	?>
		</div>    
		<?php
            echo drupal_render($form['form_id']);
            echo drupal_render($form['form_build_id']);
        ?>
    </form>
<?php } else { ?>
    <h2><?php echo t('Controls'); ?></h2>
    <div class="user-controls">
    	<a href="/user/logout?destination=dw" class="btn" target="_parent"><?php echo t('Logout'); ?></a>
    </div>
<?php } ?>
