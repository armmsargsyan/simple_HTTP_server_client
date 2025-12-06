#ifndef MY_PROTOCOL
#define MY_PROTOCOL

#define MAX_METHOD 12
#define MAX_PATH   256
#define MAX_VERSION 32
#define MAX_STATUS_MSG 64
#define MAX_BODY 4096
#define MAX_HEADERS 512

typedef struct {
    char headers[MAX_HEADERS];
    char method[MAX_METHOD];  
    char path[MAX_PATH];
    char version[MAX_VERSION];
    int body_size;
    char body[MAX_BODY];
    }HttpRequest;

typedef struct {
    char headers[MAX_HEADERS];
    char version[MAX_VERSION];
    int status;
    char msg_status[MAX_STATUS_MSG];
    int body_size;
    char body[MAX_BODY];
}HttpResponse;
    
 
    
    

#endif