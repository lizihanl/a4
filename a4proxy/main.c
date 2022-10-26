#include <stdio.h>        //printf
#include <stdlib.h>       //exit
#include <string.h>

#include <sys/socket.h>   //socket
#include <unistd.h>       //close
#include <arpa/inet.h>    //htons inet_addr
#include <netinet/in.h>   //sockaddr_in
#include <ctype.h>

#define N 1024

int main(int argc,char const *argv[]){
    int severport=8000;
    int proxyport=5000;
    char severip[50],proxyip[50];
    switch (argc) {
        case 5:
        {
            strcpy(severip,argv[4]);
            strcpy(proxyip,argv[2]);
            break;
        }
        case 7:
        {
            for(int i=1;i<6;i++){
                if((argv[i][1]=='i')||(argv[i][1]=='I')){
                    strcpy(proxyip,argv[i+1]);
                    if((argv[i+2][1]=='p')||(argv[i+2][1]=='P')){
                        proxyport=atoi(argv[i+3]);
                    }
                    else{
                        severport=atoi(argv[6]);
                    }
                }
                if((argv[i][1]=='o')||(argv[i][1]=='O')){
                    strcpy(severip,argv[i+1]);
                }
            }
            break;
        }
        case 9:
        {
            strcpy(severip,argv[6]);
            strcpy(proxyip,argv[2]);
            severport=atoi(argv[8]);
            proxyport=atoi(argv[4]);
            break;
        }
        default:
        {
            fprintf(stderr,"usage: %s ip port\n",argv[0]);
            exit(1);
        }
    }
    printf("s:%s-%d   p:%s-%d\n",severip,severport,proxyip,proxyport);


    //create socket
    int proxyfd = socket(AF_INET,SOCK_DGRAM,0);
    if(proxyfd==-1){
        perror("proxy socket error");
        exit(1);
    }

    printf("proxy_sockfd: %d\n",proxyfd);

    //struct for proxy: sockaddr_in
    struct sockaddr_in proxyserv;
    socklen_t Paddrlen= sizeof(proxyserv);
    proxyserv.sin_family = AF_INET;//ipv4
    proxyserv.sin_addr.s_addr= inet_addr(proxyip);//ip address
    proxyserv.sin_port = htons(proxyport);
    //struct for server: sockaddr_in
    struct sockaddr_in serv;
    socklen_t Saddrlen= sizeof(serv);
    serv.sin_family = AF_INET;//ipv4
    serv.sin_addr.s_addr= inet_addr(severip);//ip address
    serv.sin_port = htons(severport);

    //bind
    if(bind(proxyfd,(struct sockaddr*)&proxyserv,sizeof(proxyserv))<0){
        perror("proxy bind fail");
        exit(1);
    }
    //receive data
    char sendbuf[N]="";
    char recvbuf[N]="";
    int loop=1;
    struct sockaddr_in clientAddr;
    struct sockaddr_in sendAddr;
    while(loop==1)
    {
        if(recvfrom(proxyfd,recvbuf,sizeof(recvbuf),0,(struct sockaddr*)&sendAddr,&Paddrlen)==-1)
        {
            perror("proxy from client recvform fail!");
            exit(1);
        }
        strcpy(sendbuf,recvbuf);//use after the "%"

        if(sendAddr.sin_port!=serv.sin_port)       //receive from server or client
        {
            clientAddr=sendAddr;
            printf("from client[%s-%d]:%s\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port),sendbuf);
            if(sendto(proxyfd,sendbuf, sizeof(sendbuf),0,(struct sockaddr*)&serv,Saddrlen)==-1) {
                perror("proxy to server sendto fail");
                exit(1);
            }
        }
        else
        {
            printf("from server[%s-%d]:%s\n", inet_ntoa(sendAddr.sin_addr), ntohs(sendAddr.sin_port),recvbuf);
            if(sendto(proxyfd,recvbuf, sizeof(recvbuf),0,(struct sockaddr*)&clientAddr,Saddrlen)==-1){
                perror("proxy to client sendto fail");
                exit(1);
            }
        }
    }

    close(proxyfd);
    return 0;
}