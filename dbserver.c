/********************************************************************/
/* Copyright (C) MC2Lab-USTC, 2012                                  */
/*                                                                  */
/*  FILE NAME             : dbserver.c                              */
/*  PRINCIPAL AUTHOR      : SLK                                     */
/*  SUBSYSTEM NAME        : network                                 */
/*  MODULE NAME           : dbserver                                */
/*  LANGUAGE              : C                                       */
/*  TARGET ENVIRONMENT    : Linux                                   */
/*  DATE OF FIRST RELEASE : 2012/12/14                              */
/*  DESCRIPTION           : Impement of Socket Server Engine        */
/*  to handle clients reqs.                                         */
/********************************************************************/

/*
* Revision log:
*
* Created by SLK,2012/12/14
* Support pthread,by SLK,2012/12/19
*/

#include "dbapi.h"
#include "net.h"
#include "protocol.h"
#include "event.h"
#include "queue.h"
#include "cdmanager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>
#include <time.h>

#define PORT            5002
#define MAX_BUF_LEN     1024
#define MAX_TASK_NUM    2

#define debug printf

pthread_t threadID[MAX_TASK_NUM];
tEvent events[MAX_TASK_NUM];
Queue taskQueue[MAX_TASK_NUM];
typedef struct TaskNode
{
    QueueNode next;
    tServiceHandler req;
    char buf[MAX_BUF_LEN];
    int bufSize;
}tTaskNode;

/* start manage response thread */
#define InitResponseThread()                                    \
        int i;                                                  \
        for (i = 0; i < MAX_TASK_NUM; i++)                      \
        {                                                       \
            EventInit(&events[i], 0);                           \
            QueueCreate(&taskQueue[i]);                         \
            int tmpTaskNum = i;                                 \
            if (pthread_create(&threadID[i], NULL,              \
                (void*)ManageResponse, (void*)tmpTaskNum))      \
            {                                                   \
                fprintf(stderr, "pthread_create Error,%s:%d\n", \
                    __FILE__, __LINE__);                        \
                exit(-1);                                       \
            }                                                   \
        }

#define DisposeResponseThread()                                 \
        for (i = 0; i < MAX_TASK_NUM; i++)                      \
        {                                                       \
            EventShut(&events[i]);                              \
            QueueDelete(&taskQueue[i]);                         \
        }

int ManageResponse(int taskNum);
int ServerResponse(tServiceHandler h, char *buf, int bufSize);

int main(int argc, char* argv[])
{
    printf("***************************************************************************\n");
    printf("****  Vampire KV Database system Server Engine Start!    --version 3.0 ****\n");
    printf("***************************************************************************\n");
    
    tServiceHandler req = -1;
    
    InitResponseThread();
    InitCDManager();
    int port = -1;
    //printf("port:");
    scanf("%d", &port);
    InitializeNetService(port);
    while (1)
    {
        /* return the client fd who have real data req */
        req = ServiceStart();
        tTaskNode *pNode = (tTaskNode*) malloc(sizeof(tTaskNode));
        pNode->bufSize = MAX_BUF_LEN;
        if (!RecvData(req, pNode->buf, &(pNode->bufSize)))
        {
            ServiceStop(req);
            continue;
        }
        pNode->req = req;
        srand((unsigned)time(0));
        i = random() % MAX_TASK_NUM;
        debug("request: %d send to task: %d\n", req, i);
        QueueIn(&taskQueue[i], (QueueNode*)pNode);
        SentEvent(&events[i]);
    }    
    ShutdownNetService();
    ShutCDManager();
    DisposeResponseThread();
    
    return 0;
}
int ErrorResponse(tServiceHandler h, char *errInfo)
{
    char buf[MAX_BUF_LEN] = "\0";
    int bufSize = MAX_BUF_LEN;
    FormatData1(buf, &bufSize, ERROR_RSP, errInfo, strlen(errInfo));
    SendData(h, buf, bufSize);
    return 0;
}

int ManageResponse(int taskNum)
{
    int i = taskNum;
    debug("task: %d starts\n", taskNum);
    /* Handle Requests */
    tServiceHandler h = -1;
    while (1)
    {
        WaitEvent(&events[i]);
        debug("task %d get a event\n", i);
        tTaskNode *pNode = NULL;
        QueueOut(&taskQueue[i], (QueueNode**)&pNode);
        h = pNode->req;
        if (ServerResponse(h, pNode->buf, pNode->bufSize) == -1)
        {
            continue;
        }
        free(pNode);
    }
    return 0;
}

int ServerResponse(tServiceHandler h, char *buf, int bufSize)
{
    int cmd = -1;
    int dataNum = -1;
    char data1[MAX_BUF_LEN] = "\0";
    char data2[MAX_BUF_LEN] = "\0";
    int data1Size = MAX_BUF_LEN;
    int data2Size = MAX_BUF_LEN;
    
    if (bufSize == 0)
        return -1;
        
    /* Parse request and get cmd */
    int ret = ParseData(buf, bufSize, &cmd, &dataNum, data1, &data1Size, data2, &data2Size);
    if (ret == -1)
    {
        ErrorResponse(h, "Data Format Error!\n");
        return -1;
    }
    
    /* Handle cmd request */
    if (cmd == OPEN_CMD && dataNum == 1)
    {
        debug("OPEN_CMD\n");
        tDatabase db = NULL;
        db = CreateDB(data1);
        AttachCD(h, db);
        bufSize = MAX_BUF_LEN;
        FormatData(buf, &bufSize, OPEN_RSP);
        SendData(h, buf, bufSize);
    }
    else if (cmd == CLOSE_CMD && dataNum == 0)
    {
        debug("CLOSE_CMD\n");
        tDatabase db = NULL;
        GetMdb(h, db);
        CloseDB(db);
        //printf("closed db.\n");
        bufSize = MAX_BUF_LEN;
        FormatData(buf, &bufSize, CLOSE_RSP);
        SendData(h, buf, bufSize);
        DettachCD(h);
        //printf("DettachCD.\n");
        //ServiceStop(h); 
    }
    else if (cmd == GET_CMD && dataNum == 1)
    {
        debug("GET_CMD\n");
        if (data1Size != sizeof(tKey))
        {
            fprintf(stderr, "Data Format Error,%s:%d\n", __FILE__, __LINE__);
            return -1;
        }
        tDatabase db = NULL;
        GetMdb(h, db);
        tKey key = *(tKey*)data1;
        tValue value;
        data2Size = MAX_BUF_LEN;
        value.len = data2Size;
        value.str = data2;
        ret = GetRecord(db, key, &value);
        if (ret == FAILURE)
        {
            ErrorResponse(h, "The key NOT FOUND!\n");
            return -1;
        }
        bufSize = MAX_BUF_LEN;
        FormatData2(buf, &bufSize, GET_RSP, (char*)&key, sizeof(tKey), value.str, value.len);
        SendData(h, buf, bufSize);
    }
    else if (cmd == SET_CMD && dataNum == 2)
    {
        debug("SET_CMD\n");
        if (data1Size != sizeof(tKey))
        {
            fprintf(stderr, "Data Format Error,%s:%d\n", __FILE__, __LINE__);
            return -1;
        }
        tKey key = *(tKey*)data1;
        tValue value;
        value.len = data2Size;
        value.str = data2;
        debug("SET_CMD:%d -> %s\n", *(tKey*)(buf + 12), (char*)(buf + 20));
        debug("SET_CMD:%d -> %s\n", key,value.str);
        tDatabase db = NULL;
        GetMdb(h, db); 
        SetRecord(db, key, value);
        bufSize = MAX_BUF_LEN;
        FormatData(buf, &bufSize, SET_RSP);
        SendData(h, buf, bufSize);
    }
    else if (cmd == DELETE_CMD && dataNum == 1)
    {
        debug("DELETE_CMD\n");
        if (data1Size != sizeof(tKey))
        {
            fprintf(stderr, "Data Format Error,%s:%d\n", __FILE__, __LINE__);
            return -1;
        }
        tKey key = *(tKey*)data1;
        tDatabase db = NULL;
        GetMdb(h, db); 
        ret = DeleteRecord(db, key);
        if (ret == FAILURE)
        {
            ErrorResponse(h, "No Value Relative To The Key!\n");
            return -1;
        }
        bufSize = MAX_BUF_LEN;
        FormatData(buf, &bufSize, DELETE_RSP);
        SendData(h, buf, bufSize);
    }
    else
    {
        ErrorResponse(h, "Unknow Request!\n");
        return -1;
    }
    return 0;
}