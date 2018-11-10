<?php
$con = mysqli_connect('localhost','root','2yoreo3o','dieting');
if (!$con)  {
    die('Could not connect: ' . mysqli_error($con));
}
$in_state = $_POST['state'];
$in_upper = $_POST['upper'];
$in_lower = $_POST['lower'];
$in_schedule = $_POST['schedule'];
$in_date = $_POST['date'];
$sql = "insert into schedule(state, upper, lower, schedule, date) values('$in_state','$in_upper','$in_lower','$in_schedule','$in_date');";
mysqli_query($con,$sql);
$sql = "select id from schedule order by id desc limit 1;";
$result = mysqli_query($con,$sql);
$row = mysqli_fetch_array($result);
echo $row['id'];
mysqli_close($con);

?>