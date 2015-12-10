#include <stdio.h>
#include "http.h"
#include "util.h"
#include "d_error.h"
#include "client.h"
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>

#define REQUEST_STR_SIZE 2048

//分离实体和响应头,index=0为headers,index=1为entity
static char ** split_entity_and_headers(char *response){
    char **temp = malloc(2 * sizeof(char*));
    for(int i=0;i<strlen(response);i++){
        if(response[i] == '\n' && response[i+1] == '\n'){
            char *entity = malloc(strlen(&response[i+2])* sizeof(char));
            char *headers = malloc((i+1)* sizeof(char));
            strncpy(headers,response,i);
            strncpy(entity,&response[i+2],strlen(&response[i+1]));
            headers[i] = '\0';
            entity[strlen(&response[i+1])] = '\0';
            temp[0] = headers;
            temp[1] = entity;
            return temp;
        }
    }
}

//params
static size_t build_params(char *str,size_t str_index,struct request_param *params){
    str[str_index++] = PARAM_SPLIT;
    while(params){
        //解析name
        str_index = strncpy_index(str,str_index,params->name,strlen(params->name));
        //添加=号
        str[str_index++] = PARAM_EQUAL;
        //解析value
        str_index = strncpy_index(str,str_index,params->value,strlen(params->value));
        //调整指针
        params = params->next;
        if(params) str[str_index++] = PARAM_AND;

    }
    return str_index;
}
//GET
static size_t build_request_line(char *str,struct http_request *request){
    size_t str_index=0;

    str_index = strncpy_index(str,str_index,"GET",3);
    str_index = strncpy_index(str,str_index,request->path,strlen(request->path));

    str_index = build_params(str,str_index,request->params);

    str[str_index++] = ' ';

    str_index = strncpy_index(str,str_index,HTTP_VERSION,strlen(HTTP_VERSION));

    str[str_index++] = '\n';
    return str_index;
}
//headers
static size_t build_http_headers(char *str,struct request_header *headers){
    size_t str_index = 0;
    size_t len = 0;
    char *header_str;

    while(headers){
        //build header
        header_str = malloc(strlen(headers->name)+1+strlen(headers->value));
        len = strlen(headers->name);
        strncpy(header_str,headers->name,len);
        header_str[len++] = ':';
        strncpy(&header_str[len],headers->value,strlen(headers->value));
        header_str[len + strlen(headers->value)] = '\0';//不然strlen无法计算
        //add to str
        str_index = strncpy_index(str,str_index,header_str,strlen(header_str));
        str[str_index++] = '\n';
        headers = headers->next;
    }

    return str_index;
}

void add_param(struct http_request *request,char *name,char *value){
    struct request_param *param = malloc(sizeof(struct request_param));
    struct request_param *header_point = request->params;
    param->name = name;
    param->value = value;
    param->next = NULL;

    if(header_point == NULL){
        request->params = param;
        return;
    }

    for(;;request->params = request->params->next){
        if(request->params->next == NULL){
            request->params->next = param;
            request->params = header_point;
            return;
        }
    }
}

void add_header(struct http_request *request,char *name,char *value){
    struct request_header *header = malloc(sizeof(struct request_header));
    struct request_header *header_point = request->headers;
    header->name = name;
    header->value = value;
    header->next = NULL;

    if(header_point == NULL){
        request->headers = header;
        return;
    }

    for(;;request->headers = request->headers->next){
        if(request->headers->next == NULL){
            request->headers->next = header;
            request->headers = header_point;
            return;
        }
    }
}

char * build_http_request(struct http_request *request){
    char str[REQUEST_STR_SIZE]; /*request str:就是一个用\n分割的 大字符串*/
    char *real_str;
    size_t str_index = 0;

    if(request->type == GET){
        str_index = build_request_line(str,request);
    } else if(request->type == POST){
        //TODO: support POST
    }
    str_index += build_http_headers(&str[str_index],request->headers);
    str[str_index] = '\0';
    //str_index is not used anymore.  this is just reuse
    str_index = strlen(str);
    real_str = malloc(str_index* sizeof(char));
    strncpy(real_str,str,str_index);
    return real_str;
}

struct http_response * parse_http_response(char *response){
    struct http_response *http_rep = malloc(sizeof(struct http_response));
    char **temp = split_entity_and_headers(response);
    char *headers = temp[0];
    char *entity = temp[1];
    char **strs = split_str_by_char(headers,'\n');
    char **strs_temp;
    int index = 1; //从第二行开始解析

    char *state_code = malloc(4 * sizeof(char));
    state_code[0] = (*strs)[9];
    state_code[1] = (*strs)[10];
    state_code[2] = (*strs)[11];
    state_code[3] = '\0';
    http_rep->state_code = atoi(state_code);

    while(strs[index]){
        strs_temp = split_str_by_char(strs[index],':');

        if(strcmp(response_header_names[RESPONSE_SERVER],*strs_temp) == 0){
            http_rep->server = *(strs_temp+1);
        } else if(strcmp(response_header_names[RESPONSE_DATE],*strs_temp) == 0){
            http_rep->date = *(strs_temp+1);
        } else if(strcmp(response_header_names[RESPONSE_CONTENT_TYPE],*strs_temp) == 0){
            http_rep->content_type = *(strs_temp+1);
        } else if(strcmp(response_header_names[RESPONSE_CONTENT_LENGTH],*strs_temp) == 0){
            http_rep->content_length = atoi(*(strs_temp+1));
        }

        index++;
    }
    http_rep->entity = entity;
    return http_rep;
}

int get_client_socket(char *host,int port){
    int sock_id;
    struct sockaddr_in server_addr;
    struct hostent *hp;

    sock_id = socket(AF_INET,SOCK_STREAM,0);
    if(sock_id == -1){
        p_err("socket");
        return -1;
    }

    hp = gethostbyname(host);
    bzero(&server_addr, sizeof(struct sockaddr_in));
    bcopy(hp->h_addr,&server_addr.sin_addr,hp->h_length);
    server_addr.sin_port = htons(port);
    server_addr.sin_family = AF_INET;

    printf("Trying connect to %s:%d\n",host,port);

    if(connect(sock_id,(struct sockaddr *)&server_addr, sizeof(server_addr)) != 0) {
        p_err("connect");
        return -1;
    }
    printf("connect server success.\n");
    return sock_id;
}

Request create_request(Request_type type,char *url){
    Request request = malloc(sizeof(struct http_request));
    char **temp = check_url(url);
    printf("%s\n",temp[0]);
    printf("%s\n",temp[1]);
    request->type = type;
    request->host = temp[0];
    request->path = temp[1];
    request->port = get_port(request->host);
    add_header(request,request_header_names[REQUEST_DATE],get_current_time());
    add_header(request,request_header_names[REQUEST_ACCEPT],mime_type[JSON]);
    add_header(request,request_header_names[REQUEST_ACCEPT_LANGUAGE],language[CHINESE]);
    add_header(request,request_header_names[REQUEST_CONNECTION],connection_state[CONNECT_ALIVE]);
    add_header(request,request_header_names[REQUEST_HOST],"NNERO.mac");
    add_header(request,request_header_names[REQUEST_USER_AGENT],"CLIENT 1.0.0");
    add_header(request,request_header_names[REQUEST_ACCEPT_ENCODING],"gzip");

    return request;
}




