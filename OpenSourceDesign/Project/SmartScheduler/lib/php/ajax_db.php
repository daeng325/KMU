<?php
$con = mysqli_connect('localhost','root','2yoreo3o','dieting');
if (!$con)  {
    die('Could not connect: ' . mysqli_error($con));
}
$sql = "SELECT * FROM schedule order by date";
$result = mysqli_query($con,$sql);
while($row = mysqli_fetch_array($result))
{
   echo $row['state'];
   echo "\test";
   echo $row['upper'];
   echo "\test";
   echo $row['lower'];
   echo "\test";
   echo $row['schedule'];
   echo "\test";
   echo $row['date'];
   echo "\test";
   echo $row['id'];
   echo "\test";
}
mysqli_close($con);
?>