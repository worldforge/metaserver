#include <netinet/in.h>
#include "wrap.h"
#include "dgecholoop.h"

#define SERV_PORT           8453

int main(int argc, char **argv)
{
  int sockfd;
  struct sockaddr_in servaddr, cliaddr;

  sockfd = Socket(AF_INET, SOCK_DGRAM, 0);

  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family      = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port        = htons(SERV_PORT);

  Bind(sockfd, (SA *)&servaddr, sizeof(servaddr));

  dg_echo(sockfd, (SA *)&cliaddr, sizeof(cliaddr));

  return 0;
}
