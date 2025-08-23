# ft_irc – Quick Testing Guide

This file shows example commands for running your server and testing all required IRC features and modes.

## Running the server
```bash
./ircserv 6668 test
```

## Connecting with `nc`
```bash
nc -C localhost 6668
```

## Sample registration
```
PASS test
NICK Todd
USER tpacker
```

## Basic channel usage
Join (or create) a channel:
```
JOIN #channel1
```

Send a message to channel:
```
PRIVMSG #channel1 :hello everyone!
```

Send a private message to a user:
```
PRIVMSG Client2 :Servus!
```

Kick a client out (must be operator):
```
KICK #channel1 Client2
```

## MODE commands

### Invite-only (`+i` / `-i`)
Make channel invite-only:
```
MODE #channel1 +i
```

Join attempt without invite:
```
JOIN #channel1
; expect 473 Cannot join channel (+i)
```

Invite a user, then they join:
```
INVITE Client2 #channel1
JOIN #channel1
```

Remove invite-only:
```
MODE #channel1 -i
```

### Topic restriction (`+t` / `-t`)
Only ops may change topic:
```
MODE #channel1 +t
TOPIC #channel1 :New topic (by operator)
```

Non-op attempt:
```
TOPIC #channel1 :I try to change
; expect 482 You're not channel operator
```

Allow anyone to change topic:
```
MODE #channel1 -t
```

### Channel key (`+k` / `-k`)
Set password:
```
MODE #channel1 +k secretpw
```

Join without key:
```
JOIN #channel1
; expect 475 Cannot join channel (+k)
```

Join with key:
```
JOIN #channel1 secretpw
```

Remove key:
```
MODE #channel1 -k
```

### Operator privileges (`+o` / `-o`)
Give operator to a user:
```
MODE #channel1 +o Client2
```

Remove operator from a user:
```
MODE #channel1 -o Client2
```

### User limit (`+l` / `-l`)
Set max users:
```
MODE #channel1 +l 2
```

Third user join attempt:
```
JOIN #channel1
; expect 471 Cannot join channel (+l)
```

Remove user limit:
```
MODE #channel1 -l
```

### View current modes
```
MODE #channel1
```