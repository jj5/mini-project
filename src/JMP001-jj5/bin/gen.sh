#!/bin/bash

# 2024-08-30 jj5 - this script does all the code generation for the project.

main() {

  set -euo pipefail;

  cd "$(dirname "$0")/..";

  # 2024-08-30 jj5 - this generates the spec-sorted.txt file, a sorted version of the spec.txt file.
  #
  cat doc/spec.txt | tr -d '\r' | grep -v '^$' | sort > doc/spec-sorted.txt;

  # 2024-08-30 jj5 - this converts the BMP files in the 'bmp' directory into C header files in the
  # 'ino/Symbol_Keyboard/gen/' directory.
  #
  bin/libexec/bmp2hdr.php;

  # 2024-08-30 jj5 - this copies the 'etc/template.bmp' file to the 'bmp' directory for any symbols which don't exist yet.
  # You can then edit the new bitmap file to create a new symbol.
  #
  bin/libexec/gen-template.php;

  # 2024-08-30 jj5 - this generates the 'ino/Symbol_Keyboard/gen/header.h' file which contains includes for all of the
  # generated symbol header files.
  #
  bin/libexec/gen-header.php;

  # 2024-08-30 jj5 - this generates the 'ino/Symbol_Keyboard/gen/declare.c' file which contains example declarations for
  # all of the generated symbols. You can copy and paste these declarations into your own code for the symbols you want to
  # use.
  #
  bin/libexec/gen-declare.php;

  # 2024-09-24 jj5 - this generates the '.XCompose' file which contains the compose key mappings for the symbols.
  #
  php bin/libexec/gen-compose.php > etc/XCompose;

}

main "$@";
