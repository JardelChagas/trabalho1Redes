#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

const char BUFFER[255];
struct addrinfo *addr;
int meuSocket(const char *host, const char *service){
    struct addrinfo addrCriteria;
    memset(&addrCriteria, 0, sizeof(addrCriteria));
    addrCriteria.ai_family = AF_UNSPEC;
    addrCriteria.ai_socktype = SOCK_DGRAM;
    addrCriteria.ai_protocol = IPPROTO_UDP;

    struct addrinfo *servAddr;
    int rtnVal = getaddrinfo(host, service, &addrCriteria, &servAddr);
    
    if (rtnVal != 0){
        perror("getaddrinfo() falhou\n");
        exit(EXIT_FAILURE);
    }

    int sock = -1;
    for (addr = servAddr; addr != NULL; addr = addr->ai_next){
        sock = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
        if (sock < 0)
            continue;

        /*if (bind(sock, addr->ai_addr, addr->ai_addrlen) == -1){
            close(sock);
            perror("server: bind");
            continue;
        }*/

        break;
    }
    freeaddrinfo(servAddr);
    return sock;
    
}
int meuConnect(int sock){
    meuSend(sock,"SYN", 3);//envia o SYN
    puts("Pedindo conexão.");
    meuRecv(sock, (char *)BUFFER, sizeof(BUFFER));
    puts("Conexão estabelecida.");
}
int meuSend(int sock, char *string, int size_string){
    
    if ((sendto(sock, string, size_string, 0,
        addr->ai_addr, addr->ai_addrlen)) == -1)
    {
        perror("client: sendto, ");
        exit(1);
    }    
}
int meuRecv(int sock, char *buf, int size_buf){
    struct sockaddr_storage p;
    socklen_t fromlen;
    fromlen = sizeof p;

    if (addr == NULL){
        fprintf(stderr, "server: falha para o socket\n");
        return 2;
    }
    
    if ((recvfrom(sock, (void *)&BUFFER, size_buf, 0,
        (struct sockaddr *)&p, &fromlen)) == -1)
    {
        perror("recvfrom");
        exit(1);
    }
    return 1;
}
/*minhas fuções*/
int verifica_execucao(int argc){
    if (argc != 3){
        perror("Execução correta: ./chat-client <ip> [<porta>]\n");
        return EXIT_FAILURE;
    }
    return 1;
}
int verificaSock(int s){
    if (s < 0) {
        perror("meuSocket() falhou.\n");
        return EXIT_FAILURE;
    }
    return 1;   
}