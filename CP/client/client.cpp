#include <stdio.h>	//printf
#include <string.h>	//strlen
#include <sys/socket.h>	//socket
#include <unistd.h>
#include <arpa/inet.h>	//inet_addr
#include <string>
#include <iostream>


using std::string;
using std::cin;

bool connected = true;

void* messageListener(void* args) {
    int sock = *((int*)args);
    char buffer[2000];
    while (connected) {
	    if(read(sock , buffer , 2000) <= 0)
		{
			puts("read failed");
            connected = false;
            exit(1);
			break;
		}
        char* token = strtok(buffer, " ");
        if (strcmp(token, "message") == 0) {
            char* from = strtok(NULL, " ");
            char* what = strtok(NULL, " ");
            printf("[%s]: %s\n", from, what);
        }
    }    
    return NULL;
}

int main(int argc , char *argv[])
{
    int sock;
	struct sockaddr_in server;
    char server_reply[2000];
    char message[2000];
	//Create socket
	sock = socket(AF_INET , SOCK_STREAM , 0);
	if (sock == -1)
	{
		printf("Could not create socket");
	}
	puts("Socket created");
	
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_family = AF_INET;
	server.sin_port = htons( 8080 );
    
	//Connect to remote server
	if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		perror("connect failed. Error");
		return 1;
	}

	
	puts("Connected\n");
    pthread_t listenThread;
    pthread_create(&listenThread, NULL, messageListener, (void*)&sock);
	
	//keep communicating with server
    bool firstCommand = true;
	while(connected)
	{
		printf("> ");
        string cmd;
        cin >> cmd;
        if (firstCommand) {
            firstCommand = false;
            if (cmd == "login" || cmd == "register") {
                string login, password;
                cin >> login >> password;
                strcpy(message, cmd.c_str());
                strcat(message, " ");
                strcat(message, login.c_str());
                strcat(message, " ");
                strcat(message, password.c_str());
                if( send(sock , message , strlen(message) , 0) < 0)
                {
                    puts("Send failed");
                    return 1;
                }          
            } else {
                printf("Command is not allowed, you need to login or register first.\n");
            }
        } else {
            if (cmd == "send") {
                string who, what;
                cin >> who >> what;
                strcpy(message, cmd.c_str());
                strcat(message, " ");
                strcat(message, who.c_str());
                strcat(message, " ");
                strcat(message, what.c_str());
                if( send(sock , message , strlen(message) , 0) < 0)
                {
                    puts("Send failed");
                    return 1;
                }
            }
        }
	}
	
	//close(sock);
	return 0;
}
