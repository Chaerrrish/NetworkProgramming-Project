#include "hangman.h"

int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "usage: tcp_client hostname port\n");
        return 1;
    }

    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;

    struct addrinfo *client_address;
    if (getaddrinfo(argv[1], argv[2], &hints, &client_address)) {
        perror("getaddrinfo() failed");
        return 1;
    }

    int sock_client;
    sock_client = socket(client_address->ai_family, client_address->ai_socktype, client_address->ai_protocol);
    if (sock_client < 0) {
        perror("socket() failed");
        return 1;
    }

    if (connect(sock_client, client_address->ai_addr, client_address->ai_addrlen) < 0) {
        perror("connect() failed");
        return 1;
    }
    freeaddrinfo(client_address);

    printf("Connecting\n");

    fd_set master, reads;
    FD_ZERO(&master);
    FD_SET(sock_client, &master);
    FD_SET(STDIN_FILENO, &master);

    while (1) {
        reads = master;
        if (select(sock_client + 1, &reads, NULL, NULL, NULL) < 0) {
            perror("select() failed");
            return 1;
        }

         if (FD_ISSET(sock_client, &reads))
        {
            char read[BUFFER_SIZE];
            memset(read, 0, BUFFER_SIZE);
            int recv_bytes = recv(sock_client, read, sizeof(read), 0);
            read[recv_bytes +1] = '\0';
            if (recv_bytes < 1) {
                if (recv_bytes == 0) {  // 수신한 바이트가 0일 때 처리 추가
                    printf("Connection closed by server\n");
                } else {
                    perror("recv() failed");
                }
                break;
            }
            // read[recv_bytes + 1] = '\0';
            printf("%s\n", read);   // 수신한 문자열 출력
        }


        if (FD_ISSET(STDIN_FILENO, &reads)) {
            char input_buff[BUFFER_SIZE];
            if (fgets(input_buff, sizeof(input_buff), stdin) != NULL) {
                send(sock_client, input_buff, strlen(input_buff), 0);
            }
        }
    }

    printf("Closing socket...\n");
    close(sock_client);
    return 0;
}