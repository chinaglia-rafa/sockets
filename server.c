#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

/**
 * Aprendendo a usar sockets usando https://www.youtube.com/watch?v=GaxjJvMnz-I&ab_channel=LuizFelipe como
 * referência. Quaisquer nomes de variáveis coincidentes não são uma cópia.
 */
int main(int argc, char **argv)
{
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
      .sin_port = htons(8080),
  };
  // Outra struct do tipo sockaddr_in para ser usada na recepção de dados
  struct sockaddr_in caddr;
  // "Número do socket" o que diabos isso quer dizer gente?
  int client;
  // Essa amiguinha aqui vai armazenar o tamanho dos dados recebidos por recv()
  int recvsize;
  // Tamanaho da variável caddr para ser passada por parâmetro para accept()
  int csize = sizeof caddr;
  char buffer[129];

  printf("[hacked by chinaglia] Quick Simple Socket Server \x1b[33mFOR LINUX\x1b[37m 0.1\n");
  printf("\x1b[32m Starting server...\x1b[37m \n");

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

  printf("\x1b[32m Binding to port 6660! \x1b[37m \n");
  // Agora sim a gente tá fazendo um bind() do socket que a gente criou pra poder usar os comandos de receber
  // e enviar mensagens via rede.
  bind(server, (struct sockaddr *)&saddr, sizeof saddr);

  printf("\x1b[32m Ready to start listeing to up to 2 connections... \x1b[37m \n");

  // A função listen PREPARA (mas não ativamente escuta) o socket para conexões, e o segundo parâmetro diz
  // quantas eu posso ter ao mesmo tempo.
  listen(server, 2);

  printf("\x1b[32m Socket server started. \x1b[37m \n");
  while (1)
  {
    // Agora sim a gente tem uma conexão com o cliente.
    client = accept(server, (struct sockaddr *)&caddr, &csize);
    // A função recv() vai receber dados do socket e guardar em buffer, e retornar o tamanaho dos dados que
    // recebeu do cliente. O terceiro parâmetro diz qual o tamanho máximo que ela pode receber.
    recvsize = recv(client, buffer, sizeof buffer, 0);
    // A função send() envia dados pro cliente, simples assim.
    send(client, buffer, recvsize, 0);

    printf("Mensagem recebida: %s\n", buffer);
    fflush(stdout);

    close(client);
  }

  return 0;
}