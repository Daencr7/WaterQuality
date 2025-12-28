<?php
// send_mail.php
// Chạy được cả với POST (ESP32) và GET (debug trên trình duyệt)

$debug = isset($_GET['debug']);

if ($debug) {
    header('Content-Type: text/plain; charset=utf-8');
} else {
    header('Content-Type: application/json; charset=utf-8');
}

// Cho phép lấy dữ liệu từ POST (ESP32) hoặc GET (debug)
$subject = '';
$message = '';

if (isset($_POST['subject']) || isset($_POST['message'])) {
    $subject = isset($_POST['subject']) ? trim($_POST['subject']) : '';
    $message = isset($_POST['message']) ? trim($_POST['message']) : '';
} else if (isset($_GET['subject']) || isset($_GET['message'])) {
    $subject = isset($_GET['subject']) ? trim($_GET['subject']) : '';
    $message = isset($_GET['message']) ? trim($_GET['message']) : '';
}

if ($subject === '' || $message === '') {
    if ($debug) {
        echo "ERROR: Missing subject or message\n";
        echo "subject='$subject'\nmessage='$message'\n";
        exit;
    }
    echo json_encode([
        "status"  => "error",
        "message" => "Missing subject or message"
    ]);
    exit;
}

/* =============== CẤU HÌNH MAIL =============== */

$to = "teednaa6@gmail.com"; 

$from_email = "alert@edna.io.vn";
$from_name  = "He thong giam sat nuoc thai";

$mail_subject = "[CANH BAO NUOC THAI] " . $subject;

$body  = "Thoi gian server: " . date('Y-m-d H:i:s') . "\n\n";
$body .= $message . "\n";

$headers  = "From: $from_name <$from_email>\r\n";
$headers .= "Reply-To: $from_email\r\n";
$headers .= "X-Mailer: PHP/" . phpversion();

$log_line  = date('Y-m-d H:i:s') . " | TO=$to | SUBJ=$mail_subject\n";
$log_line .= "BODY: " . str_replace("\n", "\\n", $body) . "\n";

$log_file = __DIR__ . "/mail_debug_log.txt";
file_put_contents($log_file, $log_line, FILE_APPEND);

// Gửi mail
$result = @mail($to, $mail_subject, $body, $headers);

if ($debug) {
    echo "mail() returned: " . ($result ? "TRUE" : "FALSE") . "\n";
    echo "Log file: $log_file\n";
    $last_error = error_get_last();
    if ($last_error) {
        echo "PHP last error: " . print_r($last_error, true) . "\n";
    }
    exit;
}

if ($result) {
    echo json_encode([
        "status"  => "success",
        "message" => "Mail sent"
    ]);
} else {
    echo json_encode([
        "status"  => "error",
        "message" => "Failed to send mail"
    ]);
}
