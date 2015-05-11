<!DOCTYPE html>
<html>
<head><title>Body Mass Index (BMI) - Calculator</title><meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1" /></head>
<body>
<?php
$height=$weight="";
$BMIvalue="0";
$message="";
$error = False;

if ($_SERVER["REQUEST_METHOD"] == "POST") {
   $height = test($_POST["height"]);
   $weight = test($_POST["weight"]);
   if ($error == False && floatval($height) == 0){
     $error = true;
     $height = "";
     $message = "Division by zero";
   }
   if ($error == FALSE){
     $bmi = floatval($weight)/(floatval($height)*floatval($height));
     $BMIvalue = sprintf("%f", (floatval($weight)/(floatval($height)*floatval($height))));
     if ($bmi <= 18.5){
          $message = "Underweight";
     }else if($bmi <= 24.9){
          $message = "Normal weight";
     }else if($bmi <= 29.9){
          $message = "Overweight";
     }else if($bmi <= 39.9){
          $message = "Obesity";
     }else{
          $message = "Morbidly Obesity";
     }
   }
}

function test($data){
     if(is_numeric($data) == FALSE){
          global $error;
          global $message;
          global $height;
          global $weight;
          $height = $weight = "";
          $error = True;
          $message = "input not a number";
          return "";
     }
     return $data;
}
?>

<form method="post" action="<?php echo htmlspecialchars($_SERVER["PHP_SELF"]);?>"> 
     <p><h3>Body Mass Index Calculator</h3></p>
     <p>
          <label></label><label>Height (m):
           <input type="text" name="height" value="<?php echo $height;?>" size="5" maxlength="4" />
          </label>
          
          <label>
          Weight (Kg):
          <input name="weight" type="text" id="weight" value="<?php echo $weight;?>" size="5" maxlength="3" />
          </label>
     </p>
     <p>
          <label>BMI:
          <input  readonly="true" name="BMI" type="text" id="BMI" value="<?php echo $BMIvalue; ?>" size="6" maxlength="4" />
          </label>
     </p>
     <p>
          <input type="submit" name="Submit" value="Calculate BMI" />
     </p>
</form>

<?php
echo $message;
?>

</body>
</body>
</html>

