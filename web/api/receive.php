<?php
header("Content-Type: text/plain");
include("db_config.php");

// Đọc dữ liệu JSON gửi từ ESP32
$json = file_get_contents('php://input');
$data = json_decode($json, true);

if (!$data) {
    echo "Invalid JSON";
    exit;
}

// Lấy các giá trị từ JSON
$temp = $data['temp'] ?? null;
$ph   = $data['ph'] ?? null;
$tds  = $data['tds'] ?? null;
$ntu  = $data['ntu'] ?? null;

if ($temp === null || $ph === null || $tds === null || $ntu === null) {
    echo "Missing fields";
    exit;
}

$time = date("Y-m-d H:i:s");

// Ghi vào MySQL
$sql = "INSERT INTO sensor_data (temperature, ph, tds, turbidity, time)
        VALUES ('$temp', '$ph', '$tds', '$ntu', '$time')";

if ($conn->query($sql) === TRUE) {
    echo "Data OK";
} else {
    echo "Error: " . $conn->error;
}

$conn->close();
?>
