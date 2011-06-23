<?php
require_once "config.inc.php";
_dkg_timezone_set($GLOBALS['time_zone']);
_dkg_connect_mysql($GLOBALS['db_server'],$GLOBALS['db_user'],
    $GLOBALS['db_pass']);
_dkg_select_db($GLOBALS['db_name']);
require_once "theme.php";

function _dkg_timezone_set($time_zone)
{
    date_default_timezone_set($time_zone);
}

function _dkg_connect_mysql($server, $user, $pass)
{
    mysql_connect($server, $user, $pass) or
        die (mysql_error());
}

function _dkg_select_db($db_selected)
{
    mysql_select_db($db_selected) or
        die (mysql_error());
}

function _dkg_printf($print)
{
    echo "<p>$print</p>\n";
}

/*
 * This functions checks _POST for an change in order
 * and applys it to the GLOBALS
 */

function _dkg_order_get()
{
    if(isset($_GET['order']))
        $GLOBALS['order'] = $_GET['order'];
    else if ( $GLOBALS['order'] == NULL )
        $GLOBALS['order'] = "normal";
}

function _dkg_index_get()
{
    if(isset($_GET['index']))
        $GLOBALS['index'] = $_GET['index'];
    else if( $GLOBALS['index'] == NULL )
        $GLOBALS['index'] = 0;
}

function _dkg_table_set($value)
{
    if($value == "open" )
    {
        echo "<table>";
    }
    else if($value == "close")
    {
        echo "</table> \n";
    }
    else
        _dkg_printf("_dkg_table_set:Value Err");
}

function _dkg_table_row_group_set($value)
{
    if($value == "open" )
    {
      	echo "<tr>";
    }
    else if($value == "close")
    {
        echo "</tr> \n";
    }
    else
        _dkg_printf("_dkg_table_row_group_set:Value Err");
}

function _dkg_table_row_group_item_set($value)
{
    if($value == "open" )
    {
      	echo "<td>";
    }
    else if($value == "close")
    {
        echo "</td> \n";
    }
    else
        _dkg_printf("_dkg_table_row_group_item_set:Value Err");
}

function _dkg_table_url_set($path_name,$path_images,$thumb_image,$index)
{
    echo "<p>\n<a href='index.php?path=$path_name&index=$index#image'><img src='$GLOBALS[thumb_path]/$thumb_image'  width=244px align=center /></a>\n</p>\n";
    echo "<p>\n<a href='index.php?path=$path_name&index=$index#image' class='float-left' />"._dkg_label_processor($path_name,24)."</a>\n";
    echo "<p>\n<b class='float-right' />$path_images Images</b>\n";
}

function _dkg_label_processor($text,$char_to_print)
{
	$num = count_chars($text,1);
	$char_count = 0;
	$init = 0;
	$new_label = array();
	foreach ($num as $value)
	$char_count = $char_count + $value ;

	if($char_count > $char_to_print)
	{
		$old_label = str_split($text);

		while($init != $char_to_print)
		{
			$new_label[] = $old_label[$init];
			$init = $init + 1;
		}
		return implode('',$new_label);
	}
	else
		return $text;
}

function _dkg_folder_image_get($path_name,$order)
{
	if($order)
	{
	    $query = sprintf("SELECT thumb_name, thumb_width, thumb_height
			FROM %s
            WHERE image_dir 
            LIKE '%s'
            ORDER BY image_name ASC",
            mysql_real_escape_string($GLOBALS['db_table']),
			mysql_real_escape_string("$path_name"));
	}
	if($order == "random")
	{
    	$query = sprintf("SELECT thumb_name
            FROM %s
			WHERE image_dir='%s'
            ORDER BY RAND() LIMIT 1",
            mysql_real_escape_string($GLOBALS['db_table']),
			mysql_real_escape_string("$path_name"));
	}
	$result = mysql_query($query);
    $init_name = NULL; 
    while($row = mysql_fetch_array($result))
    {
        if(!$init_name)
            $init_name = $row['thumb_name'];
        $data[] = array(
            'name' => "".$row['thumb_name']."",
            'width' => "".$row['thumb_width']."",
            'height' => "".$row['thumb_height'].""
        );
    }
    natsort2d($data);
    foreach($data as $value)
    {
        $w = $value['width'];
        $h = $value['height'];
        $name = $value['name'];
        if($w > $h AND $order == "normal_width")
            return $name;
        
        if($w < $h AND $order == "normal_height")
            return $name;
    }
	return $init_name;
}

function natsort2d(&$aryInput) {
  $aryTemp = $aryOut = array();
  foreach ($aryInput as $key=>$value) {
    reset($value);
    $aryTemp[$key]=current($value);
  }
  natsort($aryTemp);
  foreach ($aryTemp as $key=>$value) {
    $aryOut[] = $aryInput[$key];
  }
  $aryInput = $aryOut;
}

function _dkg_init_page_selected_go($path_name,$index)
{
    if(!isset($_GET['path']) AND isset($GLOBALS['index']))
        return 0;
    $total_items = $GLOBALS['item_col'] * $GLOBALS['item_row'];

    $query = sprintf("SELECT DISTINCT image_dir
			FROM %s
            ORDER BY image_date DESC 
            LIMIT %s , %s",
            mysql_real_escape_string($GLOBALS['db_table']),
            mysql_real_escape_string(($index * $total_items)),
            mysql_real_escape_string($total_items)
        );

    $result = mysql_query($query);
     while($row = mysql_fetch_array($result))
     {
        $data[] = array(
            "path_name" => "".$row['image_dir']."",
        );
     }
     natsort2d($data);
     $prev = NULL;
     $next = NULL;
     $curr = NULL;
     $end = NULL;
     $start = NULL;

     foreach($data as $key => $value)
     {
         if($value['path_name'] == $_GET['path'])
         {
             if($key == $total_items -1)
                 $end = 1;
             if($key == 0)
                 $start = -1;
                 
             $curr = $value['path_name'];
         }
         if($curr == NULL) continue;
         if($key-1 >= 0)
             $prev = $data[$key-1]['path_name'];
         if($key+1 < $total_items)
         $next = $data[$key+1]['path_name'];
         break;
     }
     // _dkg_printf($prev);
     if($start == -1)
     {
        $query_p = sprintf("SELECT DISTINCT image_dir
		   	FROM %s
            ORDER BY image_date DESC 
            LIMIT %s , %s",
            mysql_real_escape_string($GLOBALS['db_table']),
            mysql_real_escape_string((($index-1) * $total_items)),
            mysql_real_escape_string($total_items)
        );

         $result_p = mysql_query($query_p);
         while($row = mysql_fetch_array($result_p))
         {
            $data_p[] = array(
                "path_name" => "".$row['image_dir']."",
            );
         }
         natsort2d($data_p);
         $prev = $data_p[$total_items-1]['path_name'];
     }

     if($end == 1)
     {
        $query_n = sprintf("SELECT DISTINCT image_dir
		   	FROM %s
            ORDER BY image_date DESC 
            LIMIT %s , %s",
            mysql_real_escape_string($GLOBALS['db_table']),
            mysql_real_escape_string((($index+1) * $total_items)),
            mysql_real_escape_string($total_items)
        );

         $result_n = mysql_query($query_n);
         while($row = mysql_fetch_array($result_n))
         {
            $data_n[] = array(
                "path_name" => "".$row['image_dir']."",
            );
         }
         natsort2d($data_n);
         $next = $data_n[0]['path_name'];
     }
     
     echo "<p><a href='index.php?path=$prev&index=".($index+$start)."'><img src='images/go-previous.png' width='48' height='48' class='float-left' /></a></p>\n";
     echo "<p><a href='index.php?path=$next&index=".($index+$end)."'><img src='images/go-next.png' width='48' height='48' class='float-right' /></a></p>\n";
}

function _dkg_init_pages_go($index)
{
    /*
     * FIXME: This code is bad because every refresh
     * it will lookup sql bad bad bad, need another way to calc
     */
   	$query = sprintf("SELECT DISTINCT image_dir
			FROM %s
			ORDER BY image_date DESC",
			mysql_real_escape_string($GLOBALS['db_table']));

	$result = mysql_query($query);
    $num = mysql_num_rows($result);
    $it = $index * ($GLOBALS['item_row'] * $GLOBALS['item_col']);
    $sum = $num - $it;

    if(isset($_GET['path'])) return 0;
    if($index > 0 && $sum > $GLOBALS['item_row'] * $GLOBALS['item_col'] )
    {
        echo "<p><a href='index.php?index=".($index-1)."'><img src='images/go-previous.png' width='48' height='48' class='float-left' /></a></p>\n";
		echo "<p><a href='index.php?index=".($index+1)."'><img src='images/go-next.png' width='48' height='48' class='float-right' /></a></p>\n";
    }
    else if ($index == 0)
    {
        echo "<p><a href='index.php?index=".($index+1)."'><img src='images/go-next.png' width='48' height='48' class='float-right' /></a></p>\n";
    }
    else
    {
        echo "<p><a href='index.php?index=".($index-1)."'><img src='images/go-previous.png' width='48' height='48' class='float-left' /></a></p>\n";
    }
}

function _dkg_init_main($db_table,$gal_path,$thumb_path,$order,$index)
{
    $total_items = $GLOBALS['item_col'] * $GLOBALS['item_row'];

    $query = sprintf("SELECT DISTINCT image_dir,num_of_image
        FROM %s
        ORDER BY image_date DESC LIMIT %s , %s",
        mysql_real_escape_string($db_table),
        mysql_real_escape_string($index * $total_items),
        mysql_real_escape_string($total_items));

    $result = mysql_query($query);
    $return = mysql_num_rows($result);
    $return_items = $return;
    while($row = mysql_fetch_array($result))
    {
        $data[] = array(
            "path_name" => "".$row['image_dir']."",
            "path_images" => "".$row['num_of_image'].""
        );
    }

    $table_item = 0;
    natsort2d($data);
    _dkg_table_set("open");
    foreach ($data as $value)
    {
        /*
         * Here we now build our table
         */
        $table_count = 1;

        while($table_count <= $total_items)
        {
            $open = $table_count * $GLOBALS['item_row'] - $GLOBALS['item_row'];
            $close = $table_count * $GLOBALS['item_row'];

            if($open == $table_item)
                _dkg_table_row_group_set("open");
            if($close == $table_item)
                _dkg_table_row_group_set("close");

            $table_count++;
        }
        $table_item++;
        $thumb = _dkg_folder_image_get($value['path_name'],$GLOBALS['order']);
        $GLOBALS['path_name'] = $value['path_name'];

        _dkg_table_row_group_item_set("open");
        _dkg_table_url_set($value['path_name'],$value['path_images'],$thumb,$index);
        _dkg_table_row_group_item_set("close");
    }
    _dkg_table_set("close");
}

function _dkg_init_images_main($path_name,$index)
{
    $query = sprintf("SELECT image_name,thumb_width,
        thumb_height,image_size,image_date,image_type,
        thumb_name,image_width,image_height
		FROM %s
		WHERE image_dir
        LIKE '%s' 	",
        mysql_real_escape_string($GLOBALS['db_table']),
		mysql_real_escape_string($path_name));
    $result = mysql_query($query);
	while($row = mysql_fetch_array($result))
    {
        $files[] = array(
            "image_name" => "".$row['image_name']."",
			"thumb_width" => "".$row['thumb_width']."",
			"thumb_height" => "".$row['thumb_height']."",
			"image_size" => "".$row['image_size']."",
			"image_date" => "".$row['image_date']."",
			"image_type" => "".$row['image_type']."",
			"thumb_name" => "".$row['thumb_name']."",
			"image_width" => "".$row['image_width']."",
			"image_height" => "".$row['image_height'].""
		);
    }
    natsort2d($files);
    $total_items = mysql_num_rows($result);
    _dkg_table_set("open");

    $table_item = 0;
    $mark = 0;
    foreach($files as $value)
    {
        $table_count = 1;
           while($table_count <= $total_items)
        {
            $open = $table_count * $GLOBALS['thumb_row'] - $GLOBALS['thumb_row'];
            $close = $table_count * $GLOBALS['thumb_row'];

            if($open == $table_item)
            {
                $mark++;
                _dkg_table_row_group_set("open");
            }
            if($close == $table_item)
                _dkg_table_row_group_set("close");

            $table_count++;
        }
        $table_item++;

        _dkg_table_row_group_item_set("open");
        echo "<a href='$GLOBALS[gal_path]/$path_name/$value[image_name]' class='lightview' rel='gallery[kindgirls]' title='$value[image_name] :: Resolution:".$value['image_width'].'x'.$value['image_height']." | Size: ".round($value['image_size'],2)."KB | Date: $value[image_date] ' ><img src='$GLOBALS[thumb_path]/$value[thumb_name]' width='360'  align=center  /></a>";
        echo "<p>\n<a href='$GLOBALS[gal_path]/$path_name/$value[image_name]' class='float-left' />"._dkg_label_processor($value['image_name'],24)."</a>\n";

        _dkg_table_row_group_item_set("close");
    }
    $marker = $mark * $GLOBALS['thumb_row'] - $total_items;
    while($marker > 0)
    {
        _dkg_table_row_group_item_set("open");
        _dkg_table_row_group_item_set("close");
        $marker--;
    }

    _dkg_table_set("close");

}

function _dkg_init_pages_main()
{
    if(isset($_GET['path']))
    {
        _dkg_init_images_main($_GET['path'],NULL);
    }
    else
        _dkg_init_main($GLOBALS['db_table'],$GLOBALS['gal_path'],$GLOBALS['thumb_path'],$GLOBALS['order'],$GLOBALS['index']);
}
?>
