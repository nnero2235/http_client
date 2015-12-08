#include <stdio.h>
#include "http.h"
#include "util.h"
#include "d_error.h"
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>

#define REQUEST_STR_SIZE 2048

//分离实体和响应头,会截断response
static char * get_entity(char *response){
    for(int i=0;i<strlen(response);i++){
        if(response[i] == '\n' && response[i+1] == '\n'){
            char *entity = malloc(strlen(&response[i+1])* sizeof(char));
            strncpy(entity,&response[i+1],strlen(&response[i+1]));
            response[i] = '\0';
            return entity;
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

    str_index = strncpy_index(str,str_index,"GET/",4);
    str_index = strncpy_index(str,str_index,request->request_line,strlen(request->request_line));

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
        str_index += strncpy_index(str,str_index,header_str,strlen(header_str));
        str[str_index++] = '\n';
        headers = headers->next;
    }

    return str_index;
}

void add_param(struct request_param *params,char *name,char *value){
    struct request_param *param = malloc(sizeof(struct request_param));
    param->name = name;
    param->value = value;
    param->next = NULL;

    for(;;params = params->next){
        if(params == NULL){
            params = param;
            return;
        }
    }
}

void add_header(struct request_header *headers,char *name,char *value){
    struct request_header *header = malloc(sizeof(struct request_header));
    header->name = name;
    header->value = value;
    header->next = NULL;

    for(;;headers = headers->next){
        if(headers == NULL){
            headers = header;
            return;
        }
    }
}

char * build_http_request(struct http_request *request){
    char str[REQUEST_STR_SIZE]; /*request str:就是一个用\n分割的 大字符串*/
    size_t str_index = 0;

    if(request->type == GET){
        str_index = build_request_line(str,request);
    } else if(request->type == POST){
        //TODO: support POST
    }
    str[str_index++] = '\n';
    str_index += build_http_headers(&str[str_index],request->headers);
    str[str_index] = '\0';
    printf("%s\n",str);

    return str;
}

struct http_response * parse_http_response(char *response){
    struct http_response *http_rep = malloc(sizeof(struct http_response));
    char *entity = get_entity(response);
    char **strs = split_str_by_char(response,'\n');
    char **strs_temp;
    int index = 1; //从第二行开始解析

    char *state_code = malloc(4 * sizeof(char));
    state_code[0] = (*strs)[9];
    state_code[1] = (*strs)[10];
    state_code[2] = (*strs)[11];
    state_code[3] = '\0';
    http_rep->state_code = atoi(state_code);

    while(*(strs+index)){
        strs_temp = split_str_by_char(*(strs+index),':');

        if(strcmp(*response_header_names[RESPONSE_SERVER],*strs_temp)){
            http_rep->server = *(strs_temp+1);
        } else if(strcmp(*response_header_names[RESPONSE_DATE],*strs_temp)){
            http_rep->date = *(strs_temp+1);
        } else if(strcmp(*response_header_names[RESPONSE_CONTENT_TYPE],*strs_temp)){
            http_rep->content_type = *(strs_temp+1);
        } else if(strcmp(*response_header_names[RESPONSE_CONTENT_LENGTH],*strs_temp)){
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




