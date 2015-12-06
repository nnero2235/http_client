//
// Author: NNERO 
//
// TIME: 15/12/6 下午9:07
//
// DESC: 打印错误信息
//

#ifndef HTTP_CLIENT_D_ERROR_H_H
#define HTTP_CLIENT_D_ERROR_H_H
#include <stdio.h>
#include <stdlib.h>

#define p_err(msg) {perror(msg);}
#define p_err_normal(msg) {printf("ERROR: %s\n",msg);}

#endif //HTTP_CLIENT_D_ERROR_H_H
