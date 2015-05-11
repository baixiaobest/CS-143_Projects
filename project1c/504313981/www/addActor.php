<!DOCTYPE html>
<head>
	<title>Internet Movie Database System</title>
	<link rel="stylesheet" type="text/css" href="main.css">
</head>

<body>
	<nav>
		<ul>
			<li><a href="index.php">Home</a></li>
			<li><a href="addActor.php" class="selected">Add Actor</a></li>
			<li><a href="addMovie.php">Add Movie</a></li>
			<li><a href="addActorToMovie.php">Add Actor To Movie</a></li>
		</ul>
	</nav>
	<div class="margin-40"></div>

	<section class="contents">
		<?php
			$firstName=$lastName=$sex=$dateOfBirth=$formattedDob=$dateOfDie=$formattedDod=$message="";
			$db_connection="";
			$newId;
			if($_SERVER["REQUEST_METHOD"] == "GET"){
				$firstName = $_GET["first"];
				$lastName = $_GET["last"];
				$sex = $_GET["sex"];
				$dateOfBirth = $_GET["dob"];
				$dateOfDie = $_GET["dod"];
				if ($firstName=="" || $lastName=="" || $dateOfBirth=="") {
					$message="Missing Required Input";
				}else{
					$db_connection = mysql_connect("localhost", "cs143", "");
					mysql_select_db("CS143", $db_connection);

					#get max id
					$newIdRow = mysql_fetch_row(mysql_query("select id from MaxPersonID", $db_connection));
					$newId = $newIdRow[0];

					#date
					$time = strtotime($dateOfBirth);
					$formattedDob = date('Ymd', $time);
					if($time==FALSE){
						$message = "Invalid Date";
					}
					if($dateOfDie!=""){
						$time = strtotime($dateOfDie);
						$formattedDod = date('Ymd', $time);
						if($time==FALSE){
							$message = "Invalid Date";
						}
					}
				}
			}
		?>

		<form action="<?php echo htmlspecialchars($_SERVER["PHP_SELF"]);?>" method="GET">
			First Name:	<input type="text" name="first" maxlength="20" size="15"><br/>
			Last Name:	<input type="text" name="last" maxlength="20" size="15"><br/>
			Sex:		<input type="radio" name="sex" value="Male" checked="true">Male
						<input type="radio" name="sex" value="Female">Female<br/>
						
			Date of Birth:	<input type="text" name="dob" size="14"><br/>
			Date of Die:	<input type="text" name="dod" size="15"><br/><br/>
			<input type="submit" value="Submit"/>
		</form>
		<div class="margin-40"></div>
		<div class="output">
		<?php
			echo "<p>".$message."</p>";
			if($db_connection!="" && $message==""){
				echo "<p>id ".$newId." is assigned to</br>";
				echo $firstName." ".$lastName."</br>";
				echo $sex."</br>";
				echo $formattedDob."</br>";
				echo $formattedDod."</p>";

				if($formattedDod=="") $formattedDod="NULL";
				#insert new actor
				$insert = "Insert Into Actor Values(".$newId.",'".$lastName."','".$firstName."','".$sex."',".$formattedDob.",".$formattedDod.");";
				mysql_query($insert, $db_connection);
				echo $insert;

				#set increment id
				$incrementId="Update MaxPersonID set id = id + 1;";
				mysql_query($incrementId, $db_connection);

				$firstName=$lastName=$sex=$dateOfBirth=$formattedDob=$dateOfDie=$formattedDod=$message="";
			}
			if($db_connection!=""){
					mysql_close($db_connection);
			}
		?>
		</div>
	</section>
</body>