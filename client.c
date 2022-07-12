#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

#define INPUT_LENGTH 140

void command_help()
{
  printf("To send a message just type it and press enter. For a lista of avaliable commands, check the list below:\n\n");
  printf("\x1b[1m/help\x1b[0m - shows this screen.\n");
  printf("\x1b[1m/exit\x1b[0m - closes the client.\n");
  printf("\x1b[1m/status\x1b[0m - check connection status.\n");
  printf("\x1b[1m/user\x1b[0m - changes username.\n");
}

void command_status(char *target_addr, int target_port, char *username, int client)
{
  if (client < 0)
    printf("\x1b[1mConnection:\x1b[0m offline\n");
  else
    printf("\x1b[1mConnection:\x1b[0m online with client %d\n", client);

  printf("\x1b[1mAddress:\x1b[0m %s\n", target_addr);
  printf("\x1b[1mPort:\x1b[0m %d\n", target_port);
  printf("\x1b[1mCurrent user:\x1b[0m %s\n", username);
}

void command_user(char *username)
{
  printf("New username: ");
  scanf("%s", username);
  getchar();
}

int main(int argc, char **argv)
{

  char target_addr[200];
  int target_port = 8080;

  printf("Starting client with ");
  if (argc == 1)
  {
    strcpy(target_addr, "127.0.0.1");
    printf("default parameters:\n");
  }
  else if (argc == 3)
  {
    strcpy(target_addr, argv[1]);
    target_port = atoi(argv[2]);
    printf("custom parameters:\n");
  }

  printf(" - server: \x1b[32m%s\x1b[0m \n - port:   \x1b[32m%d\x1b[0m\n", target_addr, target_port);

  struct sockaddr_in saddr;

  int server;
  int client;
  int received;
  char buffer[INPUT_LENGTH];
  char *testText = "Hello from client";

  server = socket(AF_INET, SOCK_STREAM, 0);

  saddr.sin_family = AF_INET;
  saddr.sin_port = htons(target_port);

  // Converte o endereço de conexão de texto para binário (que é a forma que a rede usa)
  inet_pton(AF_INET, target_addr, &saddr.sin_addr);

  client = connect(server, (struct sockaddr *)&saddr, sizeof saddr);

  if (client < 0)
  {
    printf("\n\x1b[31mFatal error: could not connect to %s:%d! Is the server on?\x1b[0m\n", target_addr, target_port);
    return 1;
  }

  printf("Connection stabilished. Send a message or try a command like \x1b[33m/help\x1b[0m.\n");

  char input[INPUT_LENGTH];
  char username[30] = "anon";
  while (strcmp(input, "/exit") != 0)
  {
    printf("[@%s] >> ", username);
    fgets(input, INPUT_LENGTH, stdin);
    // Remove o \n que o fgets coloca no final da string
    input[strcspn(input, "\n")] = '\0';

    if (strcmp(input, "/help") == 0)
      command_help();
    else if (strcmp(input, "/status") == 0)
      command_status(target_addr, target_port, username, client);
    else if (strcmp(input, "/user") == 0)
      command_user(username);
    else
    {
      send(server, input, sizeof input, 0);
      received = read(server, buffer, INPUT_LENGTH);
      printf("received: %s\n", buffer);
    }
  }

  close(client);

  return 0;
}