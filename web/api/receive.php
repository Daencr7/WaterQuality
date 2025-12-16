<?php
header("Content-Type: application/json; charset=utf-8");
include("db_config.php");

// Chỉ cho phép POST
if ($_SERVER["REQUEST_METHOD"] !== "POST") {
    echo json_encode(["status" => "error", "message" => "Use POST method"]);
    exit;
}

// Đọc JSON từ ESP32
$raw  = file_get_contents("php://input");
$data = json_decode($raw, true);

if (!$data) {
    echo json_encode(["status" => "error", "message" => "Invalid JSON"]);
    exit;
}

// Lấy các trường
$ph   = isset($data["ph"])   ? floatval($data["ph"])   : null;
$tds  = isset($data["tds"])  ? floatval($data["tds"])  : null;
$ntu  = isset($data["ntu"])  ? floatval($data["ntu"])  : null;  // -> turbidity
$temp = isset($data["temp"]) ? floatval($data["temp"]) : null;  // -> temperature

if ($ph === null || $tds === null || $ntu === null || $temp === null) {
    echo json_encode(["status" => "error", "message" => "Missing fields"]);
    exit;
}

/* ================== HÀM TÍNH MỨC CẢNH BÁO ================== */

/*
   Ngưỡng theo đề bài:
   Level 1:
       pH > 9.35 hoặc pH < 5.4
       NTU > 110
       Nhiệt độ > 44
       TDS > 1650
   Level 2:
       pH > 10.2 hoặc pH < 4.8
       NTU > 120
       Nhiệt độ > 48
       TDS > 1800
   Level 3:
       pH > 11.1 hoặc pH < 4.2
       NTU > 130
       Nhiệt độ > 52
       TDS > 1950
*/

function level_ph($pH) {
    if ($pH > 11.1 || $pH < 4.2) return 3;
    if ($pH > 10.2 || $pH < 4.8) return 2;
    if ($pH > 9.35 || $pH < 5.4) return 1;
    return 0;
}

function level_turbidity($ntu) {
    if ($ntu > 130) return 3;
    if ($ntu > 120) return 2;
    if ($ntu > 110) return 1;
    return 0;
}

function level_temp($t) {
    if ($t > 52) return 3;
    if ($t > 48) return 2;
    if ($t > 44) return 1;
    return 0;
}

function level_tds($tds) {
    if ($tds > 1950) return 3;
    if ($tds > 1800) return 2;
    if ($tds > 1650) return 1;
    return 0;
}

// Tính level từng tham số
$lv_ph   = level_ph($ph);
$lv_ntu  = level_turbidity($ntu);
$lv_temp = level_temp($temp);
$lv_tds  = level_tds($tds);

// Lấy mức cao nhất để gán cho bản ghi
$alert_level  = 0;
$alert_source = null;

// Gán ưu tiên theo level lớn hơn
$alert_level = $lv_ph;
$alert_source = "PH";

if ($lv_ntu > $alert_level) {
    $alert_level  = $lv_ntu;
    $alert_source = "NTU";
}
if ($lv_temp > $alert_level) {
    $alert_level  = $lv_temp;
    $alert_source = "TEMP";
}
if ($lv_tds > $alert_level) {
    $alert_level  = $lv_tds;
    $alert_source = "TDS";
}

// Nếu tất cả đều =0 thì không có nguồn
if ($alert_level == 0) {
    $alert_source = null;
}

/* ================== GHI VÀO DATABASE ================== */

$sql = "INSERT INTO sensor_data (temperature, ph, tds, turbidity, alert_level, alert_source)
        VALUES (?, ?, ?, ?, ?, ?)";

$stmt = $conn->prepare($sql);
$stmt->bind_param("ddddis", $temp, $ph, $tds, $ntu, $alert_level, $alert_source);

if ($stmt->execute()) {
    echo json_encode([
        "status"       => "ok",
        "message"      => "Data saved",
        "id"           => $stmt->insert_id,
        "alert_level"  => $alert_level,
        "alert_source" => $alert_source
    ]);
} else {
    echo json_encode(["status" => "error", "message" => $stmt->error]);
}

$stmt->close();
$conn->close();
?>
