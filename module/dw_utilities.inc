<?php

function output_csv( $fileName, $csvDataArray ){

    //$csvDataArray is expected to be an array of rows.
    //where each row is an array of string values.
    //the first row is expected to be an array of column names.
    //subsequent rows are the data values aligned to match

    $csvOutputString = '';

    //for each row, break out elements into quoted strings, separated by commas.
    foreach( $csvDataArray as $row ){
        $csvOutputString .= '"'.implode( '","', $row )."\"\r\n";
    }

    $dataByteSize = strlen( $csvOutputString );

    header("Pragma: public"); // required
    header("Expires: 0");
    header("Cache-Control: must-revalidate, post-check=0, pre-check=0");
    header("Cache-Control: private",false); // required for certain browsers
    header("Content-Type: application/force-download");
    header("Content-Disposition: attachment; filename=\"$fileName\"");
    header("Content-Transfer-Encoding: binary");
    header("Content-Length: $dataByteSize");
    header("Content-type: text/csv");

    echo $csvOutputString;

    return 'generating csv, prepare to be amazed!!';
}
