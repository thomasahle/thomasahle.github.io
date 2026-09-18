#!/usr/bin/env bash
set -eu
mkdir -p ../evidence
for config in vp pclmul schoolbook block256 portable; do
  flags='-march=native'
  case "$config" in
    pclmul) flags='-mpclmul -mno-avx' ;;
    schoolbook) flags='-march=native -DCHAINHASH128_SCHOOLBOOK' ;;
    block256) flags='-march=native -DCHAINHASH128_BLOCK_BYTES=256' ;;
    portable) flags='-DCHAINHASH128_FORCE_PORTABLE' ;;
  esac
  gcc -std=c99 -D_DEFAULT_SOURCE -Wall -Wextra -O3 $flags tests/test_chainhash128.c -o "test-$config"
  "./test-$config" > "../evidence/test-$config.txt"
  "./test-$config" vectors > "../evidence/vectors-$config.txt"
  bytes=512
  if [ "$config" = block256 ]; then bytes=256; fi
  diff "tests/vectors-$bytes.txt" "../evidence/vectors-$config.txt"
done
clang -std=c99 -D_DEFAULT_SOURCE -O1 -g -mpclmul -fsanitize=address,undefined -fno-omit-frame-pointer tests/test_chainhash128.c -o test-sanitize
./test-sanitize > ../evidence/test-sanitize.txt
