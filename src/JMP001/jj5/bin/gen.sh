#!/bin/bash

main() {

  set -euo pipefail;

  cd "$(dirname "$0")/..";

  cat doc/spec.txt | tr -d '\r' | grep -v '^$' | sort > doc/spec-sorted.txt;

  bin/libexec/bmp2gen.php;

  bin/libexec/gen-template.php;

  bin/libexec/gen-header.php;

  bin/libexec/gen-declare.php;

}

main "$@";
