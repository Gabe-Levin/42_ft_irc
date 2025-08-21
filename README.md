Example commands for running and joining the server for quicker testing.

Running server:
``` 
./ircserv 6668 test 
```

Joining server:
```
nc -C localhost 6668 
```

Sample info:
```
PASS test
NICK Todd
USER tpacker
```

Joining/Creating a channel:
```
JOIN #channel1
```

Sending a message to #channel1
```
PRIVMSG #channel1 :hello everyone!
```

Sending a message to a user
```
PRIVMSG Client2 :Servus!
```