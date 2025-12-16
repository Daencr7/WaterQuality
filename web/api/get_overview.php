<?php
header("Content-Type: application/json; charset=utf-8");
include("db_config.php");

$response = [
    "status"    => "ok",
    "current"   => null,
    "last4h"    => null,
    "alerts30d" => null,
];

/* ========== 1. BẢN GHI HIỆN TẠI ========== */

$sqlCurrent = "SELECT id, temperature, ph, tds, turbidity, `time`, alert_level, alert_source
               FROM sensor_data
               ORDER BY `time` DESC
               LIMIT 1";
$resCur = $conn->query($sqlCurrent);
if ($resCur && $row = $resCur->fetch_assoc()) {
    $response["current"] = [
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

/* ========== 2. DỮ LIỆU 4 GIỜ GẦN NHẤT ========== */

$sql4h = "SELECT `time`, ph, tds, turbidity, temperature
          FROM sensor_data
          WHERE `time` >= NOW() - INTERVAL 4 HOUR
          ORDER BY `time` ASC";
$res4h = $conn->query($sql4h);

$labels = $phArr = $tdsArr = $ntuArr = $tempArr = [];
if ($res4h) {
    while ($r = $res4h->fetch_assoc()) {
        $labels[] = date("H:i", strtotime($r["time"]));
        $phArr[]  = (float)$r["ph"];
        $tdsArr[] = (float)$r["tds"];
        $ntuArr[] = (float)$r["turbidity"];
        $tempArr[]= (float)$r["temperature"];
    }
}
$response["last4h"] = [
    "labels" => $labels,
    "ph"     => $phArr,
    "tds"    => $tdsArr,
    "ntu"    => $ntuArr,
    "temp"   => $tempArr,
];

/* ========== 3. THỐNG KÊ 30 NGÀY THEO LEVEL (TUẦN 4,3,2,1) ========== */

/*
   Chia 30 ngày thành 4 đoạn ~7 ngày:
   - Tuần 4: xa nhất
   - Tuần 3
   - Tuần 2
   - Tuần này (gần nhất)
*/

$weekLabels = ["Tuần 4", "Tuần 3", "Tuần 2", "Tuần này"];

$lv1Arr = array_fill(0, 4, 0);
$lv2Arr = array_fill(0, 4, 0);
$lv3Arr = array_fill(0, 4, 0);

$now = time();
$stmtAlert = $conn->prepare("
    SELECT alert_level, COUNT(*) AS c
    FROM sensor_data
    WHERE `time` BETWEEN ? AND ?
      AND alert_level > 0
    GROUP BY alert_level
");

for ($i = 0; $i < 4; $i++) {
    // mỗi đoạn 7 ngày
    $startTs = $now - (7 * (4 - $i)) * 86400;
    $endTs   = $now - (7 * (3 - $i)) * 86400;

    $start = date("Y-m-d H:i:s", $startTs);
    $end   = date("Y-m-d H:i:s", $endTs);

    $stmtAlert->bind_param("ss", $start, $end);
    $stmtAlert->execute();
    $res = $stmtAlert->get_result();

    while ($row = $res->fetch_assoc()) {
        $lv = (int)$row["alert_level"];
        $c  = (int)$row["c"];

        if ($lv === 1) $lv1Arr[$i] += $c;
        if ($lv === 2) $lv2Arr[$i] += $c;
        if ($lv === 3) $lv3Arr[$i] += $c;
    }
}

$stmtAlert->close();

$response["alerts30d"] = [
    "labels" => $weekLabels,
    "level1" => $lv1Arr,
    "level2" => $lv2Arr,
    "level3" => $lv3Arr,
];

echo json_encode($response);
$conn->close();
?>
