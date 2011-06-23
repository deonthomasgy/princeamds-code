<?php

//index.php?update
if(isset($_GET['update']))
{
	_execute_db_update($KG_Path,$KG_Thumb);
}
//index.php?path=*
else if(isset($_GET['path']) AND !isset($_GET['image']) AND !isset($_GET['id']))
{
	echo "i am here";
	$path_name = $_GET['path'];
	_get_contents_from_image_dir($path_name,NULL,$KG_Path,$KG_Thumb,'no');
}
//index.php?path=*&image=*
else if (isset($_GET['path']) AND isset($_GET['image']) AND !isset($_GET['id']))
{
	$path_name = $_GET['path'];
	$image_file = $_GET['image'];
	_get_contents_from_image_dir($path_name,$image_file,$KG_Path,$KG_Thumb,'no');
}
//index.php?path=*&id=*
else if(isset($_GET['path']) AND !isset($_GET['image']) AND isset($_GET['id']))
{
	$path_name = $_GET['path'];
	$id = $_GET['id'];
	_get_contents_from_image_dir($path_name,NULL,$KG_Path,$KG_Thumb,$id);
}
//index.php?path=*&id=*&image=*
else if (isset($_GET['path']) AND isset($_GET['image']) AND isset($_GET['id']))
{
	$path_name = $_GET['path'];
	$image_file = $_GET['image'];
	$id = $_GET['id'];
	_get_contents_from_image_dir($path_name,$image_file,$KG_Path,$KG_Thumb,$id);
}
else if(isset($_GET['id']) AND !isset($_GET['path']) AND !isset($_GET['image']))
{
	$id = $_GET['id'];
	_load_folders($KG_Path,$KG_Thumb,'normal',$id);
}
else if(!isset($_GET['id']) AND isset($_POST['s']))
{
	_load_folders($KG_Path,$KG_Thumb,'normal','no','0');
}
else
{
	_load_folders($KG_Path,$KG_Thumb,'normal','yes','0');
}
/*
if(isset($_GET['shuffle']) AND isset($_GET['id']) )
{
	_load_folders($KG_Path,$KG_Thumb,'shuffle','no','no');
}
else if(isset($_GET['shuffle']) AND isset($_GET['page']) AND !isset($_GET['id']))
{
	$page = $_GET['page'];
	if( $page == NULL )
		$page = 'yes';
	_load_folders($KG_Path,$KG_Thumb,'shuffle',$page,'0');
}
else if(isset($_GET['shuffle']) AND !isset($_GET['page']) AND isset($_GET['id']))
{
	$page = $_GET['page'];
	$id = $_GET['id'];
	if( $page == NULL )
		$page = 'yes';
	_load_folders($KG_Path,$KG_Thumb,'shuffle',$page,$id);
}
else if(!isset($_GET['page']) AND isset($_GET['id']))
{
	_load_folders($KG_Path,$KG_Thumb,'normal','yes',$_GET['id']);
}
if(isset($_GET['id']))
{
	$id = $_GET['id'];
	_load_folders($KG_Path,$KG_Thumb,'normal',$id);
}
else if(!isset($_GET['id']) AND isset($_POST['s']))
{
	_load_folders($KG_Path,$KG_Thumb,'normal','no','0');
}
else
{
	_load_folders($KG_Path,$KG_Thumb,'normal','yes','0');
}
*/


function _load_folders($KG_Path,$KG_Thumb,$order,$id)
{
	global $database_table;

	$colums_of_items = 6;
	$items_per_row = 4;
	$total_items = $colums_of_items * $items_per_row ;

	if(isset($_POST['s']) AND $id == 0)
	{
	$post_mnth = $_POST['s'];
	$query = sprintf("SELECT DISTINCT image_dir,num_of_image
			FROM %s
			WHERE image_date
			LIKE '%s'
			ORDER BY image_date ASC",
			mysql_real_escape_string("$database_table"),
			mysql_real_escape_string("$post_mnth%"));
	}
	else if(!isset($_POST['s']) AND $id == 0)
	{
	$query = sprintf("SELECT DISTINCT image_dir,num_of_image
			FROM %s
			ORDER BY image_date DESC LIMIT %s",
			mysql_real_escape_string("$database_table"),
			mysql_real_escape_string("$total_items"));
	}
	else if(!isset($_POST['s']) AND $id > 0)
	{
	$total_offset = $id * $total_items;
	$query = sprintf("SELECT DISTINCT image_dir,num_of_image
			FROM %s
			ORDER BY image_date DESC LIMIT %s OFFSET %s",
			mysql_real_escape_string("$database_table"),
			mysql_real_escape_string("$total_items"),
			mysql_real_escape_string("$total_offset "));
	}
	else
	{
	$query = sprintf("SELECT DISTINCT image_dir,num_of_image
			FROM %s
			ORDER BY image_date DESC",
			mysql_real_escape_string("$database_table"));
	}
	$result = mysql_query($query);
	$num = mysql_num_rows($result);

	while($row = mysql_fetch_array($result))
	{
		$files[] = array(
						"image_dir" => "".$row['image_dir']."",
						"num_of_image" => "".$row['num_of_image'].""
					);
	}



	if(isset($_POST['s']) AND $id == 0)
	{
		if(is_float($num / $items_per_row))
		{
			$dec = explode('.',(round($num / $items_per_row,1)));
			if($dec[1] >= 5)
				$colums_of_items = round($num / $items_per_row,0);
			else
				$colums_of_items = round($num / $items_per_row,0)+1;
		}
		else
			$colums_of_items = $num / $items_per_row;
	}
	else

	$num_of_items = $items_per_row * $colums_of_items;
	$first_item = $num_of_items * ($id);
	$last_item = $num_of_items * ($id+1);
	$count_items = 0;

	if ($num < $total_items)
	{
		$previous = $id - 1;
		echo "<p><a href='index.php?id=$previous'><img src='images/go-previous.png' width='48' height='48' class='float-left' /></a></p>\n";
	}
	else if ($num == $total_items AND $id > 0)
	{
		$previous = $id - 1;
		$next = $id + 1;
		echo "<p><a href='index.php?id=$previous'><img src='images/go-previous.png' width='48' height='48' class='float-left' /></a></p>\n";
		echo "<p><a href='index.php?id=$next'><img src='images/go-next.png' width='48' height='48' class='float-right' /></a></p>\n";
	}
	else
	{
		$next = $id + 1;
		echo "<p><a href='index.php?id=$next'><img src='images/go-next.png' width='48' height='48' class='float-right' /></a></p>\n";
	}

	echo "<center>";
	echo "<table>";
	foreach ($files as $key => $value)
	{

		$label = $value['image_dir'];
		$directory = $value['image_dir'];
		$image_count = $value['num_of_image'];
		$count = 1; //do not change

		while($count <= $colums_of_items)
		{
			$open = $count * $items_per_row-$items_per_row;
			$close = $count * $items_per_row;
			if($open == $count_items)
			{
				echo "<tr>";
			}
			if($close == $count_items)
			{
				echo "</tr>";
			}
			$count++;
		}
		$count_items++;

		if(isset($_POST['shuffle']))
			$folder_image = _get_folder_image($directory,'yes');
		else
			$folder_image = _get_folder_image($directory,'no');

		echo "<td>\n";
		echo "<p>\n<a href='index.php?path=$directory&id=$id#image'><img src='$KG_Thumb/$folder_image'  width=244px align=center /></a>\n</p>\n";
		//echo "<p>\n<a href='index.php?path=$directory&id=$id' class='lightview' rel='ajax' title='$directory :: caption :: fullscreen: true' '><img src='$KG_Thumb/$folder_image'  width=88px align=center /></a>\n</p>\n";

		echo "<p>\n<a href='index.php?path=$directory' class='float-left' />"._process_labels($label,9)."</a>\n";
		echo "<p>\n<b class='float-right' />$image_count Images</b>\n";
		echo "</td>\n";
	}

	echo "</table> \n";
	echo "</center> \n";
	if ($num < $total_items)
	{
		$previous = $id - 1;
		echo "<p><a href='index.php?id=$previous'><img src='images/go-previous.png' width='48' height='48' class='float-left' /></a>\n";
	}
	else if ($num == $total_items AND $id > 0)
	{
		$previous = $id - 1;
		$next = $id + 1;
		echo "<p><a href='index.php?id=$previous'><img src='images/go-previous.png' width='48' height='48' class='float-left' /></a>\n";
		echo "<p><a href='index.php?id=$next'><img src='images/go-next.png' width='48' height='48' class='float-right' /></a>\n";
	}
	else
	{
		$next = $id + 1;
		echo "<p><a href='index.php?id=$next'><img src='images/go-next.png' width='48' height='48' class='float-right' /></a>\n";
	}
}

function _get_folder_image($image_path,$order)
{
	global $database_table;
	if ( $order == 'no')
	{
	$query = sprintf("SELECT thumb_name
			FROM $database_table
			WHERE image_dir='%s'
			ORDER BY image_name ASC LIMIT 1",
			mysql_real_escape_string("$image_path"));
	}
	else
	{
	$query = sprintf("SELECT thumb_name
			FROM $database_table
			WHERE image_dir='%s'
			ORDER BY RAND() LIMIT 1",
			mysql_real_escape_string("$image_path"));
	}
	$result = mysql_query($query);
	$row = mysql_fetch_array($result);
	return $row[0];
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

function _get_contents_from_image_dir($image_path,$image_clicked,$KG_Path,$KG_Thumb,$id)
{
	global $database_table;
	$query = sprintf("SELECT image_name,thumb_width,thumb_height,image_size,image_date,image_type,thumb_name,image_width,image_height
			FROM $database_table
			WHERE image_dir
			LIKE '%s' 	",
			mysql_real_escape_string("$image_path"));
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
	//asort($files);
	natsort2d($files);
	$z = 3;
	$x = 0;
	$y = $z;
	$num = mysql_num_rows($result);
	echo "<a name='image'/>";

	echo "<center>";
	echo "<table>";
	foreach ($files as $value)
	{

		$image = $value['image_name'];
		$width = $value['thumb_width'];
		$height = $value['thumb_height'];
		$thumb = $value['thumb_name'];
		$image_height = $value['image_height'];
		$image_width = $value['image_width'];
		$image_date = $value['image_date'];
		$image_size = $value['image_size'];

		if($x == $y-$z)
			echo "<tr>";

		echo "<td>";
		echo "<a href='$KG_Path/$image_path/$image' class='lightview' rel='gallery[kindgirls]' title='$image :: Resolution:".$image_width.'x'.$image_height." | Size: ".round($image_size,2)."KB | Date: $image_date ' ><img src='$KG_Thumb/$thumb' width='360'  align=center  /></a>";
		echo "</td>";

		$x = $x + 1;
		if($x == $y)
		{
			echo "</tr>";
			$y = $x + $z;
		}
	}
	$fillin = $y-$num;
	while ($fillin != 0)
	{
		echo "<td>";
		echo "</td>";
		$fillin = $fillin -1;
	}
	echo "</table> ";
	echo "</center>";
}

function _execute_db_update($KG_Path,$KG_Thumb)
{
	echo "<table border='1' class='center'>";
	_process_text_in_form("Updating Database ...");
	echo "</table> ";
	echo "<table border='1' class='center'>";
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
	echo "</table> ";
	echo "<table border='1' class='center'>";
	_process_text_in_form("Updating Database Complete");
	echo "</table> ";
}

function _get_contents_indir($KG_Path,$KG_Thumb,$filex)
{
	global $database_table;
	$dir = "$KG_Path/$filex";
	$num_of_image = NULL;

	if ($img = opendir($dir))
	{
		while (($file = readdir($img)) !== false)
		{
			if($file!="." AND $file!="..")
			{
				$type = filetype("$dir/". $file);
				if($file!="." AND $file!="..")
				{
					if ( $type == 'file' )
					{
						$num_of_image = $num_of_image + 1;
					}
				}
			}
		}
	closedir($img);
	}

	$_path = str_replace("gallery/","",$dir);

	$query = sprintf("SELECT DISTINCT image_dir,num_of_image
			FROM $database_table
			WHERE image_dir='%s' AND num_of_image='%s'",
			mysql_real_escape_string("$_path"),
			mysql_real_escape_string("$num_of_image")
			);
	$result = mysql_query($query);
	$num = mysql_num_rows($result);



	if($num == 0 )
	{
		//if ($num_of_image != 0)
			_process_text_in_form(" Adding $_path/*");

		if ($dh = opendir($dir))
		{
			while (($file = readdir($dh)) !== false)
			{
				if($file!="." AND $file!="..")
				{
					$type = filetype("$dir/". $file);
					if($file!="." AND $file!="..")
					{
						if ( $type == 'dir' )
						{
              // I disabled recursive scanning within directory to stop from lookin in
              // the .svn folder this needs to be fix in the future.
              // uncomment the line below to allow recursion

               // _get_contents_indir("$dir",$KG_Thumb,$file);
						}
						else if ( $type == 'file' )
						{
							_process_update_info("$dir/$file","$filex",$KG_Path,$KG_Thumb,$num_of_image);
						}
					}
				}
			}
		closedir($dh);
		}
	}
}

function _process_update_info($image_name,$image_dir,$KG_Path,$KG_Thumb,$num_of_image)
{
	global $database_table;
	$_path = str_replace("gallery","",$KG_Path);
	$_path = substr($_path,1);
	if ($_path != NULL)
		$_path = "$_path/$image_dir";
	else
		$_path =  $image_dir;

	if ($image_dir != NULL )
	{
		$image_info = getimagesize("$image_name");
		$image_height = $image_info[1];
		if( $image_height != 0)
		{
			$image_width = $image_info[0];
			$aspectRatio=(float)($image_info[0] / $image_info[1]);

			$thumb_height = 512;
			$thumb_width = $thumb_height * $aspectRatio;

			while($thumb_width > 512 )
			{
				$thumb_height = $thumb_height - 1;
				$thumb_width = $thumb_height * $aspectRatio;
			}

		$bs_image_name = basename($image_name);
		$stats = stat($image_name);

		$image_file_size = $stats[7]/1024;
		$image_date = date("Y-m-d",$stats[9]);
		$image_type = $image_info['mime'];

		$md5_name = md5("$_path/$bs_image_name");
		$command =  "convert -geometry $thumb_width '$image_name' $KG_Thumb/$md5_name";
		if (!file_exists("$KG_Thumb/$md5_name"))
		{
			shell_exec($command);
			 _process_text_in_form("Generating Thumb for $bs_image_name $md5_name");
		}

		mysql_query("INSERT INTO
				$database_table (image_dir , image_name ,image_height , image_width , thumb_height , thumb_width , image_type , image_size , image_date , num_of_image , thumb_name)
				VALUES('$_path', '$bs_image_name' , '$image_height' , '$image_width' , '$thumb_height' , '$thumb_width' , '$image_type', '$image_file_size' , '$image_date' , '$num_of_image' , '$md5_name')
				") or die(mysql_error());

		}
	}
}

function _process_text_in_form($text)
{
	echo "<tr>";
	echo "<td>";
		echo "<p><b class='float-left' />$text</b>";
	echo "</td>";
	echo "</tr>";
}

function _process_labels($text,$char_to_print)
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

?>
