#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

#define PORT 8080
#define INPUT_LENGTH 140
#define SEPARATOR " "

/**
 * Aprendendo a usar sockets usando https://www.youtube.com/watch?v=GaxjJvMnz-I&ab_channel=LuizFelipe como
 * referência. Quaisquer nomes de variáveis coincidentes não são uma cópia.
 */
int main(int argc, char **argv)
{
  // Recebe uma flag -d | --debug para exibir logs mais completos
  int debug = 0;
  if (argc == 2 && (strcmp(argv[1], "-d") == 0 || strcmp(argv[1], "--debug") == 0))
    debug = 1;
  if (debug)
    printf("\x1b[33m[debug] Debug is ON.\x1b[0m \n");
  // Esse struct representa as informações presentes no processo de bind().
  // As funções htonl() e htons() são funções que convertem palavras numéricas (variáveis tipo int ou long)
  // cuja organização de bits é de um tipo para o tipo necessário para o diálogo em rede. Para mais informações
  // e uma documentação muito bem-humorada, confira https://www.gta.ufrj.br/ensino/eel878/sockets/htonsman.html
  struct sockaddr_in saddr = {
      // protocolo da conexão (essa, por exemplo, é para a internet e usa TCP e IPV4)
      .sin_family = AF_INET,
      // Endereço local usado pelo socket. No caso, o INADDR_ANY indica
      // "qualquer endereço que a rede especifique"
      .sin_addr.s_addr = htonl(INADDR_ANY),
      // duh
      .sin_port = htons(PORT),
  };
  // Outra struct do tipo sockaddr_in para ser usada na recepção de dados
  struct sockaddr_in caddr;
  // "Número do socket" o que diabos isso quer dizer gente?
  int client;
  // Essa amiguinha aqui vai armazenar o tamanho dos dados recebidos por recv()
  int recvsize;
  // Tamanaho da variável caddr para ser passada por parâmetro para accept()
  int csize = sizeof caddr;
  char buffer[INPUT_LENGTH];

  printf("[hacked by chinaglia] Quick Simple Socket Server \x1b[33mFOR LINUX\x1b[0m 0.1\n");
  printf("\x1b[32mStarting server...\x1b[0m \n");

  // Instanciando o socket em si, lembrando que instanciar um socket não é a mesma coisa de publicar ele em rede
  // ou deixá-lo ativo para conexões. Aqui a gente só está definindo como ele deverá se comportar quando as
  // outras partes estiverem em seus lugares.
  // - AF_INET protocolo de conexão (esse usa IPV4)
  // - SOCK_STREAM é o tipo de conexão que se mantém entre cliente e servidor, trocando várias mensagens e
  //               fechando apenas quando alguém mandar (usa TCP). (a outra opção seria SOCK_DGRAM que fecha
  //               a conexão logo que a comunicação é feita)
  // - 0 é usado para identificar que não queremos forçar um tipo de protocolo, mas sim usar o que é ditado
  //     pelos outros parâmetros.
  int server = socket(AF_INET, SOCK_STREAM, 0);

  printf("\x1b[32mBinding to port %d! \x1b[0m \n", PORT);
  // Agora sim a gente tá fazendo um bind() do socket que a gente criou pra poder usar os comandos de receber
  // e enviar mensagens via rede.
  bind(server, (struct sockaddr *)&saddr, sizeof saddr);

  printf("\x1b[32mReady to start listeing to up to 2 connections... \x1b[0m \n");

  // A função listen PREPARA (mas não ativamente escuta) o socket para conexões, e o segundo parâmetro diz
  // quantas eu posso ter ao mesmo tempo.
  listen(server, 2);

  printf("\x1b[32mSocket server started. \x1b[0m \n");
  // nome do usuário em sessão;
  char username[30];
  while (1)
  {
    // Agora sim a gente tem uma conexão com o cliente.
    client = accept(server, (struct sockaddr *)&caddr, &csize);
    if (client <= 0)
    {
      printf("\x1b[32mConnection failed. Something went wrong.\x1b[0m \n");
      continue;
    }
    else
      printf("\x1b[32mNew connection stabilished with ID %d. \x1b[0m \n", client);

    send(client, "[200] Hello! Identify yourself with USER <username>.", strlen("[200] Hello! Identify yourself with USER <username>."), 0);
    strcpy(buffer, "...");
    while (strcmp(buffer, "QUIT") != 0)
    {
      // A função recv() vai receber dados do socket e guardar em buffer, e retornar o tamanaho dos dados que
      // recebeu do cliente. O terceiro parâmetro diz qual o tamanho máximo que ela pode receber.
      recvsize = recv(client, buffer, sizeof buffer, 0);
      // Verifica se houve algum erro de conexão durante o loop de comunicação, como por exemplo
      // caso o cliente tenha sido fechado sem aviso prévio.
      if (recvsize <= 0)
      {
        printf("\x1b[32m[301] Connection with ID %d failed. Disconnecting... \x1b[0m \n", client);
        // Sai do loop de comunicação e volta a esperar uma conexão em accept()
        break;
      }
      if (debug)
        printf("\x1b[37m[debug] Data received:\x1b[0m \x1b[33m\"%s\"\x1b[0m\n", buffer);

      char *backupbuffer = malloc(strlen(buffer));
      strcpy(backupbuffer, buffer);

      // PROCESSA O COMANDO RECEBIDO
      // Isso vai ficar bem bagunçado bem rápido, mas o tempo não permite muita estética no código-fonte
      // no momento.
      char *token;
      token = strtok(buffer, SEPARATOR);
      if (strcmp(token, "USER") == 0)
      {
        char *user;
        user = strtok(NULL, SEPARATOR);
        if (debug)
          printf("\x1b[37m[debug] Param #01 is:\x1b[0m \x1b[33m\"%s\"\x1b[0m\n", user);
        strcpy(username, user);
        printf("\x1b[32m[200] Username set to %s.\x1b[0m\n", username);
        char *res = malloc(strlen("[200] New username set. Hello .") + strlen(username));
        strcpy(res, "[200] New username set. Hello ");
        strcat(res, username);
        strcat(res, ".");
        // A função send() envia dados pro cliente, simples assim.
        send(client, res, strlen(res), 0);
      }
      else if (strcmp(token, "HELP") == 0)
      {
        char *res = "HELP - shows help screen\nUSER <username> - changes username\nSAY <text> - Broadcasts text\nQUIT - Closes connection with client\n";
        send(client, res, strlen(res), 0);
      }
      else if (strcmp(token, "SAY") == 0)
      {
        send(client, backupbuffer, strlen(backupbuffer), 0);
      }
      else if (strcmp(token, "QUIT") == 0)
      {
        send(client, "[200] Closing connection. Bye!", strlen("[200] Closing connection. Bye!"), 0);
      }
      else
      {
        send(client, "[300] Command unknown. Try again.", strlen("[300] Command unknown. Try again."), 0);
      }

      // send(client, buffer, recvsize, 0);
      fflush(stdout);
    }
    printf("\x1b[32mConnection with ID %d is now closed. \x1b[0m\n", client);
    close(client);
  }

  return 0;
}