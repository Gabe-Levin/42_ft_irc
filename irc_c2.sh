#!/usr/bin/env bash
SERVER=${1:-localhost}
PORT=${2:-6667}
NICK=${NICK:-Client2}
PASS=${PASS:-test}
USERFIELD=${USERFIELD:-user2}
CHAN=${CHAN:-#channel1}

{
  printf 'PASS %s\r\n' "$PASS"
  printf 'NICK %s\r\n' "$NICK"
  printf 'USER %s\r\n' "$USERFIELD"
  printf 'JOIN %s\r\n' "$CHAN"
  printf 'PRIVMSG %s :hello everyone! \r\n' "$CHAN"
  cat
} | nc -C "$SERVER" "$PORT"