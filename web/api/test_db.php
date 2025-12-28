<?php
header("Content-Type: text/plain");


require "db_config.php";


echo "---- DB CONFIG LOADED ----\n";
echo "SERVER    = $servername\n";
echo "USERNAME  = $username\n";
echo "DB NAME   = $dbname\n";
echo "PASSWORD  = $password\n\n";


echo "---- TESTING CONNECTION ----\n";

$conn = new mysqli($servername, $username, $password, $dbname);


if ($conn->connect_error) {
    echo "CONNECT_ERROR: " . $conn->connect_error . "\n";
    exit;
}

echo "CONNECT_OK\n\n";


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
