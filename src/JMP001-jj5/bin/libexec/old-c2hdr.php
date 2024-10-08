#!/usr/bin/env php
<?php

// 2024-08-29 jj5 - this script is no longer used, what it did was generate bitmap header files from the bitmaps.c file in
// the mag/ino/Symbol_Keyboard directory but these files are generated from the bitmaps now.

require_once __DIR__ . '/inc/common.php';

function main( $argv ) {

  chdir( __DIR__ . '/../../' );

  $lines = file( '../mag/ino/Symbol_Keyboard/bitmaps.c' );

  $name = null;
  $data = [];

  for ( $i = 0; $i < count( $lines ); $i++ ) {

    $line = trim( $lines[ $i ] );

    if ( ! $line ) { continue; }

    if ( $line === '};' ) {

      write_file( $name, $data );

    }
    elseif ( preg_match( '/const unsigned char (.*)\[\] PROGMEM=\{64,64,/', $line, $matches ) ) {

      $name = $matches[ 1 ];
      $data = [];

    }
    else {

      $parts = explode( ',', $line );

      foreach ( $parts as $part ) {

        $part = trim( $part );

        if ( $part ) { $data[] = $part; }

      }
    }
  }
}

function write_file( $name, $data ) {

  $content = "
// this bitmap header file was generated by c2gen.php

const unsigned char {$name}[] PROGMEM={64,64,
";

  $count = 0;

  while ( count( $data ) ) {

    $content .= array_shift( $data ) . ',';

    if ( ++$count === 8 ) {

      $content .= "\n";
      $count = 0;

    }
  }

  $content .= "};\n";

  file_put_contents( "ino/Symbol_Keyboard/gen/$name.c", $content );

}

main( $argv );
