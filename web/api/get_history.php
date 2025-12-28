<?php
header("Content-Type: application/json; charset=utf-8");
include("db_config.php");


$hours = isset($_GET["hours"]) ? intval($_GET["hours"]) : 24;
if ($hours <= 0)  $hours = 1;
if ($hours > 168) $hours = 168;   // tối đa 7 ngày

$sql = "SELECT `time`, ph, tds, turbidity, temperature, alert_level
        FROM sensor_data
        WHERE `time` >= NOW() - INTERVAL $hours HOUR
        ORDER BY `time` ASC";

$res = $conn->query($sql);

$labels = $phArr = $tdsArr = $ntuArr = $tempArr = $lvArr = [];
if ($res) {
    while ($r = $res->fetch_assoc()) {
        $labels[] = date("H:i", strtotime($r["time"]));
        $phArr[]  = (float)$r["ph"];
        $tdsArr[] = (float)$r["tds"];
        $ntuArr[] = (float)$r["turbidity"];
        $tempArr[]= (float)$r["temperature"];
        $lvArr[]  = (int)$r["alert_level"];
    }
}

echo json_encode([
    "status" => "ok",
    "labels" => $labels,
    "ph"     => $phArr,
    "tds"    => $tdsArr,
    "ntu"    => $ntuArr,
    "temp"   => $tempArr,
    "level"  => $lvArr     
]);

$conn->close();
?>
