<?php
include "config.ini.php";
date_default_timezone_set($serv_timezone);   
mysql_connect($database_server, $database_username, $database_password) or die(mysql_error());
mysql_select_db($database_name) or die(mysql_error());

_execute_db_update($KG_Path,$KG_Thumb);

$thumb_array[] = array();
$image_array[] = array();
$super_array[] = NULL;

function _execute_db_update($KG_Path,$KG_Thumb)
{	
	global $super_array;
	global $database_table;
	printc("Updating Database ...");
	mysql_query("TRUNCATE TABLE $database_table");
	$files = array();
	if (is_dir($KG_Path)) 
	{
		if ($dh = opendir($KG_Path)) 
		{
			while (($file = readdir($dh)) !== false) 
			{
				if($file!="." AND $file!="..")
				{
					$type = filetype("$KG_Path/". $file);
						
					if ( $type == 'dir' )
					{
						_get_contents_indir($KG_Path,$KG_Thumb,$file);
					}
				} 
			}
		closedir($dh);
		}
	}
	//print_r($super_array);
	_insert_to_sql($super_array);
	printc("Updating Database Complete");
}

function _get_contents_indir($KG_Path,$KG_Thumb,$filex)
{
	global $database_table;
	global $thumb_array;
	global $super_array;
	$dir = "$KG_Path/$filex";

	$num_image = 0;
	if ($dh = opendir($dir)) 
	{
		while (($file = readdir($dh)) !== false) 
		{
			if (strpos($file, '.jpg',1) ) 
			{ 
				$num_image = $num_image + 1;
			}
		}
	}
	
	if ($dh = opendir($dir)) 
	{
		while (($file = readdir($dh)) !== false) 
		{
			if (strpos($file, '.jpg',1) ) 
			{ 
				$thumb_array[] = md5("$filex/$file");
				$image_info = getimagesize("$dir/$file");
				
				$stats = stat("$dir/$file");
				$image_file_size = round($stats[7]/1024,2);
				$image_date = date("Y-m-d",$stats[9]);
				$tstat = thumb_size($image_info[1],$image_info[0],512);
				
				$super_array[] = array (
							"image_dir" => $filex,
							"image_height" => $image_info[1],
							"image_width" => $image_info[0],
							"image_type" => $image_info['mime'],
							"image_size" => $image_file_size,
							"image_date" => $image_date,
							"num_of_image" => $num_image,
							"thumb_height" => $tstat['th'],
							"thumb_width" => $tstat['tw'],
							"image_name" => $file,
							"thumb_name" => "$file"
							);
			} 
		}
		closedir($dh);
	}
}

function thumb_size($image_height, $image_width, $size)
{
	$aspectRatio=(float)($image_width / $image_height);
	if ( $image_height > $size )
	{
		$image_height = $size;
		$image_width = (int) ($image_height*$aspectRatio);
	}
	//printc(" $image_width ");
	return array (
			"th" => $image_height,
			"tw" => $image_width
			);
}

function _insert_to_sql($ar)
{
	global $KG_Path;
	global $database_table;
	$x = 1;
	$sql = "INSERT INTO $database_table (image_dir , image_name ,image_height , image_width , thumb_height , thumb_width , image_type , image_size , image_date , num_of_image , thumb_name) VALUES \n";
	foreach ( $ar as $st )
	{
		$sql .= "('$st[image_dir]' , '$st[image_name]' , '$st[image_height]' , '$st[image_width]' , '$st[thumb_height]' ,  '$st[thumb_width]' , '$st[image_type]' , '$st[image_size]' , '$st[image_date]' , '$st[num_of_image]' , '$st[thumb_name]')";
		
		echo "convert -geometry  $st[thumb_width] gallery/$st[image_dir]/$st[image_name] thumb/$st[thumb_name] \n";	
		
		if ( $x < count($ar) )
		{
			$sql .= ", \n";
		}
		$x++;
	}
	mysql_query($sql) or die(mysql_error());
}

function printc($text)
{
	echo "$text \n";
}

?>
