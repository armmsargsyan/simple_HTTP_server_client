#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "protocol.h"
#include <pthread.h>
#define PORT 4363

int read_line(int clientfd, char * buffer, int max_len){
    int i = 0;
    char p;
    int r;

    while(i < max_len -1){
            r = recv(clientfd, &p, 1, 0);
            if(r <= 0){
                return -1;
            }
            if(p != '\n'){
                buffer[i] = p;
                i++;
            }else break;
        }
        if(i > 0 && buffer[i-1] == '\r'){
            i--;
        }
        buffer[i] = '\0';
    return i;
}

int parse_request_line(char *line, HttpRequest *req){

    for(int i = 0; line[i]; ++i){
            if( line[i] == '\n'){
                line[i] = '\0';
                break;
            }
    }
    char *token;
    const char *delimeter = " ";
    token = strtok(line, delimeter);
    if(token == NULL) return -1;

    strncpy(req->method, token, sizeof(req->method) -1);
    req->method[sizeof(req->method) - 1] = '\0';



    token = strtok(NULL, delimeter);
    if(token == NULL) return -2;
    strncpy(req->path, token, sizeof(req->path) - 1);
    req->path[sizeof(req->path) - 1] = '\0';
    

    token = strtok(NULL, delimeter);
    if(token == NULL) return -3;

    strncpy(req->version, token, sizeof(req->version) - 1);
    req->version[sizeof(req->version) - 1] = '\0';

    if(strtok(NULL, delimeter) != NULL) return -1;
    
    return 0;

}


int parse_request(int clientfd, HttpRequest *obj){
    char buff[1024];
    int n;

    n = read_line(clientfd, buff, sizeof(buff));
    if (n <= 0) {
        return -1;
    }

    if(parse_request_line(buff, obj) != 0) {
        return -2;
    }

    obj->body_size = 0;

    while(1){
        n = read_line(clientfd, buff, sizeof(buff));
        if(n < 0){
            return -3;
        }
        if (buff[0] == '\0') {
            break;
        }

        if (strncmp(buff, "Body-Size:", 10) == 0) {
            char *val = buff + 10;
            while(*val == ' ' || *val == '\t')
            val++;

            obj->body_size = atoi(val);
        }
    }

    if(obj->body_size >= MAX_BODY) {
        return -5;
    }

    if(obj->body_size > 0) { 

        int total = 0;
        int r;

        while(total < obj->body_size) {
            r = recv(clientfd, obj->body + total, obj->body_size - total, 0);

            if(r <= 0){
                return -4;
            }
            total += r;
        }
        
        obj->body[total] = '\0';
    }
    

    return 0;
}
