#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>

#define MAX_MESSAGE_SIZE 4096 // 2^12 characters

void cleanup(int soc, const char *file)
{
    close(soc);
    unlink(file);
}

void receiveMessage()
{
    int soc;
    char buf[MAX_MESSAGE_SIZE];
    const char *NAME = "./receiver_soc";
    struct sockaddr_un self;
    struct sockaddr_un peer;
    struct timeval timeout;
    socklen_t len = sizeof(timeout);
    int n, time_length = 5; // Seconds until timeout

    fd_set read_fds;

    self.sun_family = AF_UNIX;
    strcpy(self.sun_path, NAME);

    soc = socket(AF_UNIX, SOCK_DGRAM, 0);

    n = bind(soc, (const struct sockaddr *)&self, sizeof(self));
    if (n < 0)
    {
        fprintf(stderr, "bind failed\n");
        exit(1);
    }

    // Set a timeout of 5 seconds
    timeout.tv_sec = time_length;
    timeout.tv_usec = 0;

    FD_ZERO(&read_fds);
    FD_SET(soc, &read_fds);

    int i = 0;
    while (i < 3)
    {

        // Use select to wait for data or timeout
        n = select(soc + 1, &read_fds, NULL, NULL, &timeout);
        if (n == 0)
        {
            // Timeout occurred
            fprintf(stderr, "Timeout: No message received within the specified time.\n");
            cleanup(soc, self.sun_path);
            exit(1);
        }
        else if (n < 0)
        {
            // Error in select
            perror("select");
            cleanup(soc, self.sun_path);
            exit(1);
        }
        else
        {
            // If n > 0, data is available to read
            n = recvfrom(soc, buf, sizeof(buf), 0, (struct sockaddr *)&peer, &len);
            if (n < 0)
            {
                fprintf(stderr, "recvfrom failed\n");
            }
            else
            {
                printf("Datagram received = %s\n", buf);
            }
        }
        i++;
    }
    cleanup(soc, self.sun_path);
}

void sendMessage()
{
    int soc; // socket number
    char *buf[] = {
        "General Kenobi",
        "You are a bold one",
        NULL};
    const char *NAME = "./sendback_soc"; // name of the socket path
    struct sockaddr_un peer;             // socket address variable
    int n;

    peer.sun_family = AF_UNIX;
    strcpy(peer.sun_path, NAME);

    soc = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (access(peer.sun_path, F_OK) > -1)
    {
        int i = 0;

        while (buf[i] != NULL)
        {
            n = sendto(soc, buf[i], strlen(buf[i]), 0, (struct sockaddr *)&peer, sizeof(peer));

            // check if the transmission succeeded
            if (n < 0)
            {
                fprintf(stderr, "sendto failed\n");
                exit(1);
            }
            printf("Sender: %d characters sent!\n", n);
            i++;
        }
        close(soc);
    }
}

int main()
{
    // They must send, before they recieve
    sendMessage();
    receiveMessage();
    return (0);
}