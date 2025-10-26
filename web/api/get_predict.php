<?php
header("Content-Type: application/json");
include("db_config.php");

$sql = "SELECT * FROM sensor_data ORDER BY time DESC LIMIT 1";
$result = $conn->query($sql);
$latest = $result->fetch_assoc();

$predict = [
  "tds" => $latest["tds"] + 30,
  "turbidity" => $latest["turbidity"] + 100,
  "temperature" => $latest["temperature"] + 0.5,
  "ph" => $latest["ph"] - 0.1
];

echo json_encode([
  "now" => $latest,
  "predict" => $predict
]);
$conn->close();
?>
