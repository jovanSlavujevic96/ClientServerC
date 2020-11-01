// Server side C/C++ program to demonstrate Socket programming 
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 

#include <pthread.h> 

#define PORT 8080 

struct sockaddr_in address;
int addrlen = sizeof(address);
int server_fd;
int new_socket;
static const char *hello = "Hello from server"; 

int indexToFree = -1;

typedef struct ClientSocket_s
{
    int socket;
    pthread_t thread;
    int index;
}ClientSocket_s;

static void communicateWithClient(ClientSocket_s* socket);
void FreeDeadClient(void);

ClientSocket_s** SocketArray;
int ArraySize = 0;

static void initServer(void)
{
	int opt = 1; 
    // Creating socket file descriptor 
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
	{ 
		perror("socket failed"); 
		exit(EXIT_FAILURE); 
	} 

    // Forcefully attaching socket to the port 8080 
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, 
												&opt, sizeof(opt))) 
	{ 
		perror("setsockopt"); 
		exit(EXIT_FAILURE); 
	}

    address.sin_family = AF_INET; 
	address.sin_addr.s_addr = INADDR_ANY; 
	address.sin_port = htons( PORT ); 

    // Forcefully attaching socket to the port 8080 
	if (bind(server_fd, (struct sockaddr *)&address, 
								sizeof(address))<0) 
	{ 
		perror("bind failed"); 
		exit(EXIT_FAILURE); 
	} 
	if (listen(server_fd, 3) < 0) 
	{ 
		perror("listen"); 
		exit(EXIT_FAILURE); 
	}
}

static void receiveNewClient(void)
{
    pthread_t threadKillSocket;
    pthread_create(&threadKillSocket, NULL, (void*)&FreeDeadClient, NULL);
    while(1)
    {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, 
	    				(socklen_t*)&addrlen))<0) 
	    { 
	    	perror("accept"); 
	    	break; 
	    }
        //alloc array
        if(ArraySize == 0)
        {
            SocketArray = (ClientSocket_s**)malloc(sizeof(ClientSocket_s*));
        }
        else
        {
            SocketArray = (ClientSocket_s**)realloc(SocketArray, sizeof(ClientSocket_s*));
        }
        SocketArray[ArraySize] = (ClientSocket_s*)malloc(sizeof(ClientSocket_s));
        SocketArray[ArraySize]->socket = new_socket;
        SocketArray[ArraySize]->index = ArraySize;
        pthread_create(&SocketArray[ArraySize]->thread, NULL, (void*)&communicateWithClient, SocketArray[ArraySize]); 
        ArraySize++;
    }
}

static void communicateWithClient(ClientSocket_s* socket)
{
    int valread;
    char buffer[1024] = {0}; 
    while(1)
    {
        valread = read(socket->socket , buffer, 1024); 
        if(valread <= 0)
        {
            break;
        }
        printf("socket ID %d index %d %s received\n",socket->socket, socket->index, buffer); 
        send(socket->socket , hello , strlen(hello) , 0 ); 
        printf("socket ID %d index %d Hello message sent\n", socket->socket, socket->index); 
    }
    indexToFree = socket->index;
}

void FreeDeadClient(void)
{
    pthread_mutex_t lock;
    while(1)
    {
        if(indexToFree == -1)
        {
            continue;
        }
        pthread_mutex_lock((pthread_mutex_t*)&lock);
        close(SocketArray[indexToFree]->socket);
        SocketArray[ArraySize-1]->index = SocketArray[indexToFree]->index; 
        free(SocketArray[indexToFree]);
        SocketArray[indexToFree] = SocketArray[ArraySize-1];
        ArraySize--;
        SocketArray = (ClientSocket_s**)realloc(SocketArray, sizeof(ClientSocket_s*) * ArraySize);
        indexToFree = -1;
        pthread_mutex_unlock((pthread_mutex_t*)&lock);
    }
}

int main(int argc, char const *argv[]) 
{ 
    initServer();

    receiveNewClient();

    return 0; 
} 
