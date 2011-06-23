#!/usr/bin/php
<?php

// recheck completion of every datafile each execution :  false - faster - mimics NOD32; true - slower, safer - for unstable networks
$force_check = false;

// clean-up old unused *.nup files - saves disk space:  false | true
$clean_unused = true;

// show individual downloaded files in log:  false | true
$debug = true;

// unrar method - empty - use internal php mod_rar v2 or higher; use any string as an external command - 'unrar-free'
// - external command will be called eg. `unrar-free -x /tmp/file.rar /tmp/`
$unrar_method = 'unrar';

// list of update servers to use
$eset_server_list = '
update.eset.com
um10.eset.com
93.184.71.27
um12.eset.com
89.202.157.227
um14.eset.com
62.67.184.68
um16.eset.com
90.183.101.10
um18.eset.com
89.202.149.36
um10.eset.com
93.184.71.27
um12.eset.com
89.202.157.227
um14.eset.com
89.202.149.36
um16.eset.com
u47.eset.com
u48.eset.com
u49.eset.com
um10.eset.com
93.184.71.27
um12.eset.com
89.202.157.227
um14.eset.com
89.202.149.36
';

// local shared folder
$base_path = '/opt/share/www';

// update path suffix
$base_suffix = '/eset_upd';

// eg. we can access `http://nod32.server.tld` to physical path `/var/www/nod32` [$base_path]
// if [base_suffix] = `/3`, it means we need to enter `http://nod32.server.tld/3` into NOD32 update path
// beware, you must use a rewrite or apache virtualhost if you don't want to mix your web with NOD32 files
//  - NOD32 uses absolute path /download to get its data files

// temporary path, must exist and be writeable
$tmp = '/tmp';

// Username and password sent to you by e-mail from ESET
$user = '';
$pass = '';

$exclude_langs=array('bgr','nor','fin','dan','sve','rom','ukr','slv','sky','ptb','nld','rus','hrv','hun','trk','deu','csy','fra','plk','esn','cht','chs','ita');
///////////////////////////////////////////////
//      DO NOT EDIT BELOW THIS LINE
///////////////////////////////////////////////


echo '------------------------'."\n"
	.'-  NodUpdate by Ashus  -'."\n"
	.'-  Updated by PrinceAMD  -'."\n"
	.'------------------------'."\n"."\n"
	.'Launched: '.date('Y-m-d H:i:s')."\n";

// check all files if last update was not successfull
if (is_file($tmp.'/nod32update.force-check.tmp'))
	{
	echo 'Note: Full forced check detected from previous unsuccessful run.'."\n";
	$force_check = true;
	unlink($tmp.'/nod32update.force-check.tmp');
	}

// prepare list of eset update servers
$eset_server_list = explode("\n",trim($eset_server_list));

function SelectRandomServer()
	{
	global $eset_server_list, $eset_server;
    $eset_server = trim($eset_server_list[mt_rand(0,count($eset_server_list)-1)]);
	}

// curl init
$ch = curl_init();
curl_setopt($ch, CURLOPT_HEADER, false);
curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
curl_setopt($ch, CURLOPT_CONNECTTIMEOUT, 30);
curl_setopt($ch, CURLOPT_TIMEOUT, 900);
curl_setopt($ch, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_1);
curl_setopt($ch, CURLOPT_USERAGENT, 'ESS Update (Windows; U; 32bit; VDB 6825; BPC 4.0.474.0; OS: 6.1.7600 SP 0.0 NT; CH 1.1; LNG 1029; x64c; UPD AUTOSELECT; APP ess; BEO 1; CPU 161612; ASP 0.0)');
curl_setopt($ch, CURLOPT_HTTPHEADER, array('X-NOD32-Mode: passive'));


// download update.ver meta info
for ($i=3;$i>0;$i--)
	{
    SelectRandomServer();

	if ($debug)
		{echo ' - trying to download http://'.$eset_server.'/eset_upd/update.ver'."\n";}

	curl_setopt($ch, CURLOPT_URL, 'http://'.$eset_server.'/eset_upd/update.ver');
	$data = curl_exec($ch);

	if (!$data)
		{
		echo 'Could not connect to server '.$eset_server.'.'."\n";
		if ($i==1)
			{die('Maximum tries reached. Bye.'."\n");}
		} else {
        echo 'Successfully connected to server '.$eset_server.'.'."\n";
		break;
		}
	}

if ( (is_file($base_path.$base_suffix.'/update.ver')) &&
    (md5_file($base_path.$base_suffix.'/update.ver', true) == md5($data,true)) )
	{

	if ($force_check)
		{
		echo 'Note: Update.ver checksum is the same, however force checking is enabled.'."\n";
		} else {
		die('Update.ver checksum is the same, update not needed. Bye.'."\n");
		}
	}

file_put_contents($tmp.'/nod32update.rar', $data);


if (strlen($unrar_method) == 0)
	{
	// extract update.ver from RAR to variable   uses PECL mod_rar 2.0
	$rar_arch = rar_open($tmp.'/nod32update.rar');
	if ($rar_arch === FALSE)
	    die('Could not extract update.ver. Bye.'."\n");

	list($rar_entry) = rar_list($rar_arch);
	$rar_stream = $rar_entry->getStream();
	$update_ver = stream_get_contents($rar_stream);
	fclose($rar_stream);
	rar_close($rar_arch);

	} else {

	// external exec
	if (is_file($tmp.'/update.ver'))
		{unlink($tmp.'/update.ver');}

	$res3 = exec($unrar_method.' x -o+ "'.$tmp.'/nod32update.rar" "'.$tmp.'/"', $res2, $res);
	if (($res != 0) || (!is_file($tmp.'/update.ver')))
		{
	    die('Update.ver failed to extract ['.$res3.']. Bye.'."\n");
		}
	$update_ver = file_get_contents($tmp.'/update.ver');
	unlink($tmp.'/update.ver');
	}



// find all update files
preg_match_all('`\sfile=(.*\.nup)\s`', $update_ver, $update_files);
$update_files = $update_files[1];

foreach ($exclude_langs as $langs)
{
	$nomatch = preg_grep("/{$langs}/i",$update_files,PREG_GREP_INVERT);
	if($nomatch == NULL)
		 continue;
	$update_files = $nomatch;
	#print_r($nomatch);
}

// download new and updated files
curl_setopt($ch, CURLOPT_USERPWD, $user.':'.$pass);

$all_ok = true;
$updated_files = 0;
$total_size = 0;

foreach ($update_files as $upd_file)
	{
	// download only file info
	curl_setopt($ch, CURLOPT_HEADER, true);
	curl_setopt($ch, CURLOPT_NOBODY, true);
	curl_setopt($ch, CURLOPT_RETURNTRANSFER, true); // resets direct FILE output
	curl_setopt($ch, CURLOPT_URL, 'http://'.$eset_server.$upd_file);
	$data = curl_exec($ch);

	$time_modified = 0;
	if (preg_match('`\sLast\-Modified\: (.*)$`im', $data, $time))
		{
	    $time_modified = strtotime(trim($time[1]));
		} else {
		echo 'Failed to receive modification time of from header of '.$upd_file.'. Downloading again.'."\n";
		}

    $size = -1;
	if (preg_match('`\Content\-Length\: (.*)$`im', $data, $size))
		{
	    $size = (float) (trim($size[1]));
		} else {
		echo 'Failed to receive filesize from header of '.$upd_file.'.'."\n";
		}

	if ( (! is_file($base_path.$upd_file)) ||
		($time_modified < filemtime($base_path.$upd_file)) ||
		($size != filesize($base_path.$upd_file)) )
		{

		// download complete file
		if ($debug)
			{echo ' - downloading: '.$upd_file.''."\n";}

		curl_setopt($ch, CURLOPT_HEADER, false);
		curl_setopt($ch, CURLOPT_NOBODY, false);

		$dir = dirname($base_path.$upd_file);
		if (! is_dir($dir))
			{mkdir($dir, 0777, true);}

		$df = fopen($base_path.$upd_file, 'w');
		curl_setopt($ch, CURLOPT_FILE, $df);
		curl_exec($ch);
        fclose($df);

        clearstatcache();
        $size_real = filesize($base_path.$upd_file);
		$updated_files++;

		if ($size_real == $size)
			{
			touch($base_path.$upd_file, $time_modified);
			} else {
            touch($base_path.$upd_file, ($time_modified-3600));
			$all_ok = false;
			echo 'Warning: Filesize of '.$upd_file.' is different than expected, will be retried on next run.'."\n";
			}

        $total_size += $size_real;
		}
    }

// update meta file
$dir = $base_path.$base_suffix;
if (! is_dir($dir))
	{mkdir($dir, 0777, true);}
rename($tmp.'/nod32update.rar', $base_path.$base_suffix.'/update.ver');


function bytes($a) {
    $unim = array("B","KB","MB","GB","TB","PB");
    $c = 0;
    while ($a>=1024) {
        $c++;
        $a = $a/1024;
    }
    return number_format($a,($c ? 2 : 0),","," ")." ".$unim[$c];
	}

if (! $all_ok)
	{
	echo 'Note: Forcing full check on next run.'."\n";
	file_put_contents($tmp.'/nod32update.force-check.tmp','');
	}

echo (($updated_files>0)?'Successfully updated '.$updated_files.' files in size of '.bytes($total_size).'.'
		:'All files successfully checked.')."\n";


// clean the old, outdated nup files
if ($clean_unused)
	{
	$removed_files = 0;
    $total_size = 0;

	function file_array($path, $exclude = ".|..", $include = '*.nup')
		{
	    $path = rtrim($path, "/") . "/";
	    $folder_handle = opendir($path);
	    $exclude_array = explode("|", $exclude);
	    $result = array();
	    while(false !== ($filename = readdir($folder_handle))) {
	        if (!in_array(strtolower($filename), $exclude_array))  {
	            if(is_dir($path.$filename."/")) {
	                $result = array_merge($result, file_array($path.$filename, $exclude, $include));
	            	} else {
					if (preg_match('`^'.strtr(preg_quote($include,'`'),array('\*'=>'.*?', '\?'=>'.')).'$`i', $filename))
	                	{$result[] = $path.$filename;}
	            	}
	        	}
	    	}
	    return $result;
		}

	$existing_files = file_array($base_path);

	foreach ($existing_files as $curfile)
		{
		$curfile = str_replace($base_path, '', $curfile);
		if (! in_array($curfile, $update_files, true))
			{
            $size = filesize($base_path.$curfile);
			if (unlink($base_path.$curfile))
                {
				$removed_files++;
				$total_size += $size;
				} else {
				echo 'Could not remove unused file ('.$curfile.').'."\n";
				}
			}
		}

	echo 'Clean-up complete.'.(($removed_files>0)?' '.$removed_files.' files in size of '.bytes($total_size).' were removed.':' No files were removed.')."\n";
	}

?>
