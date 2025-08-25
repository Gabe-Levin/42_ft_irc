#!/usr/bin/env bash
SERVER=${1:-localhost}
PORT=${2:-6668}
NICK=${NICK:-Client2}
PASS=${PASS:-test}
USERFIELD=${USERFIELD:-user2}
REALNAME=${REALNAME:-Real User}
CHAN=${CHAN:-#channel1}
TARGET=${TARGET:-Client1}
MSG=${MSG:-"Hello Client1 my man"}

{
  printf 'PASS %s\r\n' "$PASS"
  printf 'NICK %s\r\n' "$NICK"
  printf 'USER %s 0 * :%s\r\n' "$USERFIELD" "$REALNAME"
  printf 'JOIN %s\r\n' "$CHAN"
  printf 'PRIVMSG %s :%s\r\n' "$TARGET" "$MSG"
  cat
} | nc -C "$SERVER" "$PORT"
