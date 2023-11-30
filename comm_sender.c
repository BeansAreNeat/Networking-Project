#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <string.h>

int main() {
    int soc;                                // socket number
    char buf[] = "Hello there";             // message content to be sent
    const char *NAME = "./receiver_soc";    //name of the socket path
    struct sockaddr_un peer;                //socket address variable
    int n;

    peer.sun_family = AF_UNIX;
    strcpy(peer.sun_path, NAME); 

    soc = socket(AF_UNIX, SOCK_DGRAM, 0);
    if ( access(peer.sun_path, F_OK) > -1 ) {
        n = sendto(soc, buf, strlen(buf), 0, (struct sockaddr *)&peer,sizeof(peer)); 

        // check if the transmission succeeded
        if ( n < 0 ) { 
            fprintf(stderr, "sendto failed\n");
            exit(1);
        }
        printf("Sender: %d characters sent!\n", n);
        close(soc);
    }
    return(0);
}