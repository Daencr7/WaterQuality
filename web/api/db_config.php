<?php
$servername = "localhost";
$username   = "ednaiovn_iotuser";   
$password   = "Test12@123@A";   
$dbname     = "ednaiovn_iotdb";

$conn = new mysqli($servername, $username, $password, $dbname);
if ($conn->connect_error) {
    die(json_encode([
        "status"  => "error",
        "message" => "Connection failed: " . $conn->connect_error
    ]));
}

$conn->set_charset("utf8mb4");
?>
