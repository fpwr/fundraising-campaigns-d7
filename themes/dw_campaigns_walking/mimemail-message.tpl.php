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
    <style type="text/css">
      <!--
      <?php print $css ?>
img {
border: 0;
}
#footer-message tr {
line-height:16px;
}
#footer-message td {
font-size:9pt;
}
    -->
    </style>
  </head>

  <body id="mimemail-body" <?php if ($key): print 'class="'. $key .'"'; endif; ?>>

  <table width="100%" cellpadding="0" cellspacing="0" border="0">
    <tr>
      <td align="left"><img src="http://onesmallstep.fpwr.org/sites/default/files/email-header3-left.png" border="0" height="50%"></td>
      <td width="100%" style="background: url('http://onesmallstep.fpwr.org//sites/default/files/email-header3-middle.png') repeat-x;"></td>
    </tr>
  </table>
<br><br>
  <div>
    <div>
      <?php print $body ?>
    </div>
  </div>
<br><br>


  <div id="footer-affils">
    <table><tr>
      <td><a href="http://tracking.fpwr.org/tracking/click?msgid=lggi4j-8t030a4g&target=http%3a%2f%2fwww.fpwr.org"><img border="0" height="40" src="http://onesmallstep.fpwr.org/sites/default/files/logo_fpwr.png"></a></td>
      <td><a href="http://tracking.fpwr.org/tracking/click?msgid=lggi4j-8t030a4g&target=http%3a%2f%2fwww.fpwr.ca"><img border="0" height="40" src="http://www.fpwr.ca/wp-content/themes/karmatheme/images/header-logo.gif"></a></td>
      <td><a href="http://tracking.fpwr.org/tracking/click?msgid=lggi4j-8t030a4g&target=http%3a%2f%2fwww.prader-willi.be%2f"><img border="0" height="40" src="http://www.fpwr.ca/wp-content/uploads/2011/04/logo-belgium.png"></a></td>
      <td><a href="http://tracking.fpwr.org/tracking/click?msgid=lggi4j-8t030a4g&target=http%3a%2f%2fwww.prader-willi.fr%2f"><img border="0" height="40" src="http://onesmallstep.fpwr.org/sites/default/files/logoPWF.jpg"></a></td>
      <td><a href="http://tracking.fpwr.org/tracking/click?msgid=lggi4j-8t030a4g&target=http%3a%2f%2fwww.pwsausa.org"><img border="0" height="40" a="42" src="http://www.pwsausa.org/images/PWSA_2c_US.jpg"></a></td>
      <td><a href="http://tracking.fpwr.org/tracking/click?msgid=lggi4j-8t030a4g&target=http%3a%2f%2fwww.fpwr.co.uk">FPWR UK</a></td>
    </tr></table>
  </div>

  <div id="footer-message" style="background-color:#1D415A">
    <table>
    <tr><td>
    <div style="display:inline-block; color:#ffffff;padding-left:20px;font-size:9pt;">
      <table style="color:white">
        <tr><td> The Foundation for Prader-Willi Research </td></tr>
        <tr><td>5455 Wilshire Blvd, Suite 2020</td></tr>
        <tr><td>Los Angeles, CA 90036</td></tr>
      </table>
    </div>
    </td><td>
    <div style="display:inline-block; color:#ffffff;padding-left:20px;font-size:9pt;">
      <table style="color:white">
        <tr><td><a style="color:white;" href="http://www.FPWR.org">www.FPWR.org</a></td></tr>
        <tr><td>Phone (760) 536-3027, (888) 322-5487</td></tr>
        <tr><td>Fax (888) 559-4105</td></tr>
      </table>
    </div>
    </td><td>
      <div style="display:inline-block;vertical-align:top;;padding-left:30px;">
        <img src="http://www.fpwr.ca/wp-content/uploads/2010/01/logo_02.jpg" width="214">
      </div>
    </td>
    </tr></table>
  </div>
   
  </body>
</html>
