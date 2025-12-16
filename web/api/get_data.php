<?php
header("Content-Type: application/json; charset=utf-8");
include("db_config.php");

// Lấy 20 mẫu mới nhất
$sql = "SELECT id, temperature, ph, tds, turbidity, `time`, alert_level, alert_source
        FROM sensor_data
        ORDER BY `time` DESC
        LIMIT 20";

$result = $conn->query($sql);

$data = [];
if ($result) {
    while ($row = $result->fetch_assoc()) {
        $data[] = [
            "id"           => (int)$row["id"],
            "temp"         => (float)$row["temperature"],
            "ph"           => (float)$row["ph"],
            "tds"          => (float)$row["tds"],
            "ntu"          => (float)$row["turbidity"],
            "time"         => $row["time"],
            "alert_level"  => (int)$row["alert_level"],
            "alert_source" => $row["alert_source"],
        ];
    }
}

// Đảo lại cho time tăng dần
$data = array_reverse($data);

echo json_encode([
    "status" => "ok",
    "data"   => $data
]);

$conn->close();
?>
