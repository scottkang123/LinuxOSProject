#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include "socket.h"

#define BUFFER_LEN 256

int main() {
  // Open a server socket
  unsigned short port = 0;
  int server_socket_fd = server_socket_open(&port);
  if(server_socket_fd == -1) {
    perror("Server socket was not opened");
    exit(2);
  }
  
  // Start listening for connections, with a maximum of one queued connection
  if(listen(server_socket_fd, 1)) {
    perror("listen failed");
    exit(2);
  }
  
  printf("Server listening on port %u\n", port);
  
  // Wait for a client to connect
  int client_socket_fd = server_socket_accept(server_socket_fd);
  if(client_socket_fd == -1) {
    perror("accept failed");
    exit(2);
  }
  
  printf("Client connected!\n");
  
  // Set up file streams to access the socket
  FILE* to_client = fdopen(dup(client_socket_fd), "wb");
  if(to_client == NULL) {
    perror("Failed to open stream to client");
    exit(2);
  }
  
  FILE* from_client = fdopen(dup(client_socket_fd), "rb");
  if(from_client == NULL) {
    perror("Failed to open stream from client");
    exit(2);
  }
  
  // Send a message to the client
  //fprintf(to_client, "Hello client!\n");
  
  // Flush the output buffer
  //fflush(to_client);
  
  // Receive a message from the client
  char buffer[BUFFER_LEN];
  
  while(1){
	  if(fgets(buffer, BUFFER_LEN, from_client) == NULL) {
	      perror("Reading from client failed");
              exit(2);
	  }
	  printf("Client sent: %s\n", buffer);
	  int i = 0;
	  while (buffer[i]) {
              buffer[i] = toupper(buffer[i]);
              i++;
          }
	  
	//  if(strcmp(buffer, "quit\n") == 0) {
      	//	break;
    	  //}
	  
	  if(fprintf(to_client, "%s", buffer) < 0){
	      perror("Failed to send message to client");
	  }
	  
	  fflush(to_client);  //fflush needs to be before printf
	  
	  
	  
  }
  
  
  
 
  // Close file streams
  fclose(to_client);
  fclose(from_client);
  
  // Close sockets
  close(client_socket_fd);
  close(server_socket_fd);
  
  return 0;
}





















