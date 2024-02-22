#include <sys/socket.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <poll.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "args.h"
#include "server.h"


void *handleConnection(void *arg) {
  struct connectionArgs *args = (struct connectionArgs *)arg;
  char buffer[255] = { 0 };
  int clientfd = args->array[args->index];
  while(read(clientfd, buffer, 255) != -1) { //(*args).clientfd (equivalent)
    for(int i = 0; i < *(args->length); i++) {
      int peer = args->array[i];
      if (peer == clientfd) {
        continue;
      }
      send(peer, buffer, 255, 0);
    }
  }

  args->array[args->index] = -1;
  free(arg);
}


void initServer(Args args) {
  printf("Initializing Server\n");
  fflush(stdout);
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);

  struct sockaddr_in address = {
    .sin_family = AF_INET,
    .sin_port = htons(args.port), // Error check this!
    .sin_addr = INADDR_ANY
  };
  
  int res = bind (sockfd, (struct sockaddr *)&address, sizeof(address));
  res = listen(sockfd, 10);
  
  int *clientFdArray = malloc(sizeof(int) * 8);
  int clientFdArrayLen = 8;
  int clientFdArrayIndex = 0;

  for(;;) {
    int clientfd = accept(sockfd, 0, 0);
    clientFdArray[clientFdArrayIndex] = clientfd;
    
    pthread_t newThread;
    struct connectionArgs *args = (struct connectionArgs *)malloc(sizeof(struct connectionArgs));
    *args = (struct connectionArgs) {
      .array = clientFdArray,
      .index = clientFdArrayIndex,
      .length = &clientFdArrayLen
    };
    clientFdArrayIndex++;
    if (clientFdArrayIndex = clientFdArrayLen - 1) {
      //resize
      clientFdArrayLen *= 2;
      clientFdArray = realloc(clientFdArray, sizeof(int) * clientFdArrayLen);
    }

    int result = pthread_create(&newThread, NULL, handleConnection, args);
  }
  
  
  
}