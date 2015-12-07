//
// Author: NNERO 
//
// TIME: 15/12/6 下午11:15
//
// DESC: 测试用例
//
#include <stdio.h>
#include <stdlib.h>
#include "http.h"

struct request_param * create_param(char *name,char *value){
    struct request_param *param = malloc(sizeof(struct request_param));
    param->name = name;
    param->value = value;
    return param;
}

void test_make_http_request(){
    struct http_request request;
    struct request_param *params;
    request.type = GET;
    request.request_line = "index.html";

    params = create_param("pname","NNERO");
    request.params = params;
    params->next = create_param("age","13");
    params->next->next = create_param("sex","boy");
    params->next->next->next = NULL;

    make_http_request(&request);
}

int main(){
    test_make_http_request();
    return 0;
}



