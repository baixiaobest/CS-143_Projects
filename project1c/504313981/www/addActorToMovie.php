<!DOCTYPE html>
<head>
	<link rel="stylesheet" type="text/css" href="main.css">
</head>

<body>
	<nav>
		<ul>
			<li><a href="index.php">Home</a></li>
			<li><a href="addActor.php">Add Actor</a></li>
			<li><a href="addMovie.php">Add Movie</a></li>
			<li><a href="addActorToMovie.php" class="selected">Add Actor To Movie</a></li>
		</ul>
	</nav>
	<div class="margin-40"></div>

	<section class="contents">
	<?php
		$title=$first=$last=$role=$message=$mid=$aid="";
		if($_SERVER["REQUEST_METHOD"] == "GET"){
			$title = $_GET["title"];
			$first = $_GET["first"];
			$last = $_GET["last"];
			$role = $_GET["role"];
			if($title=="" || $first=="" || $last=="" || $role==""){
				$message = "Incomplete Input";
			}else{
				$db_connection = mysql_connect("localhost", "cs143", "");
				mysql_select_db("CS143", $db_connection);

				#get mid of movie
				$getMid = "Select id From Movie Where title = '".$title."';";
				$midRow = mysql_fetch_row(mysql_query($getMid, $db_connection));
				$mid = $midRow[0];

				#get aid of actor
				$getAid = "Select id From Actor where first='".$first."' and last='".$last."';";
				$aidRow = mysql_fetch_row(mysql_query($getAid, $db_connection));
				$aid = $aidRow[0];
			}
		}
	?>

	<form action="<?php echo htmlspecialchars($_SERVER["PHP_SELF"]);?>" method="GET">
		Title : <input type="text" name="title" maxlength="20"><br/>	
		First Name:	<input type="text" name="first" maxlength="20" size="15"><br/>
		Last Name:	<input type="text" name="last" maxlength="20" size="15"><br/>
		Role: <input type="text" name="role" maxlength="50" size="20"><br/>
		<br/><br/>
		<input type="submit" value="Submit"/>

	</form>

	<div class="output">
		<?php
			echo "<p>".$message."</p>";

			if($db_connection!="" && $message==""){
				echo "<p> mid".$mid."<br/>";
				echo "aid ".$aid."</p>";

				#insert the info
				$insert = "Insert Into MovieActor Values(".$mid.",".$aid.",'".$role."');";
				mysql_query($insert, $db_connection);
			}

			if($db_connection!=""){
				mysql_close($db_connection);
			}
		?>
	</div>
	</section>

</body>