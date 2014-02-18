<?php
global $user;

if(isset($user) && !is_null($user) && isset($thisUser) && !is_null($thisUser) && $user->uid === $thisUser->uid) {
?>
	<ul class="links">
		<li><a href="/dw/user/edit_page"><?php echo t('Edit My Fundraising Page'); ?></a></li>
	</ul>
<?php
} else {
	
}
