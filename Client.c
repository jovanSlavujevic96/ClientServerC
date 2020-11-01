// Client side C/C++ program to demonstrate Socket programming 
#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h> 
#include <stdbool.h>
#include <time.h>

#define PORT 8080 

int main(int argc, char const *argv[]) 
{ 
	int sock = 0, valread; 
	struct sockaddr_in serv_addr; 
	const char *hello = "Hello from client"; 
	char buffer[1024] = {0}; 
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{ 
		printf("\n Socket creation error \n"); 
		return -1; 
	} 

	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_port = htons(PORT); 
	
	// Convert IPv4 and IPv6 addresses from text to binary form 
	if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) 
	{ 
		printf("\nInvalid address/ Address not supported \n"); 
		return -1; 
	} 

	uint8_t dTryCounter = 0;
	int ret;
	while ( (ret = connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr))) < 0 && (dTryCounter++) < 5 )
	{
		usleep(1000000); 
	}
	if(ret < 0)
	{
		printf("Time elapsed\n");
		return -1;
	}

	while(true)
	{
		send(sock , hello , strlen(hello) , 0 ); 
		printf("Hello message sent\n"); 
		valread = read( sock , buffer, 1024);
		if(valread < 0)
		{
			break;
		} 
		printf("%s\n",buffer );
		usleep(500000);   
	}
	return 0;
} 
