#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

socklen_t fromlen;
int sock;
const char BUFFER[255];
struct addrinfo *addr, *cliaddr;
enum estados{
    iniciando,
    comunicando,
    finalizando,
    encerrado
};

int meuSocket(const char *host, const char *service);
int meuAccept();
int meuListen();
int meuSend(int sock, char *string, int size_string);
int meuRecv(int sock, char *buf, int size_buf);
int verifica_execucao(int argc);

int main(int argc, char const *argv[]){
    int estado_atual = iniciando;
    const char *host = argv[1];
    const char *service = argv[2];

    memset(&cliaddr, 0, sizeof(cliaddr));

    while (estado_atual != encerrado){
        switch (estado_atual){
            case iniciando:
                verifica_execucao(argc);
                sock = meuSocket(host, service);
                meuListen();
                estado_atual = comunicando;
                break;
            case comunicando:
                meuAccept();
                meuRecv(sock, (void *)&BUFFER, sizeof(BUFFER));
                printf("\nrecebir: %s\n",BUFFER);
                char *xx = "TESTANDO";
                meuSend(sock,xx,8);
                printf("mandando -> %s\n",xx);
                estado_atual = finalizando;
                break;
            case finalizando:
                estado_atual = encerrado;
                break;
            default:
                break;
            }
        }
    return EXIT_SUCCESS;
}

int meuSocket(const char *host, const char *service){
    
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
    for (addr = servAddr; addr != NULL; addr = addr->ai_next){
 
        servSock = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
        if (servSock < 0)
            continue; 

        if (bind(servSock, addr->ai_addr, addr->ai_addrlen) == -1){
            close(servSock);
            perror("server: bind");
            continue;
        }else{
            break;
        }
        
        close(servSock);
        servSock = -1;
    }
    if (addr == NULL) {
        printf("erro bind\n");
    }

    freeaddrinfo(servAddr);
    return servSock;
}

int meuListen(){
    puts("Iniciando escuta");
}

int meuAccept(){
    puts("Esperando conexão");
    meuRecv(sock, (char *)BUFFER, sizeof(BUFFER));
    /*
    if (strcmp (BUFFER, "SYN") == 0)
        puts(":)");
    printf("string: %s",BUFFER);
    */
    //printf("chegou: %s\n",BUFFER);
    puts("Conexão pedida");
    meuSend(sock, "SYNACK", 7);
    puts("Conexão aceita.");
    return 1;
}

int meuSend(int sock, char *string, int size_string){
    if(cliaddr == NULL){
        puts("cli ta null");
    }
    //printf("\tmas pq dissoo %s\n", string);
    if ((sendto(sock, (void *)string, size_string, 0,
        (struct sockaddr *)&cliaddr, fromlen)) == -1)
    {
        perror("client: sendto, ");
        exit(1);
    }
    return 1;
}

int meuRecv(int sock, char *buf, int size_buf){
    
    fromlen = sizeof(cliaddr);
    
    if (addr == NULL){
        fprintf(stderr, "server: falha ao fazer bind para o socket\n");
        return 2;
    }
    
    if ((recvfrom(sock, (void *)buf, sizeof(buf), 0,
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


int verifica_execucao(int argc){
    if (argc != 3){
        perror("Execução correta: ./chat-client <ip> [<porta>]\n");
        return EXIT_FAILURE;
    }
    return 1;
}