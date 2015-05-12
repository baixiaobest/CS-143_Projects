<!DOCTYPE html>
<head>
	<title>Internet Movie Database System</title>
	<link rel="stylesheet" type="text/css" href="main.css">
</head>

<body>
	<nav>
		<ul>
			<li><a href="index.php">Home</a></li>
			<li><a href="addActor.php">Add Actor</a></li>
			<li><a href="addMovie.php">Add Movie</a></li>
			<li><a href="addActorToMovie.php">Add Actor To Movie</a></li>
			<li><a href="search.php" class="selected">Search</a></li>
		</ul>
	</nav>

	<section class="contents">

		<div class="margin-100"></div>
		<div class="margin-100"></div>

		<?php
			$searchEntry=$message="";
			$aid=$mid="";
			$actorSearchResult;
			$movieSearchResult;
			$nameResult="";
			$movieResult="";
			$aid = $_GET["aid"];
			$mid = $_GET["mid"];
			if($_SERVER["REQUEST_METHOD"] == "GET"){
				$searchEntry = $_GET["search"];
				$db_connection = mysql_connect("localhost", "cs143", "");
				mysql_select_db("CS143", $db_connection);
				if($searchEntry!=""){
					$aid=""; #remove previous request
					$mid="";

					$keys = explode(" ", $searchEntry);
					#search in Actor
					$nameQuery = "";
					if(count($keys)==1){
						#query first name
						$nameQuery = "Select * From Actor Where first like '%".$keys[0]."%' or last like '%".$keys[0]."%';";
					}else if (count($keys)>1){
						#query last name
						$nameQuery = "Select * From Actor Where first like '%".$keys[0]."%' and last like '%".$keys[1]."%';";
					}
					$nameResult = mysql_query($nameQuery, $db_connection);

					#search in Movie
					$movieQuery = "Select * From Movie Where title like '%".$searchEntry."%';";
					$movieResult = mysql_query($movieQuery, $db_connection);
				}else if($aid=="" && $mid==""){
					$message = "Please Enter Search Entry";
				}
			}
		?>
		<form style="margin-left:30%" class="searchBox" action="<?php echo htmlspecialchars($_SERVER["PHP_SELF"]);?>" method="GET">

			IMDb Search: <input class="searchBox" type="text" name="search" maxlength="50" size="20">
			<input type="submit" value="Search"/>
		</form>

		<div class="margin-40"></div>

		<div class="output">
		<?php
			echo "<p>".$message."</p>";

			if($db_connection!="" && $message=="" && $aid=="" && $mid==""){

				printf("<h2 style=\"font-family: futura; margin-left: 50px\">Search in Actor Database</h2>");
				#fetch search from Actor
				while($row=mysql_fetch_row($nameResult)){
					printf("<p class=\"searchResult\">");
					printf("<a href=\"search.php?aid=".$row[0]."\">");
					for($i=1; $i<count($row); $i++){
						printf("%s ", $row[$i]);
					}
					printf("</a>");
					printf("</p>");
				}

				printf("<h2 style=\"font-family: futura; margin-left: 50px\">Search in Movie Database</h2>");
				#fetch search from Movie
				while($row=mysql_fetch_row($movieResult)){
					printf("<p class=\"searchResult\">");
					printf("<a href=\"search.php?mid=".$row[0]."\">");
					for($i=1; $i<count($row); $i++){
						printf("%s ", $row[$i]);
					}
					printf("</a>");
					printf("</p>");
				}
				
			}
			?>
			</div>

			<?php
			#dynamic link request
			if($aid!=""){
				$actorInfoQuery = "Select * From Actor Where id=".$aid.";";
				$actorInfoRow = mysql_fetch_row(mysql_query($actorInfoQuery, $db_connection));

				printf("<h2 style=\"font-family: futura; margin-left: 50px\">Actor Basic Info</h2>");
				printf("<p style=\"margin-left:50px;\">Name: %s %s <br/> Sex: %s<br/>Dob: %s <br/>Dod: %s<p>", $actorInfoRow[2], $actorInfoRow[1], $actorInfoRow[3], $actorInfoRow[4], $actorInfoRow[5]);


				printf("<h2 style=\"font-family: futura; margin-left: 50px\">Movies ".$actorInfoRow[2]." ".$actorInfoRow[1]." has acted in</h2>");
				$movieActorQuery = "Select * From MovieActor Where aid=".$aid.";";
				$movieActorResult = mysql_query($movieActorQuery, $db_connection);

				while($movieRow = mysql_fetch_row($movieActorResult)){
					$queryMovieTitle = "Select * From Movie Where id=".$movieRow[0].";";
					$movieInfo = mysql_fetch_row(mysql_query($queryMovieTitle, $db_connection));
					$movieTitle = $movieInfo[1];
					printf("<p style=\"margin-left:50px;\"><a href=\"search.php?mid=".$movieRow[0]."\">Acted in \"%s\" as \"%s\"</a><p>", $movieTitle, $movieRow[2]);
				}

			}
			if($mid!=""){
				$movieInfoQuery = "Select * From Movie Where id=".$mid.";";
				$movieInfoRow = mysql_fetch_row(mysql_query($movieInfoQuery, $db_connection));
				printf("<h2 style=\"font-family: futura; margin-left: 50px\">Movie Info</h2>");
				printf("<p style=\"margin-left:50px;\">Title: %s <br/>Year: %s<br/>Rating: %s<br/>Company: %s</p>", $movieInfoRow[1], $movieInfoRow[2], $movieInfoRow[3], $movieInfoRow[4]);
				printf("<h2 style=\"font-family: futura; margin-left: 50px\">Casts</h2>");

				$castQuery = "Select * From MovieActor Where mid=".$mid.";";
				$castQueryResult = mysql_query($castQuery, $db_connection);

				while($castRow = mysql_fetch_row($castQueryResult)){
					$queryCastname = "Select * From Actor Where id=".$castRow[1].";";
					$actorInfo = mysql_fetch_row(mysql_query($queryCastname,$db_connection));
					$actorName = $actorInfo[2]." ".$actorInfo[1];
					printf("<p style=\"margin-left:50px;\"><a href=\"search.php?aid=".$castRow[1]."\">%s acts as %s<a></p>", $actorName, $castRow[2]);
				}
			}

			if($db_connection!=""){
				mysql_close($db_connection);
			}
		?>
		</section>

</body>