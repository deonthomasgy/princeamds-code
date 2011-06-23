<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
<head>

<meta name="Description" content="Deon's Krazy Gallery" />
<meta name="Keywords" content="your, keywords" />
<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1" />
<meta name="Distribution" content="Global" />
<meta name="Author" content="Deon Thomas - princeamd.elive@gmail.com" />
<meta name="Robots" content="index,follow" />
<link rel="stylesheet" href="css/CoolWater.css" type="text/css" />
<link rel="stylesheet" type="text/css" href="css/lightview.css" />
<title><?php echo $title?></title>

<script type='text/javascript' src='js/prototype.js'></script>
<script type='text/javascript' src='js/scriptaculous.js'></script>
<script type='text/javascript' src='js/lightview.js'></script>

<script language="JavaScript">
(function()
{ // Import GET Vars
	document.$_GET = [];
	var urlHalves = String(document.location).split('?');
	if(urlHalves[1])
	{
		var urlVars = urlHalves[1].split('&');
		for(var i=0; i<=(urlVars.length); i++)
		{
			if(urlVars[i]){
				var urlVarPair = urlVars[i].split('=');
				document.$_GET[urlVarPair[0]] = urlVarPair[1];
			}
		}
	}
}
)
();

function whichButton(event){
	var key = event.keyCode;
	var id = document.$_GET["index"];
    
    if(id == null)
        var id = 0;

	var prev = id-1;
	var next = eval(2+prev);
    
	if(key == 66 && id != 0 )
		location.href = "index.php?index="+prev;
		
	if(key == 78)
		location.href = "index.php?index="+next;
		
}
</script>
</head>

<body onkeyup="whichButton(event)" >
<!-- wrap starts here -->
<div id="wrap">
		
	<!--header -->
	<div id="header">			
		
		<h1 id="logo-text"><a href="index.php"><?php echo $GLOBALS['title']?></a></h1>		
		<p id="slogan"><?php echo $GLOBALS['slogan']?></p>		
		
		<div id="header-links">
			<p>
			<a href="index.php">Home</a> | 
			<a href="index.php?update">Update</a> 
			</p>		
		</div>	
		
		<div id="middle-links">
			<p>
			<form border="1" name="jumpto" method="post" >
				<select onchange="javascript: submit();" name="s">
					<?php 
					$query = sprintf("SELECT DISTINCT image_date  
					FROM %s
					ORDER BY image_date DESC",
					mysql_real_escape_string($GLOBALS['db_table']));
			
					$result = mysql_query($query);
					while($row = mysql_fetch_array($result))
					{
						$list = explode('-',"".$row['image_date']."");
						$mnth[] = "".$list[0]."-".$list[1]."";
					}
			
						$mnths = array_unique($mnth);
						$uss = 0;
						if(isset($_POST['s']))
							$pvalue = $_POST['s'];
							
						foreach ($mnths as $value)
						{
							$select = "selected=\"selected\"";
							if($uss == 0)
							{
								echo " <option value=$value $select>".date('F Y',strtotime($value))."</option>";
							}
							else
							{
								if(isset($_POST['s']))
								{
									if($value == $pvalue)
										echo " <option value=$value $select>".date('F Y',strtotime($value))."</option>";
									else
										echo " <option value=$value >".date('F Y',strtotime($value))."</option>";
								}
								else
									echo " <option value=$value >".date('F Y',strtotime($value))."</option>";
								
							}
							$uss = $uss + 1;
                        }
				?>
				</select>
			<form>
			</p>		
		</div>	
		
		<div id="options-links">	
			<p>
			<form border="1" name="switch" method="post">
				
				<?php
				//index.php
//				if(isset($_POST['s']))
//					echo "<input type=\"hidden\" name=\"s2\" value=\"$_POST[s]\">";
//				echo "<input type=\"hidden\" name=\"order\" value=\"YES\">";
				if(isset($_GET['index']))
					echo "<button type=submit > <a href=\"index.php?order=random&index=$_GET[index]\" >Shuffle</a> </button>";
				else
					echo "<button type=submit > <a href=\"index.php?order=random\" >Shuffle</a> </button>";
             
				/*if(!isset($_GET['path']) AND !isset($_GET['page']) AND !isset($_GET['shuffle']))
				{
					echo "<a TYPE=\"submit\" href=\"index.php?shuffle\" onclick=\"javascript: submit();\" name=\"d\" value=deon >Shuffle</a>  ";
				}*/
				/*//index.php?shuffle
				else if(!isset($_GET['path']) AND !isset($_GET['page']) AND isset($_GET['shuffle']))
				{
					echo "<a href=\"index.php\">Static</a> | ";
					echo "<a href=\"index.php?shuffle\">Shuffle</a> ";
				}*/
				
		/*		//index.php?path=*&id*
				if(isset($_GET['path']) AND isset($_GET['id']))
				{
					$id = $_GET['id'];
					echo "<a href=\"index.php?page&id=$id\">Back</a>";
				}
				//index.php?path=*
				else if(isset($_GET['path']) AND !isset($_GET['id']))
				{
					echo "<a href=\"index.php\">Back</a>";
                }*/
				/*
				//index.php?page
				if(isset($_GET['page']) AND !isset($_GET['id']) AND !isset($_GET['shuffle']))
				{
					echo "<a href=\"index.php?page&shuffle\">Shuffle</a> ";
				}
				//index.php?page&id
				else if(isset($_GET['page']) AND isset($_GET['id']) AND !isset($_GET['shuffle']))
				{
					$id = $_GET['id'];
					echo "<a href=\"index.php?page&shuffle&id=$id\">Shuffle</a> \n";
				}
				//index.php?page&shuffle
				else if(isset($_GET['page']) AND !isset($_GET['id']) AND isset($_GET['shuffle']))
				{
					echo "<a href=\"index.php?page\">Static</a> | ";
					echo "<a href=\"index.php?page&shuffle\">Shuffle</a> ";
				}
				//index.php?page&shuffle&id
				else if(isset($_GET['page']) AND isset($_GET['id']) AND isset($_GET['shuffle']))
				{
					$id = $_GET['id'];
					if ($id == NULL)
						$id = 0;
					echo "<a href=\"index.php?page&id=$id\">Static</a> | ";
					echo "<a href=\"index.php?page&shuffle&id=$id\">Shuffle</a> ";
				}*/
				?>
			<form>
			</p>
		</div>
		
	</div>
	
	<div id="content-wrap">
		
        <div id="main" >
<?php
echo "<center>";
//_dkg_timezone_set($GLOBALS['time_zone']);
//_dkg_connect_mysql($GLOBALS['db_server'],$GLOBALS['db_user'],
//    $GLOBALS['db_pass']);
//_dkg_select_db($GLOBALS['db_name']);
_dkg_order_get();
_dkg_index_get();
_dkg_table_set("open");
_dkg_init_pages_go($GLOBALS['index']);
_dkg_init_page_selected_go($GLOBALS['path_name'],$GLOBALS['index']);
_dkg_table_set("close");
_dkg_init_pages_main();
_dkg_table_set("open");
_dkg_init_pages_go($GLOBALS['index']);
_dkg_init_page_selected_go($GLOBALS['path_name'],$GLOBALS['index']);
_dkg_table_set("close");
echo "</center>";
?>
		</div>
				
	<!-- content-wrap ends here -->	
	</div>
	
	<!--footer starts here-->
	<div id="footer">
			
		<p>
        &copy; <strong><?php echo $GLOBALS['copyright']?></strong> | 
		Design by: <a href="./">PrinceAMD</a> | 
		Valid <a href="http://validator.w3.org/check?uri=referer">XHTML</a> | 
		<a href="http://jigsaw.w3.org/css-validator/check/referer">CSS</a>   		
   	</p>
				
	</div>	

<!-- wrap ends here -->
</div>

</body>
</html>
