<?php
header("Content-Type: text/plain");

// Tải file db_config.php
require "db_config.php";

// In ra thông tin cấu hình đang sử dụng
echo "---- DB CONFIG LOADED ----\n";
echo "SERVER    = $servername\n";
echo "USERNAME  = $username\n";
echo "DB NAME   = $dbname\n";
echo "PASSWORD  = $password\n\n";

// Thử kết nối MySQL
echo "---- TESTING CONNECTION ----\n";

$conn = new mysqli($servername, $username, $password, $dbname);

// Nếu lỗi, in lỗi và dừng
if ($conn->connect_error) {
    echo "CONNECT_ERROR: " . $conn->connect_error . "\n";
    exit;
}

echo "CONNECT_OK\n\n";

// Thử truy vấn xem bảng sensor_data có tồn tại không
echo "---- CHECKING TABLE sensor_data ----\n";
$result = $conn->query("SHOW TABLES LIKE 'sensor_data'");

if ($result && $result->num_rows > 0) {
    echo "TABLE sensor_data EXISTS\n";
} else {
    echo "TABLE sensor_data NOT FOUND\n";
}

$conn->close();
echo "\n---- DONE ----\n";
?>
