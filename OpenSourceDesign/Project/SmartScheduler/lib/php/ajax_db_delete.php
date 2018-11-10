<?php
$con = mysqli_connect('localhost','root','2yoreo3o','dieting');
if (!$con)  {
    die('Could not connect: ' . mysqli_error($con));
}
$in_id = $_POST['id'];
$sql = "delete from schedule where id='$in_id';";
mysqli_query($con,$sql);
mysqli_close($con);
?>