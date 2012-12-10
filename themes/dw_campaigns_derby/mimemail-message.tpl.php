<?php 

/**
 * @file
 * Default theme implementation to format an HTML mail.
 *
 * Copy this file in your default theme folder to create a custom themed mail.
 * Rename it to mimemail-message--[key].tpl.php to override it for a
 * specific mail.
 *
 * Available variables:
 * - $recipient: The recipient of the message
 * - $subject: The message subject
 * - $body: The message body
 * - $css: Internal style sheets
 * - $key: The message identifier
 *
 * @see template_preprocess_mimemail_message()
 */
?>
<html>
  <head>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <?php if ($css): ?>
    <style type="text/css">
      <!--
      <?php print $css ?>
--></style><?php endif; ?>
   </head>
  <body id="mimemail-body" <?php if ($key): print 'class="'. $key .'"'; endif; ?>>
    <div id="center">
      <div id="main">
<table border="0" width="100%" cellpadding="2" cellspacing="0" >
	<tr style="background-color:#1d415a" >
	<td style="width:290px;height:84px;"><a href="http://www.pwsderby.com"><img src="http://www.pwsaco.org/sites/default/files/pwsderbyLogo.png" width="100%" alt="PWS Derby 2012" border="0" /></a></td>
    <td align="right" valign="bottom" style="color:#FFF"><i>connect with us today</i>!&nbsp;&nbsp;&nbsp;<a href ="https://www.facebook.com/pages/Prader-Willi-Syndrome-Association-of-Colorado/49937919903"><img src="http://www.pwsaco.org/sites/default/files/facebook.png" width="27" height="26" alt="facebook" border="0"/></a>&nbsp;&nbsp;<a href="https://plus.google.com/s/prader%20willi%20syndrome%20colorado"><img src="http://www.pwsaco.org/sites/default/files/googleplus.png" width="27" height="26" alt="google+"border="0" /></a>&nbsp;&nbsp;</td>
</tr>
</table>          


	<blockquote>
        <blockquote>
        <?php print $body ?>
        </blockquote>
       </blockquote>
     </div>
      </div>
   
  </body>
</html>
