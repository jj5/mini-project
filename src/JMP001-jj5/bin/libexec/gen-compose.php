#!/usr/bin/env php
<?php

// 2024-09-24 jj5 - this script generates a .XCompose file from the spec.txt file

require_once __DIR__ . '/inc/common.php';

function main( $argv ) {

  echo "# 2024-09-24 jj5 - copy this file to ~/.XCompose and configure your Compose Key as Right Alt\n";

  chdir( __DIR__ . '/../../' );

  $spec = file( 'doc/spec.txt' );

  foreach ( $spec as $line ) {

    $line = trim( $line );

    if ( ! $line ) { continue; }

    $parts = explode( ':', $line );

    if ( count( $parts ) !== 2 ) { continue; }

    $code = $parts[ 0 ];

    if ( ! preg_match( '/^[0-9]{4}$/', $code ) ) { continue; }

    $parts = explode( '-' , $parts[ 1 ] );

    $char = trim( $parts[ 0 ] );

    $a = $code[ 0 ];
    $b = $code[ 1 ];
    $c = $code[ 2 ];
    $d = $code[ 3 ];

    echo "<Multi_key> <$a> <$b> <$c> <$d> : \"$char\"\n";

  }
}

main( $argv );
