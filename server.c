#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "protocol.h"
#include "parser.h"
#include <pthread.h>
#define PORT 4363

void handle_get(int clientfd, HttpRequest *obj);
void handle_post(int clientfd, HttpRequest *obj);
void handle_echo(int clientfd, HttpRequest *obj);
void error_request(int clientfd);

void handle_request(int clientfd, HttpRequest *obj){
    if(strcmp(obj->method, "GET") == 0) {
        handle_get(clientfd, obj);
    }
    else if (strcmp(obj->method, "POST") == 0)
    {
        handle_post(clientfd, obj);
    }
    else if (strcmp (obj->method, "ECHO") == 0)
    {
        handle_echo(clientfd, obj);
    }
    else{
        error_request(clientfd);
    }
    
    
}

void handle_get(int clientfd, HttpRequest *obj) {
        char path[512];
        snprintf(path, sizeof(path), "local%s", obj->path);

        FILE *f = fopen(path, "r");
        if(!f){
            const char *msg= "CHLP/1.0 404 Not Found\n""Body-Size: 0\n""\n";
            send(clientfd, msg, strlen(msg), 0);
            return;

        }
        fseek(f, 0, SEEK_END);
        int size = ftell(f);
        fseek(f, 0, SEEK_SET);

        char *file_content = malloc(size + 1);
        fread(file_content, 1, size, f);
        file_content[size] = '\0';
        fclose(f);
         

        char header[256];
        sprintf(header, "CHLP/1.0 200 OK\n""Body-Size: %d\n""\n", size);
        send(clientfd, header, strlen(header), 0);
        send(clientfd, file_content, size, 0);

        free(file_content);

}


void handle_post(int clientfd, HttpRequest *obj) {
        char path[512];
        snprintf(path, sizeof(path), "local%s", obj->path);
        FILE *f = fopen(path, "a+");
        if(!f){
            const char *msg = "CHLP/1.0 500 Internal Server Error\n""Body-Size: 0\n""\n";
            send(clientfd, msg, strlen(msg), 0);
            return;
        }

        if(obj->body_size > 0){
            fwrite(obj->body, 1, obj->body_size, f);
        }
        fclose(f);

        const char *resp = "CHLP/1.0 200 OK\n""Body-Size: 0\n""\n";
        send(clientfd, resp, strlen(resp), 0);
}

void handle_echo(int clientfd, HttpRequest *obj) {
    char buff[256];
    sprintf(buff, "CHLP/1.0 200 OK\n""Body-Size: %d\n""\n", obj->body_size);
    send(clientfd, buff, strlen(buff), 0);

    if(obj->body_size > 0){
        send(clientfd, obj->body, obj->body_size, 0);
    }
}

void error_request(int clientfd) {
    const char *msg ="CHLP/1.0 400 Bad Request\n""Body-Size: 0\n""\n";

    send(clientfd, msg, strlen(msg), 0);
}


void *handle_client(void *n){
    int clientfd = *((int *)n);
    free(n);

    HttpRequest obj;
    if(parse_request(clientfd, &obj) != 0) {
        error_request(clientfd);
        close(clientfd);
        return NULL;
    }
    handle_request(clientfd, &obj);

    close(clientfd);
    return NULL;

}



int main(){
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = INADDR_ANY;
    if(bind(sockfd, (struct sockaddr*)&server, sizeof(server)) == -1){
        perror("bind failed\n");
        exit(1);
    }

   if(listen(sockfd, SOMAXCONN) < 0){
        perror("listen failed\n");
        exit(1);
   }

   while(1)
   {
    struct sockaddr_in client;
    socklen_t len = sizeof(client);
    int clientfd = accept(sockfd,(struct sockaddr*)&client, &len);
        if(clientfd == -1){
            perror("accept failed\n");
            continue;
        }

        int *pclient = malloc(sizeof(int));
        *pclient = clientfd;

        pthread_t t;
        pthread_create(&t, NULL, handle_client, pclient);
        pthread_detach(t);


   }










return 0;
}
