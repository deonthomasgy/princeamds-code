<?php
/*
@author Deon 'PrinceAMD' Thomas : Email PrnceAMD.Elive@Gmail.com
@ copywrite 2009 - 2011
@ License = GPL
*/

//DKG = Deon's Krazy Gallery

//Table info
/*
CREATE TABLE IF NOT EXISTS `DKG` (
  `image_id` mediumint(8) NOT NULL AUTO_INCREMENT,
  `image_dir` varchar(128) NOT NULL,
  `image_height` int(5) NOT NULL,
  `image_width` int(5) NOT NULL,
  `image_type` varchar(20) CHARACTER SET latin1 COLLATE latin1_bin NOT NULL,
  `image_size` varchar(32) CHARACTER SET latin1 COLLATE latin1_bin NOT NULL,
  `image_date` date NOT NULL,
  `num_of_image` int(5) NOT NULL,
  `thumb_height` int(5) NOT NULL,
  `thumb_width` int(5) NOT NULL,
  `image_name` varchar(1024) NOT NULL,
  `thumb_name` varchar(512) NOT NULL,
  PRIMARY KEY (`image_id`)
) ENGINE=MyISAM  DEFAULT CHARSET=utf8 AUTO_INCREMENT=1 ;

*/
$GLOBALS['title'] = "DKG 2011 -  Page Name";
$GLOBALS['title2'] = "DKC 2011 - Page Title";
$GLOBALS['slogan'] = "Clean Pictures Always";
$GLOBALS['time_zone'] = "America/Guyana";
$GLOBALS['db_name'] = "SMC";
$GLOBALS['db_table'] = "TABLE";
$GLOBALS['db_server'] = "SQL_SERVER";
$GLOBALS['db_user'] = "SQL_USER";
$GLOBALS['db_pass'] = "SIMPLEPASSWORD_HAHA";
$GLOBALS['gal_path'] = "gallery";
$GLOBALS['thumb_path'] = "thumb";
$GLOBALS['path_name_prev'] = NULL;
$GLOBALS['[path_name_next'] = NULL;
/*
 * values: normal, normal_width, normal_height, random
 */
$GLOBALS['order'] = 'normal_height';
$GLOBALS['path_name'] = NULL;
$GLOBALS['index'] = NULL;
$GLOBALS['item_col'] = 6;
$GLOBALS['item_row'] = 4;
$GLOBALS['thumb_row'] = 3;
$GLOBALS['copyright'] = "2009 - 2011 Deon 'PrinceAMD' Thomas";
?>
