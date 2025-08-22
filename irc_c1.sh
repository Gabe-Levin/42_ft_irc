#!/usr/bin/env bash
SERVER=${1:-localhost}
PORT=${2:-6668}
NICK=${NICK:-Client1}
PASS=${PASS:-test}
USERFIELD=${USERFIELD:-user1}
CHAN=${CHAN:-#channel1}

{
  printf 'PASS %s\r\n' "$PASS"
  printf 'NICK %s\r\n' "$NICK"
  printf 'USER %s\r\n' "$USERFIELD"
  printf 'JOIN %s\r\n' "$CHAN"
  cat
} | nc -C "$SERVER" "$PORT"