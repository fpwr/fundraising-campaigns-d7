SET SQL_MODE="NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;

--
-- Database: `dw_walking_2012`
--

-- --------------------------------------------------------

--
-- Table structure for table `dw_campaigns_address_book`
--

CREATE TABLE IF NOT EXISTS `dw_campaigns_address_book` (
  `address_id` int(11) NOT NULL auto_increment,
  `owner_uid` int(11) NOT NULL,
  `email` char(120) character set utf8 collate utf8_unicode_ci NOT NULL,
  `name` char(120) character set utf8 collate utf8_unicode_ci NOT NULL,
  `date_added` timestamp NOT NULL default CURRENT_TIMESTAMP on update CURRENT_TIMESTAMP,
  `hidden` char(1) NOT NULL default '0',
  PRIMARY KEY  (`address_id`),
  UNIQUE KEY `owner_uid` (`owner_uid`,`email`)
) ENGINE=MyISAM  DEFAULT CHARSET=utf8 AUTO_INCREMENT=13234 ;

-- --------------------------------------------------------

--
-- Table structure for table `dw_campaigns_address_book_status`
--

CREATE TABLE IF NOT EXISTS `dw_campaigns_address_book_status` (
  `address_id` int(11) NOT NULL,
  `invite_dates` text collate utf8_unicode_ci NOT NULL,
  `pcp_id` int(11) NOT NULL,
  PRIMARY KEY  (`address_id`),
  KEY `pcp_id` (`pcp_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- --------------------------------------------------------

--
-- Table structure for table `dw_campaigns_campaigns_to_contribution_pages`
--

CREATE TABLE IF NOT EXISTS `dw_campaigns_campaigns_to_contribution_pages` (
  `nid` int(11) NOT NULL,
  `contribution_page_id` int(11) NOT NULL,
  `event_page_id` int(11) default NULL,
  `active` char(1) NOT NULL default 'Y',
  PRIMARY KEY  (`nid`,`contribution_page_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure for table `dw_campaigns_checkout_swap`
--

CREATE TABLE IF NOT EXISTS `dw_campaigns_checkout_swap` (
  `tid` int(11) NOT NULL auto_increment,
  `hash` char(128) NOT NULL,
  `data` text NOT NULL,
  PRIMARY KEY  (`tid`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=924 ;

-- --------------------------------------------------------

--
-- Table structure for table `dw_campaigns_contribution_receipts_USD`
--

CREATE TABLE IF NOT EXISTS `dw_campaigns_contribution_receipts_USD` (
  `rid` int(10) unsigned NOT NULL auto_increment,
  `contribution_id` int(11) NOT NULL default '-1',
  `tax_receipt_sent` int(11) NOT NULL default '0',
  PRIMARY KEY  (`rid`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=11536 ;

-- --------------------------------------------------------

--
-- Table structure for table `dw_campaigns_donation_privacy`
--

CREATE TABLE IF NOT EXISTS `dw_campaigns_donation_privacy` (
  `privacy_id` int(11) NOT NULL auto_increment,
  `civi_contact_id` int(11) NOT NULL,
  `soft_id` int(11) NOT NULL,
  `pcp` int(11) NOT NULL,
  `options` text NOT NULL,
  PRIMARY KEY  (`privacy_id`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=12290 ;

-- --------------------------------------------------------

--
-- Table structure for table `dw_campaigns_drupal_civi_contact_mapping`
--

CREATE TABLE IF NOT EXISTS `dw_campaigns_drupal_civi_contact_mapping` (
  `mapping_id` int(11) NOT NULL auto_increment,
  `drupal_id` int(11) NOT NULL,
  `civi_contact_id` int(11) NOT NULL,
  PRIMARY KEY  (`mapping_id`)
) ENGINE=MyISAM  DEFAULT CHARSET=utf8 AUTO_INCREMENT=2003 ;

-- --------------------------------------------------------

--
-- Table structure for table `dw_campaigns_event_participants`
--

CREATE TABLE IF NOT EXISTS `dw_campaigns_event_participants` (
  `campaign_id` int(11) NOT NULL,
  `participant_type` char(80) NOT NULL default 'adult_pledge',
  `pcp_id` int(11) NOT NULL,
  `participants` int(11) NOT NULL,
  `contact_id` int(11) NOT NULL default '0',
  `children` int(11) NOT NULL default '0',
  `adults` int(11) NOT NULL default '0',
  `age` int(11) NOT NULL default '0',
  `gender` ENUM( 'M', 'F' ) NULL DEFAULT NULL COMMENT 'Null / Male / Female',
  KEY `campaign_id` (`campaign_id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Table structure for table `dw_campaigns_generaldonation_mapping`
--

CREATE TABLE IF NOT EXISTS `dw_campaigns_generaldonation_mapping` (
  `campaign_id` int(11) NOT NULL,
  `contact_id` int(11) NOT NULL,
  PRIMARY KEY  (`campaign_id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Table structure for table `dw_campaigns_groups`
--

CREATE TABLE IF NOT EXISTS `dw_campaigns_groups` (
  `group_id` int(11) NOT NULL auto_increment,
  `group_name` varchar(128) NOT NULL,
  `group_tag_id` int(11) NOT NULL,
  PRIMARY KEY  (`group_id`)
) ENGINE=MyISAM  DEFAULT CHARSET=utf8 AUTO_INCREMENT=2 ;

-- --------------------------------------------------------

--
-- Table structure for table `dw_campaigns_offline_donation`
--

CREATE TABLE IF NOT EXISTS `dw_campaigns_offline_donation` (
  `offline_id` int(10) unsigned NOT NULL auto_increment,
  `contribution_id` int(11) NOT NULL default '-1',
  `deleted` int(11) NOT NULL default '0',
  `responsible_drupal_id` int(11) NOT NULL default '0',
  `remote_addr` char(20) NOT NULL default '',
  `currency` char(20) NOT NULL default '',
  `first_name` char(200) NOT NULL default '',
  `last_name` char(200) NOT NULL default '',
  `email` char(200) NOT NULL default '',
  `address_1` char(200) NOT NULL default '',
  `city` char(200) NOT NULL default '',
  `state` char(200) NOT NULL default '',
  `postal_code` char(200) NOT NULL default '',
  `country` char(200) NOT NULL default '',
  `pcp_id` char(20) NOT NULL default '',
  `campaign` char(20) NOT NULL default '',
  `include_in_honor_roll` char(20) NOT NULL default '',
  `donation_amount` char(20) NOT NULL default '',
  `payment_instrument` char(20) NOT NULL default '',
  `payment_check_number` char(20) NOT NULL default '',
  `receive_date` char(64) NOT NULL default '',
  `trxn_id` varchar(255) character set utf8 collate utf8_unicode_ci default NULL,
  `non_deductible` INT( 11 ) NOT NULL DEFAULT '0',
  PRIMARY KEY  (`offline_id`),
  KEY `deleted` (`deleted`)
) ENGINE=MyISAM  DEFAULT CHARSET=utf8 AUTO_INCREMENT=3905 ;

-- --------------------------------------------------------

--
-- Table structure for table `dw_campaigns_pcp_extra`
--

CREATE TABLE IF NOT EXISTS `dw_campaigns_pcp_extra` (
  `id` int(11) NOT NULL auto_increment,
  `pcp_id` int(11) NOT NULL,
  `youtube_url` char(120) default NULL,
  `force_photo_show` int(11) NOT NULL default '0',
  PRIMARY KEY  (`id`),
  UNIQUE KEY `pcp_id` (`pcp_id`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=4203 ;

-- --------------------------------------------------------

--
-- Table structure for table `dw_campaigns_photos_mapping`
--

CREATE TABLE IF NOT EXISTS `dw_campaigns_photos_mapping` (
  `fid` int(11) NOT NULL,
  `type` varchar(127) NOT NULL,
  `resource_id` int(11) NOT NULL,
  UNIQUE KEY `type_id` (`type`,`resource_id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Table structure for table `dw_campaigns_request_services`
--

CREATE TABLE IF NOT EXISTS `dw_campaigns_request_services` (
  `nid` int(11) NOT NULL,
  `ins_location` text NOT NULL,
  `ins_ai_name` text NOT NULL,
  `ins_ai_address` text NOT NULL,
  `ins_event_date` char(20) NOT NULL,
  `ins_status` int(11) NOT NULL default '0',
  `shirt_event_date` char(20) NOT NULL,
  `shirt_latest_delivery_date` char(20) NOT NULL,
  `shirt_quantity` int(11) NOT NULL,
  `shirt_special_requests` text NOT NULL,
  `shirt_shipping_address` text NOT NULL,
  `shipping_status` int(11) NOT NULL default '0',
  `lit_custom_brochure` int(11) NOT NULL default '0',
  `lit_custom_brochure_qty` int(11) NOT NULL,
  `lit_custom_flier` int(11) NOT NULL default '0',
  `lit_custom_flier_qty` int(11) NOT NULL,
  `lit_fpwr_brochure` int(11) NOT NULL default '0',
  `lit_fpwr_brochure_qty` int(11) NOT NULL,
  PRIMARY KEY  (`nid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure for table `dw_campaigns_scheduled_host_contacts`
--

CREATE TABLE IF NOT EXISTS `dw_campaigns_scheduled_host_contacts` (
  `eid` int(11) NOT NULL auto_increment,
  `schedule_type` char(32) collate utf8_unicode_ci NOT NULL default '',
  `day_offset` int(11) NOT NULL default '0',
  `action_date` char(10) collate utf8_unicode_ci NOT NULL default '',
  `from_name` char(128) collate utf8_unicode_ci NOT NULL default '',
  `email_subject` char(250) collate utf8_unicode_ci NOT NULL default '',
  `email_text` text collate utf8_unicode_ci NOT NULL,
  `enabled` char(10) collate utf8_unicode_ci NOT NULL default '1',
  `deleted` int(1) NOT NULL default '0',
  PRIMARY KEY  (`eid`)
) ENGINE=MyISAM  DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci AUTO_INCREMENT=59 ;

-- --------------------------------------------------------

--
-- Table structure for table `dw_campaigns_thankyou`
--

CREATE TABLE IF NOT EXISTS `dw_campaigns_thankyou` (
  `thanks_id` int(11) NOT NULL auto_increment,
  `pcp_id` int(11) NOT NULL,
  `contribution_id` int(11) NOT NULL,
  `status` char(20) character set utf8 collate utf8_unicode_ci NOT NULL COMMENT 'values can be "sent" or "offline"',
  `last_modified` timestamp NOT NULL default CURRENT_TIMESTAMP,
  PRIMARY KEY  (`thanks_id`),
  UNIQUE KEY `contribution_id` (`contribution_id`),
  KEY `pcp_id` (`pcp_id`)
) ENGINE=MyISAM  DEFAULT CHARSET=utf8 AUTO_INCREMENT=867 ;

-- --------------------------------------------------------

--
-- Table structure for table `dw_campaigns_uploaded_photos`
--

CREATE TABLE IF NOT EXISTS `dw_campaigns_uploaded_photos` (
  `fid` int(11) NOT NULL auto_increment,
  `filepath` varchar(250) NOT NULL,
  `owner` int(11) NOT NULL,
  `for_nid` int(11) NOT NULL default '0',
  `status` char(1) NOT NULL,
  PRIMARY KEY  (`fid`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=3788 ;

-- --------------------------------------------------------

--
-- Table structure for table `dw_campaigns_user_notify_settings`
--

CREATE TABLE IF NOT EXISTS `dw_campaigns_user_notify_settings` (
  `drupal_id` int(10) NOT NULL,
  `notify_on_donation` int(1) NOT NULL,
  `notify_on_donation_additional_emails` text NOT NULL,
  `notify_weekly_report` int(1) NOT NULL,
  `notify_campaign_progress` int(1) NOT NULL,
  UNIQUE KEY `drupal_id` (`drupal_id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
