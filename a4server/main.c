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
    int port;
    if(argc<3){
        fprintf(stderr,"usage: %s ip port\n",argv[0]);
        exit(1);
    }
    else if(argc==3){
        port=8000;
    }
    else{
        port=atoi(argv[4]);
    }
    //create socket
    int cfd = socket(AF_INET,SOCK_DGRAM,0);
    if(cfd==-1){
        perror("socket error");
        exit(1);
    }
    printf("sockfd: %d\n",cfd);
    //struct for server:  sockaddr_in
    struct sockaddr_in serv;
    socklen_t addrlen= sizeof(serv);
    serv.sin_family = AF_INET;//ipv4
    serv.sin_addr.s_addr= inet_addr(argv[2]);//ip address
    serv.sin_port = htons(port);

    //bind
    if(bind(cfd,(struct sockaddr*)&serv,sizeof(serv))<0){
        perror("bind fail");
        exit(1);
    }
    //receive data
    char buf[N]="";
    int loop=1;
    struct sockaddr_in clientAddr;
    while(loop==1)
    {
        if(recvfrom(cfd,buf,sizeof(buf),0,(struct sockaddr*)&clientAddr,&addrlen)==-1)
        {
            perror("recvform fail!");
            exit(1);
        }
        printf("from proxy[%s-%d]:%s\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port),buf);
        strcat(buf,"*_*");

        if(sendto(cfd,buf, sizeof(buf),0,(struct sockaddr*)&clientAddr,addrlen)==-1){
            perror("sendto fail");
            exit(1);
        }
    }

    close(cfd);

    return 0;
}