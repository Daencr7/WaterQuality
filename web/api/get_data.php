<?php
header("Content-Type: application/json");
include("db_config.php");

$sql = "SELECT * FROM sensor_data ORDER BY time DESC LIMIT 20";
$result = $conn->query($sql);

$data = [];
while($row = $result->fetch_assoc()) {
    $data[] = $row;
}
echo json_encode(array_reverse($data)); // đảo lại cho đúng thứ tự thời gian
$conn->close();
?>
