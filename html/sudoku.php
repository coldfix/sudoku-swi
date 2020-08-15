<?php
header('Content-Type: application/json');

$cols = 3;
$rows = 3;

if (array_key_exists('size', $_GET)) {
    list($c, $r) = explode('x', $_GET['size']);
    if (is_numeric($c) && is_numeric($r)){
        $cols = $c;
        $rows = $r;
    }
}

$dim = $cols.'x'.$rows;

exec("./sudoku $dim --solution", $riddle, $code);

if ($code == 0) {
    $riddle = array_values(array_filter($riddle));
    $data = array_map(function($line) {
        return preg_split("/\s+/", trim($line));
    }, $riddle);


    $response = [
        'success' => true,
        'board' => $data,
        'size' => $dim,
    ];
}
else {
    $response = [
        'success' => false,
        'board' => null,
        'size' => null,
    ];
}

echo json_encode($response);
?>
