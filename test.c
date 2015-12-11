//
// Author: NNERO 
//
// TIME: 15/12/6 下午11:15
//
// DESC: 测试用例
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "http.h"
#include "client.h"
#include "util.h"

void test_new_call(){
    struct http_request *request = create_request(GET,"http://www.csdn.com/");
    Response response = new_call(request);
}

void test_make_http_request(){
    struct http_request *request = create_request(GET,"http://www.gzip.com:9090/api/v3/index.jsp");

    add_param(request,"pname","NNERO");
    add_param(request,"age","13");
    add_param(request,"sex","boy");

    char *str = build_http_request(request);
    printf("%s\n",str);
}

void test_split(){
    char *response = "HTTP/1.1 200 OK\n"
            "Server:Apache Tomcat/5.0.12\n"
            "Date:Mon,6Oct2003 13:13:33 GMT\n"
            "Content-Type:text/html\n"
            "Content-Length:112\n";
    char **temp = split_str_by_char(response,'\n');
    printf("%s\n",temp[0]);
    printf("%s\n",temp[1]);
    printf("%s\n",temp[2]);
    printf("%s\n",temp[3]);
}

void test_parse_response(){
    char *response = "HTTP/1.1 200 OK\n"
            "Server:Apache Tomcat/5.0.12\n"
            "Date:Mon,6Oct2003 13:13:33 GMT\n"
            "Content-Type:text/html\n"
            "Content-Length:112\n"
            "\n"
            "{\"url\":\"www.baidu.com\",\"id\":\"1001\"}";
    struct http_response *response_1 = parse_http_response(response);
    printf("%s\n",response_1->server);
    printf("%s\n",response_1->date);
    printf("%d\n",response_1->state_code);
    printf("%s\n",response_1->content_type);
    printf("%d\n",response_1->content_length);
    printf("%s\n",response_1->entity);
}

int main(){
    test_new_call();
//    test_make_http_request();
//    test_split();
//    test_parse_response();
    return 0;
}



