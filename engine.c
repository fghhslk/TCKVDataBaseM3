/********************************************************************/
/* Copyright (C) MC2Lab-USTC, 2012                                  */
/*                                                                  */
/*  FILE NAME               :  engine.c                             */
/*  PRINCIPAL AUTHOR        :  SLK                                  */
/*  SUBSYSTEM NAME          :  main                                 */
/*  MODULE NAME             :  engine                               */
/*  LANGUAGE                :  C                                    */
/*  TARGET ENVIRONMENT      :  Linux                                */
/*  DATE OF FIRST RELEASE   :  2012/1/4                             */
/*  DESCRIPTION             :  Impement of Socket Server Engine     */
/*                             to handle clients requests.          */
/********************************************************************/

/*
 * Revision log:
 *
 * Created by SLK,2012/1/4
 *
 */

#include "dbapi.h"
#include "net.h"
#include "protocol.h"
#include "event.h"
#include "queue.h"
#include "servlistmgr.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>
#include <time.h>

#define MAX_BUF_LEN     1024

#define debug		printf

#define MAX_TASK_NUM      0
pthread_t thread_id[MAX_TASK_NUM];
tEvent event[MAX_TASK_NUM];
Queue taskq[MAX_TASK_NUM];

tLinkedList *gServerList = NULL;

typedef struct TaskNode
{
    QueueNode next;
    tServiceHandler req;
    char Buf[MAX_BUF_LEN];
    int BufSize;
}tTaskNode;  

#define RANDOM(x)                           			\
                  srand((unsigned)time(0)); 			\
                  i = random() % (x == 0 ? 1 : x);

/*
 * store cliend fd and database mapping table
 */
tDatabase CDManager = NULL;
#define InitCDManager()  CDManager = CreateMDB();
#define ShutCDManager()  CloseMDB(CDManager);
#define AttachCD(c,mdb)   								\
        tValue v;										\
        v.str = (char*)&mdb;							\
        v.len = sizeof(tDatabase); 						\
        SetRecordMDB(CDManager,c,v);
#define DettachCD(c)   									\
        DeleteRecordMDB(CDManager,c);          
#define GetMdb(c,mdb)   								\
        tValue v;										\
        v.str = (char*)&mdb;							\
        v.len = sizeof(tDatabase); 						\
        GetRecordMDB(CDManager,c,&v);      

int HandleRequests(int tasknum);
int Handler(tServiceHandler h,char *Buf,int BufSize);
int HandleOneRequest(tServiceHandler h,char *Buf,int BufSize);
int HandleControlRequest(tServiceHandler h,char *Buf,int BufSize);

int ServiceEngine(char *addr, int port)
{
    int i;
    if (MAX_TASK_NUM > 0)
    {
        for (i = 0; i < MAX_TASK_NUM; i++)
        {
            EventInit(&event[i],0);
            QueueCreate(&taskq[i]);
            int temp = i;
            if (pthread_create(&thread_id[i], NULL, (void*)HandleRequests, (void*)temp) != 0)
            {
                fprintf(stderr, "pthread_create Error,%s:%d\n", __FILE__, __LINE__);
                exit(-1);
            }            
        }           
    }       
    InitCDManager();
    /* Server Engine for Clients' Requests */
    tServiceHandler request = -1;
    debug("port:%d\n", port);
    if (InitializeNetService(addr, port) == -1)
    	return -1;
    printf("InitializeNetService at %s:%d\n", addr, port);
    while (1)
    {
        /* return the client fd who have real data request */
        request = ServiceStart();
        tTaskNode *pNode = malloc(sizeof(tTaskNode));
        pNode->BufSize = MAX_BUF_LEN;
        if (RecvData(request, pNode->Buf, &(pNode->BufSize)) == 0)
        {
            /* close by peer */
            ServiceStop(request);
            continue;        
        }        
        if (MAX_TASK_NUM > 0)
        {            
            pNode->req = request;
            RANDOM(MAX_TASK_NUM);	//i = random(MAX_TASK_NUM)
            debug("fd %d send to task %d\n", request, i);
            QueueInMsg(&taskq[i], (QueueNode*)pNode);
            SentEvent(&event[i]);
        }
        else
        {
            Handler(request, pNode->Buf, pNode->BufSize);
            free(pNode);
        }  
    }
    ShutdownNetService();
    ShutCDManager();
    if (MAX_TASK_NUM > 0)
    {        
        for (i = 0; i < MAX_TASK_NUM; i++)
        {
            EventShut(&event[i]);
            QueueDelete(&taskq[i]);
        }    
    }    
    return 0;
}

int ErrorResponse(tServiceHandler h, char * errorinfo)
{
    char Buf[MAX_BUF_LEN] = "\0";
    int BufSize = MAX_BUF_LEN;
    FormatData1(Buf, &BufSize,ERROR_RSP, errorinfo, strlen(errorinfo));
    SendData(h, Buf, BufSize);
    return 0;    
}

int HandleRequests(int tasknum)
{
    debug("task %d starts\n", tasknum);
    /* Handle Requests */
    tServiceHandler h = -1;
    int i = tasknum;
    while (1)
    {
        WaitEvent(&event[i]);
        debug("task %d get a event\n",i);
        tTaskNode *pNode = NULL;
        QueueOut(&taskq[i], (QueueNode**)&pNode);
        h = pNode->req; 
        if(Handler(h, pNode->Buf, pNode->BufSize) == -1)
        {
            free(pNode);
            continue;
        }
        free(pNode);
    }
}
int Handler(tServiceHandler h, char *Buf, int BufSize)
{
        int cmd = -1;
        int DataNum = -1;
        ParseCmd(Buf, BufSize, &cmd, &DataNum);
        if (cmd >= 0 && cmd > CTRL_CMD)
        {
            if (HandleControlRequest(h, Buf, BufSize) == -1)
            {
                return -1;
            }            
        }
        else
        {            
            if (HandleOneRequest(h, Buf, BufSize) == -1)
            {
                return -1;
            }
        }
        return 0;
            
}
int HandleOneRequest(tServiceHandler h, char *Buf, int BufSize)
{
    int cmd = -1;
    int DataNum = -1;
    char Data1[MAX_BUF_LEN] = "\0";
    int Data1Size = MAX_BUF_LEN;
    char Data2[MAX_BUF_LEN] = "\0";
    int Data2Size = MAX_BUF_LEN;  
      
    /* Handle One Request */
    if (BufSize == 0)
    {
        return -1; 
    }
    Data1Size = MAX_BUF_LEN;
    Data2Size = MAX_BUF_LEN;
    int ret = ParseData(Buf,BufSize, &cmd, &DataNum, Data1, &Data1Size, Data2, &Data2Size);
    if (ret == -1)
    {
        ErrorResponse(h,"Data Format Error!\n");
        return -1; 
    }
    if (cmd == OPEN_CMD && DataNum == 1)
    {
        debug("OPEN_CMD\n");
        tDatabase  db = NULL;
        db = CreateDB(Data1);
        AttachCD(h,db);
        BufSize = MAX_BUF_LEN;
        FormatData(Buf, &BufSize, OPEN_RSP);
        SendData(h, Buf, BufSize);        
    }
    else if (cmd == CLOSE_CMD && DataNum == 0)
    {
        debug("CLOSE_CMD\n");
        tDatabase  db = NULL;
        GetMdb(h,db);
        CloseDB(db);
        BufSize = MAX_BUF_LEN;
        FormatData(Buf, &BufSize, CLOSE_RSP);
        SendData(h, Buf, BufSize);
        DettachCD(h);
        ServiceStop(h);       
        return 0;
    }
    else if(cmd == GET_CMD && DataNum == 1)
    {
        debug("GET_CMD\n");
        if (Data1Size != sizeof(tKey))
        {
            fprintf(stderr,"Data Format Error,%s:%d\n",__FILE__,__LINE__);
            return -1;        
        }
        tDatabase  db = NULL;
        GetMdb(h,db);        
        tKey key = *(tKey*)Data1;
        tValue value;
        Data2Size = MAX_BUF_LEN;
        value.len = Data2Size;
        value.str = Data2;
        ret = GetRecord(db,key,&value);
        if (ret == FAILURE)
        {
            ErrorResponse(h,"The key NOT FOUND!\n");
            return -1; 
        }
        BufSize = MAX_BUF_LEN;
        FormatData2(Buf, &BufSize, GET_RSP, (char*)&key, sizeof(tKey), value.str, value.len);
        SendData(h,Buf,BufSize);         
    }
    else if (cmd == SET_CMD && DataNum == 2)
    {
        debug("SET_CMD\n");
        if (Data1Size != sizeof(tKey))
        {
            fprintf(stderr,"Data Format Error,%s:%d\n", __FILE__, __LINE__);
            return -1;        
        }
        tKey key = *(tKey*)Data1;
        tValue value;
        value.len = Data2Size;
        value.str = Data2;
        //debug("SET_CMD:%d -> %s\n", *(tKey*)(Buf + 12), (char*)(Buf + 20));
        debug("SET_CMD:%d -> %s\n", key, value.str);
        tDatabase  db = NULL;
        GetMdb(h, db);        
        SetRecord(db, key, value);
        BufSize = MAX_BUF_LEN;
        FormatData(Buf, &BufSize, SET_RSP);
        SendData(h, Buf, BufSize);                 
    }
    else if (cmd == DELETE_CMD && DataNum == 1)
    {
        debug("DELETE_CMD\n");
        if (Data1Size != sizeof(tKey))
        {
            fprintf(stderr, "Data Format Error,%s:%d\n", __FILE__, __LINE__);
            return -1;         
        }
        tKey key = *(tKey*)Data1; 
        tDatabase  db = NULL;
        GetMdb(h, db);              
        ret = DeleteRecord(db, key);
        if (ret == FAILURE)
        {
            ErrorResponse(h, "The key NOT FOUND!\n");
            return -1; 
        }        
        BufSize = MAX_BUF_LEN;
        FormatData(Buf, &BufSize, DELETE_RSP);
        SendData(h, Buf, BufSize);         
    }
    else
    {
        ErrorResponse(h,"Unknow Request!\n");
        return -1;
    } 
    return 0;                     
}
int HandleControlRequest(tServiceHandler h, char *Buf, int BufSize)
{
    int cmd = -1;
    int DataNum = -1;  

    if (BufSize == 0)
    {
        return -1; 
    }
    int ret;
    char ppData[MAX_DATA_NUM][MAX_DATA_LEN] = {0};
    ret = ParseDataN(Buf, BufSize, &cmd, &DataNum, ppData);
    if (DataNum > 1)
    {
        ret = -1;
    }
    if (ret == -1)
    {
        ErrorResponse(h, "Data Format Error!\n");
        return -1; 
    }
    if (cmd == CTRL_REG_CMD && (DataNum == 1 || DataNum == 0))
    {
        debug("CTRL_REG_CMD\n");
        if (DataNum == 1)
        {
            AddServer(gServerList, ppData, DataNum);       
        }
        int NodeNum = MAX_DATA_NUM;
        PackServer(gServerList, ppData, &NodeNum);       
        BufSize = MAX_BUF_LEN;
        FormatDataN(Buf, &BufSize, CTRL_REG_RSP, ppData, NodeNum);
        SendData(h, Buf, BufSize);         
    }
    else if (cmd == CTRL_RVK_CMD && (DataNum == 1 || DataNum == 0))
    {
        debug("CTRL_RVK_CMD\n");
        if (DataNum == 1)
        {
            DelServer(gServerList, ppData, DataNum);       
        }
        int NodeNum = MAX_DATA_NUM;
        PackServer(gServerList, ppData, &NodeNum);       
        BufSize = MAX_BUF_LEN;
        FormatDataN(Buf, &BufSize, CTRL_RVK_RSP, ppData, NodeNum);
        SendData(h, Buf, BufSize);         
    }
    else
    {
        ErrorResponse(h, "Unknow Request!\n");
        return -1;
    } 
    return 0;    
}
/* create cluster in master */
int CreateCluster(char *addr, int port)
{
    gServerList = InitServerList(addr, port);
    return 0;
}

/* loading cluster nodes in nodes */
int LoadingClusterNodes(char *addr, int port)
{
    gServerList = RegisterServer(addr, port);
    return 0;
}

int UnLoadingClusterNodes(char *addr, int port)
{
    RevokeServer(addr, port);
    return 0;
}
