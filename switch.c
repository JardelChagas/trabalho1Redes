#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

socklen_t fromlen;
char BUFFER[255],limparB[255];
struct addrinfo *addrS, *addrC, *cliaddr;
enum estados{
    iniciando,
    comunicando,
    finalizando,
    encerrado
};

int verifica_execucao(int argc);
int meuSocketC(const char *host, const char *service);
int meuSocketS(const char *host, const char *service);
int meuSend(int sock, char *string, int size_string);
int meuRecv(int sock, char *buf, int size_buf);
int meuRecv2(int sock, char *buf, int size_buf);
int meuSend2(int sock, char *string, int size_string);

int main(int argc, char const *argv[]){
   
    verifica_execucao(argc);

    int estado_atual = iniciando;
    int servSock, clntSock;
    const char *host = argv[1];
    const char *serviceC = argv[3];
    const char *serviceS = argv[2];
    //printf("host -> %s\nport_1 -> %s\nport_2 -> %s\n", host, serviceS, serviceC);
    while (estado_atual != encerrado){
        switch (estado_atual){
        case iniciando:
            servSock = meuSocketS(host, serviceS);
            clntSock = meuSocketC(host, serviceC);
            estado_atual = comunicando;
            
            meuRecv(servSock, (void *)&BUFFER, sizeof(BUFFER));
            char *txt = BUFFER;
            printf("recebir da origem: %s\n",txt);
            meuSend(clntSock, txt, strlen(txt));            
            meuRecv2(clntSock, (void *)&BUFFER, sizeof(BUFFER));
            txt = BUFFER;
            printf("recebir do destino: %s\n",BUFFER);
            meuSend2(servSock, txt, strlen(txt));
            memset(BUFFER,0,255);
            break;
        case comunicando:
            meuRecv(servSock, (void *)&BUFFER, sizeof(BUFFER));
            txt = BUFFER;
            printf("recebir da origem: %s\n",txt);
            meuSend(clntSock, txt, strlen(txt));
            meuRecv2(clntSock, (void *)&BUFFER, sizeof(BUFFER));
            printf("recebir do destino: %s\n",BUFFER);
            txt = BUFFER;
            meuSend2(servSock, txt, strlen(txt));
            memset(BUFFER,0,255);
            
            estado_atual = finalizando;
            break;
        case finalizando:
            estado_atual = encerrado;
            close(servSock);
            close(clntSock);
            break;
        default:
            break;
        }
    }

    return EXIT_SUCCESS;
}

int verifica_execucao(int argc){
     if (argc != 4){
        perror("Execução correta: ./server <porta origem> <porta destino>\n");
        return EXIT_FAILURE;
    }
}

int meuSocketC(const char *host, const char *service){
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
    for (addrC = servAddr; addrC != NULL; addrC = addrC->ai_next){
        sock = socket(addrC->ai_family, addrC->ai_socktype, addrC->ai_protocol);
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
int meuSocketS(const char *host, const char *service){
    
    struct addrinfo addrCriteria;                   
    memset(&addrCriteria, 0, sizeof(addrCriteria)); 
    addrCriteria.ai_family = AF_UNSPEC;             
    addrCriteria.ai_flags = AI_PASSIVE;            
    addrCriteria.ai_socktype = SOCK_DGRAM;         
    addrCriteria.ai_protocol = IPPROTO_UDP;         

    struct addrinfo *servAddr; 
    int rtnVal = getaddrinfo(NULL, service, &addrCriteria, &servAddr);
    if (rtnVal != 0)
    {
        perror("getaddrinfo() falhou\n");
        exit(EXIT_FAILURE);
    }

    int servSock = -1;
    for (addrS = servAddr; addrS != NULL; addrS = addrS->ai_next){
 
        servSock = socket(addrS->ai_family, addrS->ai_socktype, addrS->ai_protocol);
        if (servSock < 0)
            continue; 

        if (bind(servSock, addrS->ai_addr, addrS->ai_addrlen) == -1){
            close(servSock);
            perror("server: bind");
            continue;
        }else{
            break;
        }
        
        close(servSock);
        servSock = -1;
    }
    if (addrS == NULL) {
        printf("erro bind\n");
    }

    freeaddrinfo(servAddr);
    return servSock;
}
int meuRecv(int sock, char *buf, int size_buf){
    
    fromlen = sizeof(cliaddr);
    
    if ((recvfrom(sock, (void *)BUFFER, sizeof(BUFFER), 0,
        (struct sockaddr *)&cliaddr, &fromlen)) == -1)
    {
        perror("recvfrom");
        exit(1);
    }
    if(cliaddr==NULL){
        printf("ad");
    }
    return 1;
}
int meuRecv2(int sock, char *buf, int size_buf){
    struct sockaddr_storage p;
    socklen_t fromlen;
    fromlen = sizeof p;

    /*if (addr == NULL){
        fprintf(stderr, "server: falha para o socket\n");
        return 2;
    }*/
    
    if ((recvfrom(sock, (void *)&BUFFER, sizeof(BUFFER), 0,
        (struct sockaddr *)&p, &fromlen)) == -1)
    {
        perror("recvfrom");
        exit(1);
    }
    return 1;
}
int meuSend(int sock, char *string, int size_string){
    
    if ((sendto(sock, string, size_string, 0,
        addrC->ai_addr, addrC->ai_addrlen)) == -1)
    {
        perror("client: sendto, ");
        exit(1);
    }
}
int meuSend2(int sock, char *string, int size_string){
    if(cliaddr == NULL){
        puts("cli ta null");
    }
    if ((sendto(sock, string, size_string, 0,
        (struct sockaddr *)&cliaddr, fromlen)) == -1)
    {
        perror("client: sendto, ");
        exit(1);
    }
    return 1;
}

