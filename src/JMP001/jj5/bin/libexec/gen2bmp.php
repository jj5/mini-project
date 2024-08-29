#!/usr/bin/env php
<?php

// 2024-08-29 jj5 - this script was for generating bitmap files from the gen header files, but it's not used anymore.

require_once __DIR__ . '/inc/common.php';

function main( $argv ) {

  chdir( __DIR__ . '/../../' );

  foreach ( glob( 'gen/*.c' ) as $path ) {

    //echo "$path\n";

    $filename = basename( $path, '.c' );

    echo "$filename\n";

    $data = [];

    foreach ( file( $path ) as $line ) {

      if ( strpos( $line, '0x' ) === 0 ) {

        $parts = explode( ',', $line );

        foreach ( $parts as $part ) {

          $part = trim( $part );

          if ( ! $part ) { continue; }

          $pair = explode( 'x', $part );

          $byte = hexdec( $pair[ 1 ] );

          $data[] = $byte;

        }
      }
    }

    write_file( $filename, $data );

  }
}

function write_file( $filename, $data ) {

  $binary = '';

  for ( $i = 0; $i < count( BMP_HEADER ); $i++ ) {

    $binary .= chr( BMP_HEADER[ $i ] );

  }

  $chunk_list = [];

  foreach ( array_chunk( $data, 8 ) as $chunk ) {

    $chunk_list[] = $chunk;

  }

  $chunk_list = array_reverse( $chunk_list );

  foreach ( $chunk_list as $chunk ) {

    for ( $i = 0; $i < count( $chunk ); $i++ ) {

      $binary .= chr( $chunk[ $i ] );

    }
  }


  file_put_contents( "bmp/{$filename}.bmp", $binary );

}

main( $argv );
