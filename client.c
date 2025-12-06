#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 4363

int main(){
    char method[32];
    char path[256];
    char body[2048];

    printf("Enter METHOD (GET / POST / ECHO) \n");
    scanf("%15s", method);

    printf("Enter PATH (example: /index.txt) \n");
    scanf("%255s", path);

    int body_size = 0;
    if(strcmp(method, "GET") != 0) {
        printf("Enter BODY text \n");
        getchar(); 
        fgets(body,sizeof(body), stdin);

        body[strcspn(body, "\n")] = 0;
        body_size = strlen(body);

    }else {
        body[0] = '\0';
        body_size = 0;
    }


    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server; 
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);


    if(connect(sockfd, (struct sockaddr*)&server, sizeof(server)) < 0) {
        perror("connect failed\n");
        exit(1);
    }
    char buff[512];
    sprintf(buff, "%s %s CHLP/1.0\n""Body-Size: %d\n""\n", method, path, body_size);
    
    send(sockfd, buff, strlen(buff), 0);
    if(body_size > 0) {
        send(sockfd, body, body_size, 0);

    }

    printf("server responce\n");
    char buf_responce[2048];
    int r; 
    
    while((r = recv(sockfd, buf_responce, sizeof(buf_responce) - 1, 0)) > 0){
        buf_responce[r] = '\0';
        printf("%s \n", buf_responce);

    }
    
    close(sockfd);
    return 0;

}