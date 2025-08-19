#include <vector>
#include <poll.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

int main() {
    std::vector<struct pollfd> pfds(1);
    pfds[0].fd = 0;            // stdin
    pfds[0].events = POLLIN;

    printf("Waiting for input...\n");
    int ret = poll(&pfds[0], pfds.size(), 5000);
    if (ret > 0 && (pfds[0].revents & POLLIN)) {
        char buf[1024];
        ssize_t n = read(0, buf, sizeof(buf)-1); // read what's available
        if (n > 0) {
            buf[n] = '\0';
            printf("Got line: %s", buf);        // will include your newline
        }
    } else if (ret == 0) {
        printf("Timeout, no input\n");
    } else {
        perror("poll");
    }
}
