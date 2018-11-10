<?php
$con = mysqli_connect('localhost','root','2yoreo3o','dieting');
if (!$con)  {
    die('Could not connect: ' . mysqli_error($con));
}
$in_id = $_POST['id'];
$in_state = $_POST['state'];
$in_upper = $_POST['upper'];
$in_lower = $_POST['lower'];
$in_schedule = $_POST['schedule'];
$in_date = $_POST['date'];
$sql = "update schedule set state='$in_state', upper='$in_upper', lower='$in_lower', schedule='$in_schedule', date='$in_date' where id='$in_id';";
mysqli_query($con,$sql);
mysqli_close($con);
?>