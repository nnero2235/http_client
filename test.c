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

void test_split_string(){
    char *str = "nnero\r\nfetch news\r\nhttp:www.173.com\r\n\r\n{jjs:nnero,sdsd:ssss}\nssad\rsss";
    char **strs = split_str_by_string(str,"\r\n");
    int index = 0;
    while(strs[index]) {
        printf("%s\n", strs[index]);
        index++;
    }
}

void test_split_n_char(){
    char *str = "nnero:dsasda 9:9:9 adasd:sss";
    char **strs = split_n_str_by_char(str,':',2);
    int index = 0;
    while(strs[index]) {
        printf("%s\n", strs[index]);
        index++;
    }
}

void test_new_call(){
    struct http_request *request = create_request(GET,"http://www.163.com/");
    Response response = new_call(request);
    printf("%d\n",response->state_code);
    printf("%d\n",response->content_length);
    printf("%s\n",response->entity);
    struct response_header *temp = response->response_headers;
    while(temp){
        printf("%s--->%s\n",temp->name,temp->value);
        temp = temp->next;
    }
}

void test_multi_call(){
    char ch;
    while(getchar()){
        Request request = create_request(GET,"http://www.163.com/");
        Response response = new_call(request);
    }
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
    printf("%d\n",response_1->state_code);
    printf("%d\n",response_1->content_length);
    printf("%s\n",response_1->entity);
}

int main(){
//    test_split_n_char();
//    test_multi_call();
    test_new_call();
//    test_make_http_request();
//    test_split();
//    test_split_string();
//    test_parse_response();
    return 0;
}



