<!DOCTYPE html>
<head>
	<link rel="stylesheet" type="text/css" href="main.css">
</head>

<body>
	<nav>
		<ul>
			<li><a href="index.php">Home</a></li>
			<li><a href="addActor.php">Add Actor</a></li>
			<li><a href="addMovie.php" class="selected">Add Movie</a></li>
			<li><a href="addActorToMovie.php">Add Actor To Movie</a></li>
			<li><a href="search.php">Search</a></li>
		</ul>
	</nav>
	<div class="margin-100"></div>
	<section class="contents">
		<?php
			$title=$company=$year=$formattedYear=$rating=$genre=$message="";
			$db_connection="";
			$newMovieId;
			if($_SERVER["REQUEST_METHOD"] == "GET"){
				$title = $_GET["title"];
				$company = $_GET["company"];
				$year = $_GET["year"];
				$rating = $_GET["mpaarating"];
				$genre = $_GET["genre"];
				if ($title==""){
					$message = "Please Complete the Form";
				}else{
					$db_connection = mysql_connect("localhost", "cs143", "");
					mysql_select_db("CS143", $db_connection);

					$newIdRow = mysql_fetch_row(mysql_query("select id from MaxMovieID", $db_connection));
					$newMovieId = $newIdRow[0];
				}
			}
		?>

		<form action="<?php echo htmlspecialchars($_SERVER["PHP_SELF"]);?>" method="GET">			
			Title : <input type="text" name="title" maxlength="20"><br/>
			Company: <input type="text" name="company" maxlength="50" size="16"><br/>
			Year : <input type="text" name="year" maxlength="4"><br/>	<!-- Todo: validation-->	
			MPAA Rating : <select name="mpaarating" style="width: 145px;">
					<option value=""></option>
					<option value="G">"G"</option>
					<option value="NC-17">"NC-17"</option>
					<option value="PG">"PG"</option>
					<option value="PG-13">"PG-13"</option>
					<option value="R">"R"</option>
					</select>
			<br/>
			Genre :
					<select name="genre" style="width: 210px">
					<option  value=""></option>
					<option  value="Action">Action</option>
					<option  value="Adult">Adult</option>
					<option  value="Adventure">Adventure</option>
					<option  value="Animation">Animation</option>
					<option  value="Comedy">Comedy</option>
					<option  value="Crime">Crime</option>
					<option  value="Documentary">Documentary</option>
					<option  value="Drama">Drama</option>
					<option  value="Family">Family</option>
					<option  value="Fantasy">Fantasy</option>
					<option  value="Horror">Horror</option>
					<option  value="Musical">Musical</option>
					<option  value="Mystery">Mystery</option>
					<option  value="Romance">Romance</option>
					<option  value="Sci-Fi">Sci-Fi</option>
					<option  value="Short">Short</option>
					<option  value="Thriller">Thriller</option>
					<option  value="War">War</option>
					<option  value="Western">Western</option>
					</select>
			<br/>
			<br/>
			<input type="submit" value="Submit"/>
			</form>

			<div class="output">
			<?php
				echo "<p>".$message."</p>";

				if($db_connection!="" && $message==""){
					echo "<p> Assign id ".$newMovieId." to ".$title."<br/>";
					echo "Produced by company ".$company." in ".$year."<br/>";
					echo "rated ".$rating."<br/>";
					echo "genre ".$genre."</p>";

					#insert movie
					$title = "'".$title."'";
					if($company=="") $company="NULL"; else $company="'".$company."'";
					if($year=="") $year="NULL";
					if($rating=="") $rating="NULL"; else $rating = "'".$rating."'";
					if($genre=="") $genre="NULL"; else $genre = "'".$genre."'";
					$insertMovie = "Insert Into Movie Values(".$newMovieId.",".$title.",".$year.",".$rating.",".$company.");";
					mysql_query($insertMovie, $db_connection);
					#echo $insertMovie;

					#insert genre for Movie
					if ($genre!="NULL"){
						$linkMovieGenre = "Insert Into MovieGenre Values(".$newMovieId.",".$genre.");";
						mysql_query($linkMovieGenre, $db_connection);
						#echo $linkMovieGenre;
					}

					#set increment id
					$incrementId="Update MaxMovieID set id = id + 1;";
					mysql_query($incrementId, $db_connection);

					$title=$company=$year=$formattedYear=$rating=$genre=$message="";
				}
				if($db_connection!=""){
					mysql_close($db_connection);
				}
			?>
		</div>
	</section>

</body>