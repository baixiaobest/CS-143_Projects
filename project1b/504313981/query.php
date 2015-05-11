<!DOCTYPE html>
<html>
<head>
	<title>mysql</title>
	<style>
		h1{
			font-family: Verdana;
			color: red;
			text-align: center;
		}
		body{
			height: 1000px;
		}
		.contents{
			width: 85%;
			//text-align: center;
			margin-left: auto;
			margin-right: auto;
			background-color: #EEE;
			height: 100%;
		}
		tr:nth-child(odd){
			background-color: #FFF;
		}
	</style>

</head>



<body>

	<section class="contents">
	<h1>Welcome to IMDB search</h1>

	<?php
		$text="";
		$result;
		$db_connection = "";
		if ($_SERVER["REQUEST_METHOD"] == "POST"){
			if($_POST["area"]!=""){
				$text = $_POST["area"];
				$db_connection = mysql_connect("localhost", "cs143", "");
				mysql_select_db("CS143", $db_connection);
				$result = mysql_query($text, $db_connection);
			}
		}
	?>
	<form method="post" action="<?php echo htmlspecialchars($_SERVER["PHP_SELF"]);?>"><textarea  name="area" rows=15 cols=70><?php echo $text?></textarea>
		<input type="submit" value="Submit"/>
	</form>
	<?php
		if($db_connection != ""){
			echo "<div style=\"height:500px; overflow:auto;\"><table width=\"100%\">";

			echo "<tr>";
			for($i=0; $i<mysql_num_fields($result); $i++){
				$fieldName = mysql_field_name($result, $i);
				echo "<td>".$fieldName."</td>";
			}
			echo "</tr>";
			while($row = mysql_fetch_row($result)) {
				echo "<tr>";
    			for($i=0; $i<count($row); $i++){
    				echo "<td>".$row[$i]."</td>";
    			}
    			echo "</tr>";
			}
			echo "</table></div>";
			mysql_close($db_connection);
		}
	?>
	</section>
</body>
</html>