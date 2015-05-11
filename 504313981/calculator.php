<!DOCTYPE html>
<html>
<head><title>Calculator</title></head>
<body>

<h1>Calculator</h1>
<?php
  $message="";
  $error = False;

  if ($_SERVER["REQUEST_METHOD"] == "GET"){
    if (preg_match('/[^0-9-+*\/().]/', $_GET["expr"]) )
    {
      $error = True;
      $message = "Invalid Expression: character other than numbers and operators is found!";
    }
    if ($error == False){
      ob_start();                   //handle error
      $result = eval("return ".$_GET["expr"].";");
      if ('' !== $errorMessage = ob_get_clean()) {  //error found!
        $error = True;
        $message = "Invalid Expression: error parsing expression";
        if (strstr($errorMessage, 'Division by zero') != False){
          $message = "Division by zero";
        }
      }
      if ($error==False){
        $message = $result;
      }
    }
  }
?>
<p>
    <form method="GET">
        <input type="text" name="expr">
        <input type="submit" value="Calculate">
    </form>
</p>

<ul>
    <li>Only numbers and +,-,* and / operators are allowed in the expression.
    <li>The evaluation follows the standard operator precedence.
    <li>The calculator does not support parentheses.
    <li>The calculator handles invalid input "gracefully". It does not output PHP error messages.
</ul>

Here are some(but not limit to) reasonable test cases:
<ol>
  <li> A basic arithmetic operation:  3+4*5=23 </li>
  <li> An expression with floating point or negative sign : -3.2+2*4-1/3 = 4.46666666667, 3*-2.1*2 = -12.6 </li>
  <li> Some typos inside operation (e.g. alphabetic letter): Invalid input expression 2d4+1 </li>
</ol>

Test Cases
<ol>
  <li>-49 (Ans: -49)
  <li>2+3+4 (Ans: 9)
  <li>2*3*-4 (Ans: -24)
  <li>2*-1*-2*-3 (Ans: -12)
  <li>100-100/100 (Ans: 99)
  <li>3/2+1/3 (Ans: close or equal to 1.83333333333)
  <li>0/0 (NO excpetion shown on the page)
  <li>abcd (Invalid Expression)
  <li>one/two (Invalid Expression)
  <li><> (Invalid Expression)
  <li>1++1 (Invalid Expression)
  <li>1 + 2 (Invalide Expression)
  <li>1.1+2.5 (Ans: 3.6)
</ol>

<?php
  echo "<h1>Result</h1>";
  echo $message;

?>

