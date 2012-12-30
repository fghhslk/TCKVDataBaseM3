/********************************************************************/
/* Copyright (C) SSE-USTC, 2012                                     */
/*                                                                  */
/*  FILE NAME             : client.h                                */
/*  PRINCIPAL AUTHOR      : SLK                                     */
/*  SUBSYSTEM NAME        : client                                  */
/*  MODULE NAME           : client                                  */
/*  LANGUAGE              : C                                       */
/*  TARGET ENVIRONMENT    : Any                                     */
/*  DATE OF FIRST RELEASE : 2012/11/22                              */
/*  DESCRIPTION           : Reply Service User(Client)              */
/********************************************************************/

/*
* Revision log:
*
* Created by SLK,2012/11/22
*
*/
#include"net.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define PORT 5002
#define IP_ADDR "127.0.0.1"
#define MAX_BUF_LEN 1024

int main()
{
    char szBuf[MAX_BUF_LEN] = "\0";
    int size = MAX_BUF_LEN;
    char szMsg[MAX_BUF_LEN] = "hello\0";
    tServiceHandler h = -1;
    h = OpenRemoteService(IP_ADDR,PORT);
    SendData(h,szMsg,strlen(szMsg));
    printf("client send:%s\n",szMsg);
    RecvData(h,szBuf,&size);
    printf("client recv:%s\n",szBuf);
    CloseRemoteService(h);
    return 0;
}