#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

int main(int argc, char **argv)
{

  printf("Hi");

  struct sockaddr_in saddr;

  int server;
  int client;
  int received;
  char buffer[129];
  char *testText = "Hello from client";

  server = socket(AF_INET, SOCK_STREAM, 0);

  saddr.sin_family = AF_INET;
  saddr.sin_port = htons(8080);

  // Converte o endereço de conexão de texto para binário (que é a forma que a rede usa)
  inet_pton(AF_INET, "127.0.0.1", &saddr.sin_addr);

  client = connect(server, (struct sockaddr *)&saddr, sizeof saddr);

  printf("Server, %d\n", server);
  printf("Client, %d\n", client);

  send(server, testText, strlen(testText), 0);

  received = read(server, buffer, 129);

  printf("received: %s\n", buffer);

  close(client);

  return 0;
}