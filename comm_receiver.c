#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/un.h>
#include <string.h>
#include <sys/time.h>

#define MAX_MESSAGE_SIZE 4096 // 2^12 characters

void cleanup(int soc, const char *file) {
    close(soc);
    unlink(file);
}

int main() {
    int soc;
    char buf[MAX_MESSAGE_SIZE];
    const char *NAME = "./receiver_soc";
    struct sockaddr_un self, peer;
    socklen_t len = sizeof(peer);
    int n;
    int time_length = 5; // Seconds until timeout
    
    fd_set read_fds;
    struct timeval timeout;

    self.sun_family = AF_UNIX;
    strcpy(self.sun_path, NAME);

    soc = socket(AF_UNIX, SOCK_DGRAM, 0);

    n = bind(soc, (const struct sockaddr *)&self, sizeof(self));
    if (n < 0) {
        fprintf(stderr, "bind failed\n");
        exit(1);
    }

    // Set a timeout of 5 seconds
    timeout.tv_sec = time_length;
    timeout.tv_usec = 0;

    FD_ZERO(&read_fds);
    FD_SET(soc, &read_fds);

    // Use select to wait for data or timeout
    n = select(soc + 1, &read_fds, NULL, NULL, &timeout);
    if (n == 0) {
        // Timeout occurred
        fprintf(stderr, "Timeout: No message received within the specified time.\n");
        cleanup(soc, self.sun_path);
        exit(1);
    } else if (n < 0) {
        // Error in select
        perror("select");
        cleanup(soc, self.sun_path);
        exit(1);
    }

    // If n > 0, data is available to read
    n = recvfrom(soc, buf, sizeof(buf), 0, (struct sockaddr *)&peer, &len);
    if (n < 0) {
        fprintf(stderr, "recvfrom failed\n");
        cleanup(soc, self.sun_path);
        exit(1);
    }
    printf("Datagram received = %s\n", buf);

    cleanup(soc, self.sun_path);
    return 0;
}