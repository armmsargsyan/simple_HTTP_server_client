#ifndef PARSER_H
#define PARSER_H

#include "protocol.h"


int read_line(int clientfd, char *buffer, int max_len);

int parse_request_line(char *line, HttpRequest *req);

int parse_request(int clientfd, HttpRequest *req);



#endif
 