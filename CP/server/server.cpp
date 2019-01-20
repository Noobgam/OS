#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <map>
#include <string>
#include <thread>
#include <set>
#include <chrono>
#include <thread>
#include <iostream>
#include <queue>
#include <string>

using namespace std;

bool disposed = false;

set <int> onlineUsers;
map <string, int> userId; 
map <int, string> userName;
map <string, string> registered;
map <int, pthread_t> servingThread; 
map <int, int> userSocket;
map <int, queue<string>> msgqueue;
int globalId = 0;

void* messageUser(void* args) {
    int id = *((int*)args);
    int socketId = userSocket[id];
    char msg[1024];
    while (userSocket.count(id)) {
        if (msgqueue.count(id) && msgqueue[id].size() > 0) {
            string temp = msgqueue[id].front();
            msgqueue[id].pop();
            memset(msg, 0, sizeof(msg));
            strcpy(msg, "message ");
            strcat(msg, temp.c_str());
            write(socketId, msg, strlen(msg));
        }
    }
    return NULL;
}

void* serveUser(void* args) {
    pthread_t messenger;
    int id = *((int*)args);
    string myName = userName[id];
    pthread_create(&messenger, NULL, messageUser, (void*)&id);
    int socketId = userSocket[id];
    fprintf(stderr, "Listening socket %d\n", socketId);
    bool connected = true;    
    char buffer[1024];
    char buffer2[1024];
    while (!disposed) {
        memset(buffer, 0, sizeof(buffer));
        int valread = read( socketId , buffer, 1024); 
        if (valread <= 0) {
            printf("User %d disconnected\n", id);
            break;
        }
        printf("User %d sent: %s\n", id, buffer);        
        char* token = strtok(buffer, " ");
        if (strcmp(token, "send") == 0) {
            char* who = strtok(NULL, " ");
            char* what = strtok(NULL, " ");
            if (userId.count(who) == 0) {
                continue;
            }            
            int otherId = userId[who];
            memset(buffer2, 0, sizeof(buffer2));
            strcpy(buffer2, myName.c_str()),
            strcat(buffer2, " ");
            strcat(buffer2, what);
            msgqueue[otherId].push(buffer2);
        }
    }    
    servingThread.erase(id);
    userSocket.erase(id);    
    pthread_join(messenger, NULL);
    return NULL;
}

void* handleSockets(void* args) {
    int PORT = *((int*)args);
    int server_fd, new_socket, valread; 
    struct sockaddr_in address; 
    int opt = 1; 
    int addrlen = sizeof(address); 
    char buffer[1024] = {0}; 
    string hello = "Hello from server"; 
           
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    { 
        perror("Socket allocation failed"); 
        exit(EXIT_FAILURE);
    } 
       
    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons( PORT );
       
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) 
    { 
        perror("Couldn't bind socket"); 
        exit(EXIT_FAILURE);
    } 
    if (listen(server_fd, 10) < 0) 
    { 
        perror("Can't listen"); 
        exit(EXIT_FAILURE);
    }
    int lastId = -1;
    while (!disposed) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address,  
                           (socklen_t*)&addrlen))<0) 
        {         
            perror("accept"); 
            continue; 
        }
        memset(buffer, 0, sizeof(buffer));
        valread = read( new_socket , buffer, 1024); 
        printf("%s\n", buffer);
        char* token = strtok(buffer, " ");
        if (strcmp(token, "register") == 0) {
            char* name = strtok(NULL, " ");
            char* password = strtok(NULL, " ");
            if (registered.count(name)) {
                close(new_socket);
                continue;
            }
            registered[name] = password;
            lastId = globalId++;
            userId[name] = lastId;
            userName[lastId] = name;
            userSocket[lastId] = new_socket;
            fprintf(stderr, "New socket %d\n", new_socket);
            pthread_create(&servingThread[lastId], NULL, serveUser, (void*)&lastId);
        } else if (strcmp(token, "login") == 0) {
            char* name = strtok(NULL, " ");
            char* password = strtok(NULL, " ");
            if (registered.count(name) == 0 || strcmp(registered[name].c_str(), password) != 0) {
                close(new_socket);
                continue;
            }
            lastId = userId[name];
            userSocket[lastId] = new_socket;
            pthread_create(&servingThread[lastId], NULL, serveUser, (void*)&lastId);
        } else {
            close(new_socket);            
        }
//        printf("%s\n",buffer ); 
//        send(new_socket , hello.c_str() , hello.length(), 0 ); 
    }
    return NULL;
}

int main(int argc, char const *argv[]) 
{ 
	if (argc <= 1) {
		printf("Please provide a port to listen on\n");
        exit(EXIT_FAILURE); 
	}
    pthread_t socketHandler;
    int port = atoi(argv[1]);
    pthread_create(&socketHandler, NULL, handleSockets, (void*)&port); 
    string command;
    while (cin >> command) {
        if (command == "exit") {
            disposed = true;
            pthread_join(socketHandler, NULL);
            break;
        }
    }
    return 0; 
} 
