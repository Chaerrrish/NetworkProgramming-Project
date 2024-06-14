#include "hangman.h"

typedef struct {
    int socket;
    char word[BUFFER_SIZE];
    char buff[BUFFER_SIZE];
    int dead_count;
} ClientInfo;

ClientInfo clients[MAX_CLIENTS];

char *select_random_word(char **word_list)
{
    srand(time(NULL));
    int index = rand() % 100;
    char *random_word = word_list[index];
    printf("Answer : %s\n", random_word);
    return (random_word);
}

int check_word(const char *word, char c, char *buff)
{
    int found = 0;
    for (int i = 0; i < strlen(word); i++)
    {
        if (word[i] == c && buff[i] != c)
        {
            buff[i] = c;
            found = 1;
        }
    }
    return found;
}

ClientInfo* get_client_info(int socket)
{
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i].socket == socket)
            return &clients[i];
    }
    return NULL;
}

void remove_client(int socket)
{
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i].socket == socket)
        {
            clients[i].socket = 0;
            clients[i].word[0] = '\0';
            clients[i].buff[0] = '\0';
            clients[i].dead_count = 0;
            break;
        }
    }
}

void start_new_game(ClientInfo *client)
{
    char *word = select_random_word(word_list);
    strcpy(client->word, word);
    int len = strlen(word);
    for (int j = 0; j < len; j++)
    {
        client->buff[j] = '_';
    }
    client->buff[len] = '\0';
    client->dead_count = 0;
    printf("buff : %s\n", client->buff);
    send(client->socket, hangman_stages[0], 100, 0);
    send(client->socket, client->buff, len, 0);
}

void play_hangman(ClientInfo *client, char *read, int i)
{
    int result = check_word(client->word, read[0], client->buff);
    if (result == 0)
        client->dead_count++;
    if (strcmp(client->word, client->buff) == 0)
    {
        send(i, client->buff, strlen(client->buff), 0);
        send(i, win_msg, strlen(win_msg), 0);
        send(i, newgame_msg, strlen(newgame_msg), 0);
    }
    else if (client->dead_count == 6) // 죽었을 때
    {
        send(i, hangman_stages[6], strlen(hangman_stages[6]), 0);
        send(i, answer_msg, strlen(answer_msg), 0);
        send(i, client->word, strlen(client->word), 0);
        send(i, lose_msg, strlen(lose_msg), 0);
        send(i, start, strlen(start), 0);
    }
    else
    {
        send(i, hangman_stages[client->dead_count], 100, 0);
        send(i, client->buff, strlen(client->buff), 0);
    }
}

int main(void)
{
    memset(clients, 0, sizeof(clients));
    
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    struct addrinfo *bind_address;
    getaddrinfo(0, "8080", &hints, &bind_address); // 지정된 포트를 사용하여 소켓 주소 정보를 가져옴

    // 소켓 생성
    printf("CREATE SOCKET...\n");
    int sock_listen = socket(bind_address->ai_family, SOCK_STREAM, bind_address->ai_protocol);
    if (sock_listen < 0)
    {
        perror("socket() failed");
        return 1;
    }

    // 소켓에 주소 바인딩
    printf("BIND SOCKET...\n");
    if (bind(sock_listen, bind_address->ai_addr, bind_address->ai_addrlen) < 0)
    {
        perror("bind() failed");
        return 1;
    }
    freeaddrinfo(bind_address);

    // 연결 요청 대기
    printf("LISTENING...\n");
    if (listen(sock_listen, 10) < 0)
    {
        perror("listen() failed");
        return 1;
    }
    fd_set master;
    FD_ZERO(&master);
    FD_SET(sock_listen, &master);
    int max_socket = sock_listen;

    while (1)
    {
        fd_set reads;
        reads = master;
        if (select(max_socket + 1, &reads, 0, 0, 0) < 0)
        {
            perror("select() failed");
            return 1;
        }

        for (int i = 0; i <= max_socket; i++)
        {
            if (FD_ISSET(i, &reads))
            {
                if (i == sock_listen)
                {
                    struct sockaddr_storage client_address;
                    socklen_t client_len = sizeof(client_address);
                    int socket_client = accept(sock_listen, (struct sockaddr *)&client_address, &client_len); // 클라이언트 연결 수락

                    printf("New Client: %d\n", socket_client); // 연결된 클라이언트 번호 부여하여 출력
                    if (socket_client < 0)
                    {
                        perror("accept() failed");
                        continue;
                    }

                    FD_SET(socket_client, &master);
                    if (socket_client > max_socket)
                        max_socket = socket_client;
                    printf("client connected..\n");
                    send(socket_client, start, strlen(start), 0); //연결이 수립되고 나면 클라이언트에게 모드 선택 문자열 전송

                    // 새로운 클라이언트 정보 초기화
                    for (int j = 0; j < MAX_CLIENTS; j++)
                    {
                        if (clients[j].socket == 0)
                        {
                            clients[j].socket = socket_client;
                            clients[j].dead_count = 0;
                            break;
                        }
                    }
                }
                else
                {
                    char read[1024];
                    memset(read, 0, 1024);
                    int bytes_received = recv(i, read, 1024, 0); //client가 전송한 값 수신
                    if (bytes_received < 1)
                    {
                        if (bytes_received < 0)
                            perror("recv() failed");
                        else
                            printf("client disconnected..\n");
                        close(i);
                        FD_CLR(i, &master); //연결이 끊어졌을 경우 해당 소켓을 fd_set에서 제거
                        remove_client(i);
                        continue;
                    }
                    read[bytes_received] = '\0'; // 받은 문자열 뒤에 널 문자 추가
                    printf("client : %s\n", read);

                    ClientInfo *client = get_client_info(i);
                    if (!client)
                    {
                        printf("Client info not found for socket: %d\n", i);
                        continue;
                    }
                    if (read[0] == '1') //클라이언트가 1을 입력했을 경우
                    {
                        start_new_game(client);
                    }
                    else if (read[0] == '2') //2를 입력했을 경우
                    {
                        // fflush(stdout);
                        send(i, exit_msg, strlen(exit_msg), 0);
                        FD_CLR(i, &master);
                        remove_client(i);
                        close(i);
                        // fflush(stdout);
                    }
                    else if (read[0] >= 'a' && read[0] <= 'z')
                    {
                        play_hangman(client, read, i);
                    }
                }
            }
        }
    }
    printf("finished");
    return 0;
}