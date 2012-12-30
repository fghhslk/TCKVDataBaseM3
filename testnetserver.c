/********************************************************************/
/* Copyright (C) SSE-USTC, 2012                                     */
/*                                                                  */
/*  FILE NAME             : server.c                                */
/*  PRINCIPAL AUTHOR      : SLK                                     */
/*  SUBSYSTEM NAME        : server                                  */
/*  MODULE NAME           : server                                  */
/*  LANGUAGE              : C                                       */
/*  TARGET ENVIRONMENT    : Any                                     */
/*  DATE OF FIRST RELEASE : 2012/11/22                              */
/*  DESCRIPTION           : Reply Service Provider                  */
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
    char szReplyMsg[MAX_BUF_LEN] = "hi\0";
    tServiceHandler h = -1;
    InitializeNetService(IP_ADDR,PORT);
    while(1)
    {
        h = ServiceStart();
        RecvData(h, szBuf, &size);
        printf("server recv:%s\n", szBuf);
        SendData(h, szReplyMsg, strlen(szReplyMsg));
        printf("server send:%s\n", szReplyMsg);
        ServiceStop(h);
    }
    ShutdownNetService();
    return 0;
}

