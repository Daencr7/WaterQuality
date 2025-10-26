<?php
$servername = "localhost"; // luôn là localhost trên hosting
$username   = "ednaiovn_edna";
$password   = "Hudangcr@611";
$dbname     = "ednaiovn_iotdb";

$conn = new mysqli($servername, $username, $password, $dbname);
if ($conn->connect_error) {
  die("Connection failed: " . $conn->connect_error);
}
$conn->set_charset("utf8");
?>
