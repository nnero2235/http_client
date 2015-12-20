#include <stdio.h>
#include "http.h"
#include "util.h"
#include "d_error.h"
#include "client.h"
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>

#define REQUEST_STR_SIZE 2048

//释放整个request
static void free_request(Request request){
    while(request->params){
        free(request->params);
        request->params = request->params->next;
    }
    while(request->headers){
        free(request->headers);
        request->headers = request->headers->next;
    }
//    if(request->host) free(request->host);
    if(request->entity) free(request->entity);
    if(request->path) free(request->path);
    free(request);
}


static size_t build_http_entity(char *str,char *entity){
    //TODO:
    return 0;
}

//分离实体和响应头,index=0为headers,index=1为entity
static char ** split_entity_and_headers(char *response){
    char **temp = malloc(2 * sizeof(char*));
    for(int i=0;i<strlen(response);i++){
        if(response[i] == '\n' && response[i+1] == '\r' && response[i+2] == '\n'){
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
    return NULL;
}

//params
static size_t build_params(char *str,size_t str_index,struct request_param *params){
    if(params) str[str_index++] = PARAM_SPLIT;
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

    str_index = strncpy_index(str,str_index,"GET ",4);

//    if(strcmp(request->path,"/") != 0)
    str_index = strncpy_index(str,str_index,request->path,strlen(request->path));

    str_index = build_params(str,str_index,request->params);

    str[str_index++] = ' ';

    str_index = strncpy_index(str,str_index,HTTP_VERSION,strlen(HTTP_VERSION));

    str[str_index++] = '\n';
    return str_index;
}

//request headers
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
    str[str_index++] = '\r';
    str[str_index++] = '\n';
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
//response headers
void parse_response_headers(struct http_response *response,char *name,char *value){
    struct response_header *header = malloc(sizeof(struct response_header));
    struct response_header *header_point = response->response_headers;
    header->name = name;
    header->value = value;
    header->next = NULL;

    if(header_point == NULL){
        response->response_headers = header;
        return;
    }

    for(;;response->response_headers = response->response_headers->next){
        if(response->response_headers->next == NULL){
            response->response_headers->next = header;
            response->response_headers = header_point;
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
//    str_index += build_http_entity(&str[str_index],request->entity);
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
    char **strs = split_str_by_string(headers,"\r\n");
    char **strs_temp = NULL;
    int index = 1; //从第二行开始解析

    char *state_code = malloc(4 * sizeof(char));
    state_code[0] = (*strs)[9];
    state_code[1] = (*strs)[10];
    state_code[2] = (*strs)[11];
    state_code[3] = '\0';
    http_rep->state_code = atoi(state_code);
    http_rep->response_headers = NULL;

    while(strs[index]){
        strs_temp = split_n_str_by_char(strs[index],':',1);

        if(strs_temp != NULL){
            if(strcmp(response_header_names[RESPONSE_CONTENT_LENGTH],*strs_temp) == 0){
                http_rep->content_length = atoi(*(strs_temp+1));
            } else {
                parse_response_headers(http_rep,strs_temp[0],strs_temp[1]);
            }
        }
        index++;
    }
    http_rep->entity = entity;
    return http_rep;
}

int get_client_socket(char *host, int port){
    int sock_id;
    size_t len = strlen(host)+1;
    char *host_temp = malloc(len * sizeof(char));
    int max_try = 5;
    char ip[32];
    struct sockaddr_in server_addr;
    struct hostent *hp = NULL;

    sock_id = socket(AF_INET,SOCK_STREAM,0);
    if(sock_id == -1){
        p_err("socket");
        return -1;
    }
    strncpy(host_temp,host, len);
    printf("%s\n",host_temp);
    hp = gethostbyname(host);
    while(max_try && hp == NULL){ //DNS解析失败
        p_err_normal("dns error:try again!\n");
        strncpy(host,host_temp, len);
        hp = gethostbyname(host);
        max_try--;
    }
    if(hp == NULL){
        p_err_normal("dns error:can't parse host\n");
        return -1;
    }
    bzero(&server_addr, sizeof(struct sockaddr_in));
    bcopy(hp->h_addr,&server_addr.sin_addr,hp->h_length);
    server_addr.sin_port = htons(port);
    server_addr.sin_family = AF_INET;

    printf("Trying connect to %s:%d\n",inet_ntop(hp->h_addrtype,hp->h_addr,ip, sizeof(ip)),port);

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
    request->headers = NULL;
    request->params = NULL;
    request->entity = NULL;
    add_header(request,request_header_names[REQUEST_DATE],get_current_time());
    add_header(request,request_header_names[REQUEST_ACCEPT],mime_type[HTML]);
    add_header(request,request_header_names[REQUEST_ACCEPT_CHARSET],char_set[UTF8]);
    add_header(request,request_header_names[REQUEST_ACCEPT_LANGUAGE],language[CHINESE]);
    add_header(request,request_header_names[REQUEST_CONNECTION],connection_state[CONNECT_ALIVE]);
    add_header(request,request_header_names[REQUEST_HOST],request->host);
    add_header(request,request_header_names[REQUEST_USER_AGENT],"Mozilla/5.0 (Macintosh; Intel Mac OS X 10_10_2) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/46.0.2490.86 Safari/537.36");
    add_header(request,request_header_names[REQUEST_ACCEPT_ENCODING],"gzip,deflate");
    free(*temp);
    return request;
}

char * get_entity(Response response){
    return response->entity;
}

Response new_call(Request request){
    int sock_id;
    char buf[BUFSIZ];
    sock_id = get_client_socket(request->host,request->port); //connect
    if(sock_id == -1) {
        p_err_normal("connect fail! sock_id = -1\n");
        exit(1);
    }
    char *msg = build_http_request(request);
    printf("%s\n",msg);
    ssize_t len = write(sock_id,msg,strlen(msg)); //char 是1字符
    if(len == -1){
        p_err("send");
        exit(1);
    }
//    printf("%zd\n",len);
    if ((len = read(sock_id,buf, sizeof(buf)-2)) != EOF){
        buf[len-1] = '\0';
        printf("%s\n",buf);
    }
    close(sock_id);
    free(msg);
    free_request(request);
    return parse_http_response(buf);
}




