<?php
header('Content-Type: application/json');
header('X-Content-Type-Options: nosniff');

function fail_response($status)
{
    http_response_code($status);
    echo json_encode(['success' => false, 'data' => null]);
    exit;
}

$cols = 3;
$rows = 3;

if (array_key_exists('size', $_GET)) {
    // Bound the input before converting to integers or multiplying it.
    if (!is_string($_GET['size']) ||
        !preg_match('/\A([1-9][0-9]?)x([1-9][0-9]?)\z/', $_GET['size'], $matches)) {
        fail_response(400);
    }
    $cols = (int) $matches[1];
    $rows = (int) $matches[2];
    if ($cols * $rows > 25) {
        fail_response(400);
    }
}

$dim = $cols.'x'.$rows;

// An argument array bypasses the shell; resolve the executable independently
// of the web server's working directory.
$process = proc_open([__DIR__.'/sudoku', $dim, '--solution'], [
    0 => ['file', '/dev/null', 'r'],
    1 => ['pipe', 'w'],
    2 => ['file', '/dev/null', 'w'],
], $pipes);
if ($process === false) {
    fail_response(500);
}

// PHP's execution time limit does not reliably limit child processes.
stream_set_blocking($pipes[1], false);
$deadline = hrtime(true) + 10 * 1000000000;
$output = '';
$failure = null;
do {
    $output .= stream_get_contents($pipes[1], 16385 - strlen($output));
    $status = proc_get_status($process);
    if (!$status['running']) {
        $output .= stream_get_contents($pipes[1], 16385 - strlen($output));
    }
    if (strlen($output) > 16384) {
        $failure = 502;
    } elseif ($status['running'] && hrtime(true) >= $deadline) {
        $failure = 504;
    }
    if ($failure !== null) {
        proc_terminate($process, 9);
        break;
    }
    if ($status['running']) {
        usleep(10000);
    }
} while ($status['running']);
fclose($pipes[1]);
proc_close($process);
if ($failure !== null) {
    fail_response($failure);
}
$code = $status['exitcode'];
$riddle = explode("\n", $output);

if ($code == 0) {
    $riddle = array_values(array_filter($riddle));
    $data = array_map(function($line) {
        return preg_split("/\s+/", trim($line));
    }, $riddle);


    $response = [
        'success' => true,
        'data' => $data,
        'cols' => $cols,
        'rows' => $rows,
    ];
}
else {
    fail_response(502);
}

echo json_encode($response);
