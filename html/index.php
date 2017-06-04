<!DOCTYPE html
PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
  <title>Sudoku</title>
  <meta http-equiv="Content-Type" content="text/html;charset=utf-8"/>
  <link rel="stylesheet" type="text/css" href="./sudoku.css"/>
  <script type="text/javascript" src="jquery.js"></script>
</head>
<body>
<div id="content">

 <h3>Sudoku</h3>
<?php
    $cols = 3;
    $rows = 3;

    if (array_key_exists('size', $_GET)) {
        list($c, $r) = explode('x', $_GET['size']);
        if (is_numeric($c) && is_numeric($r)){
            $cols = $c;
            $rows = $r;
        }
    }

    // $size = $length >= 10 ? 2 : 1;
    $length = $cols * $rows;
    $dim = $cols.'x'.$rows;

    $options = array("2x2", "3x3", "4x4");
    if (!in_array($dim, $options))
        $options[] = $dim;
    sort($options);
?>

<form action="" method="get">
 <div>
  Size:
  <select name="size">
<?php
    foreach ($options as $option)
        echo "    <option" . ($option==$dim?" selected=\"selected\"":"") .">$option</option>\n";
?>
  </select>
  <input type="submit" value="Go!"/>
 </div>
</form>

<table class="sudoku">
<?php

    exec("./sudoku $dim --solution", $riddle, $code);

    if ($code != 0) {
        die ("Fatal error: $code! Please contact t_glaessle@gmx.de");
    }

    $row = 0;
    foreach ($riddle as $line) {
        if ($line == "")
            continue;
        echo " <tr>\n";

        $row_attr = "";
        if ($row%$rows == 0 && $row != 0) $row_attr .= " top";
        if (($row+1)%$rows == 0 && $row+1 != $length) $row_attr .= " bottom";

        $cells = explode(" ", $line);
        $col = 0;
        foreach ($cells as $cell) {
            if ($cell == "")
                continue;
            $is_hidden = false;

            $col_attr = "";
            if ($cell != 0) $col_attr .= "r";
            if ($col%$cols == 0 && $col != 0) $col_attr .= " left";
            if (($col+1)%$cols == 0 && $col+1 != $length) $col_attr .= " right";

            $hidden_attr = "";
            if (substr($cell, 0, 1) == "~") {
                $cell = substr($cell, 1);
                $is_hidden = true;
                // $hidden_attr = " hidden";
            }

            $attr = trim ($col_attr . $row_attr . $hidden_attr);
            if ($attr)
                $attr = " class=\"$attr\"";

            echo "  <td$attr>";
            if ($is_hidden) {
                echo "<span class=\"hidden\">$cell</span>";
                // echo "<input type=\"text\" class=\"edit\" size=\"2\"/>";
            }
            else
                echo $cell;
            echo "</td>\n";
            ++$col;
        }
        echo " </tr>\n";
        ++$row;
    }


?>
</table>


<script type="text/javascript">
    var visible = false;
    function ShowSolution()
    {
        if (visible) {
            $(".hidden").hide();
            // $(".edit").show();
            $("#showhidebutton").text("Show solution");
        } else {
            // $(".edit").hide();
            $(".hidden").show();
            $("#showhidebutton").text("Hide solution");
        }
        visible = !visible;
    }
</script>

<div>
 <button id="showhidebutton" onclick="javascript:ShowSolution()">Show solution</button>
</div>

</div>
</body>
</html>

