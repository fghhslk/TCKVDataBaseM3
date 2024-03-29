/********************************************************************/
/* Copyright (C) SSE-USTC, 2012-2013                                */
/*                                                                  */
/*  FILE NAME             :  servlistmgr.c                          */
/*  PRINCIPAL AUTHOR      :  SLK                                    */
/*  SUBSYSTEM NAME        :  cloud                                  */
/*  MODULE NAME           :  servlistmgr                            */
/*  LANGUAGE              :  C                                      */
/*  TARGET ENVIRONMENT    :  Linux                                  */
/*  DATE OF FIRST RELEASE :  2013/1/4                               */
/*  DESCRIPTION           :  server List Manage module              */
/********************************************************************/

/*
 * Revision log:
 *
 * Created by SLK,2013/1/4
 *
 */
#include "servlistmgr.h"
#include "linkedlist.h"
#include "net.h"
#include "protocol.h"
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>

#define MAX_BUF_LEN     1024

#define debug              

/* Init server list info */
tLinkedList* InitServerList(char* addr, int port)
//tLinkedList* InitServerList()
{

    tLinkedList *serverList = CreateLinkedList();
    tServerNode *pNode = (tServerNode*) malloc(sizeof(tServerNode));
    if (addr != NULL)
    {
        memcpy(pNode->addr,addr,strlen(addr));
        pNode->addr[strlen(addr) + 1] = '\0';
        pNode->port = port;
        pNode->fd = -1;
        AddLNode(serverList, (tLNode*)pNode);
    }
    return serverList;
}

/* Register And Load serverList info */
tLinkedList* RegisterServer(char* addr, int port)
{
    int i;
    int DataNum = -1;
    char Buf[MAX_BUF_LEN] = "\0";
    int BufSize = MAX_BUF_LEN;
    char szMsg[MAX_BUF_LEN] = "\0";
    char ppData[MAX_DATA_NUM][MAX_DATA_LEN] = {0};
    tServiceHandler h = -1;
    h = OpenRemoteService(IP_ADDR, PORT);
    if (addr == NULL)
    {
        DataNum = 0;
    }
    else
    {
        DataNum = 1;
        sprintf(ppData[0], "%s %d\0", addr, port);
        debug("pasrer sprintf :%s\n", ppData[0]);     
    }
    FormatDataN(Buf, &BufSize, CTRL_REG_CMD, ppData, DataNum);
    SendData(h, Buf, BufSize);
    RecvData(h, Buf, &BufSize);
    int cmd = -1;
    ParseDataN(Buf, BufSize, &cmd, &DataNum, ppData);
    if (cmd != CTRL_REG_RSP || DataNum > MAX_DATA_NUM || DataNum < 2)
    {
        fprintf(stderr, "AddServer Error,%s:%d\n", __FILE__, __LINE__);
        return NULL;         
    }
    tLinkedList* serverList = CreateLinkedList();
    AddServer(serverList, ppData, DataNum); 
    //debug("Add Server Successfully\n");   
    CloseRemoteService(h);
    return serverList;    
}
/* add recved buf(serverList info) to linked list */
int AddServer(tLinkedList *serverList, char ppData[MAX_DATA_NUM][MAX_DATA_LEN], int DataNum)
{
    if (serverList == NULL || ppData == NULL)
    {
        fprintf(stderr, "AddCloudNodes Error,%s:%d\n", __FILE__, __LINE__);
        return -1;         
    }
    if (DataNum == 0)
    {
        return 0;
    }
    int i;
    for (i = 0; i < DataNum; i++)
    {
        tServerNode* pNode = (tServerNode*) malloc(sizeof(tServerNode));
        char addr[MAX_DATA_LEN];
        int port;
        sscanf(ppData[i], "%s%d", addr, &port);
        debug("pasrer %s:%d\n", addr, port);
        memcpy(pNode->addr, addr, strlen(ppData[i]));
        pNode->addr[strlen(ppData[i]) + 1] = '\0';
        pNode->port = port;
        pNode->fd = -1;
        AddLNode(serverList, (tLNode*)pNode);
    }
    return 0;
}

/* revoke a server node from linked list */
int RevokeServer(char *addr, int port)
{
	int i;
    int DataNum = -1;
    char Buf[MAX_BUF_LEN] = "\0";
    int BufSize = MAX_BUF_LEN;
    char szMsg[MAX_BUF_LEN] = "\0";
    char ppData[MAX_DATA_NUM][MAX_DATA_LEN] = {0};
    tServiceHandler h = -1;
    h = OpenRemoteService(IP_ADDR, PORT);
    if (addr == NULL)
    {
        DataNum = 0;
    }
    else
    {
        DataNum = 1;
        sprintf(ppData[0], "%s %d\0", addr, port);
        debug("pasrer sprintf :%s\n", ppData[0]);     
    }
    FormatDataN(Buf, &BufSize, CTRL_RVK_CMD, ppData, DataNum);
    SendData(h, Buf, BufSize);
    RecvData(h, Buf, &BufSize);
    int cmd = -1;
    ParseDataN(Buf, BufSize, &cmd, &DataNum, ppData);
    if (cmd != CTRL_RVK_RSP || DataNum > MAX_DATA_NUM || DataNum < 2)
    {
        fprintf(stderr, "DelServer Error,%s:%d\n", __FILE__, __LINE__);
        return -1;         
    } 
    CloseRemoteService(h);
    return 0;
}
/* del recved buf(serverList info) from linked list */
int DelServer(tLinkedList *serverList, char ppData[MAX_DATA_NUM][MAX_DATA_LEN], int DataNum)
{
	if (serverList == NULL || ppData == NULL)
    {
        fprintf(stderr, "DelCloudNodes Error,%s:%d\n", __FILE__, __LINE__);
        return -1;         
    }
    if (DataNum == 0)
    {
        return 0;
    }
    int i;
    for (i = 0; i < DataNum; i++)
    {
        tServerNode *tmpNode = (tServerNode*) malloc(sizeof(tServerNode));
        char addr[MAX_DATA_LEN];
        int port;
        sscanf(ppData[i], "%s%d", addr, &port);
        debug("pasrer %s:%d\n", addr, port);
        memcpy(tmpNode->addr, addr, strlen(ppData[i]));
    	tmpNode->addr[strlen(ppData[i]) + 1] = '\0';
    	tmpNode->port = port;
    	tmpNode->fd = -1;
    	tServerNode *pNode = (tServerNode*)SearchLNode(serverList, (tLNode*)tmpNode, (SearchFunc)SearchServerFunc);
        debug("list length:%d\n", serverList->len);
	    DelLNode(serverList, (tLNode*)pNode);
	    debug("list length:%d\n", serverList->len);
	    free(tmpNode);
    }
    return 0;
}
/* search a server node from linked list */
tLNode* SearchServerFunc(tLinkedList *pLinkedList, tLNode *pNode)
{
	tServerNode *curNode;
	tServerNode *tmpNode;
	if (pLinkedList == NULL || pLinkedList->pHead == NULL 
		|| pLinkedList->pHead->pNext == NULL || pNode == NULL)
	{
		debug("Node not found\n");
		return NULL;
	}
	curNode = (tServerNode*)pLinkedList->pHead;
	tmpNode = (tServerNode*)pNode;
	while (curNode != NULL)
	{
		debug("current find port:%d\n", curNode->port);
		if (strcmp(curNode->addr, tmpNode->addr) == 0
			&& curNode->port == tmpNode->port)
			return (tLNode*)curNode;
		curNode = (tServerNode*)curNode->pNext;	
	}
	return NULL;
}

/* LinkedList(server info) to ppData array */
int PackServer(tLinkedList *serverList, char ppData[MAX_DATA_NUM][MAX_DATA_LEN], int *NodeNum)
{
    if (*NodeNum < serverList->len)
    {
        fprintf(stderr, "PackServer Error,%s:%d\n", __FILE__, __LINE__);
        return -1;
    }
    *NodeNum = serverList->len;
    int i = 0;
    tServerNode *pNode = (tServerNode*)GetLHead(serverList);
    while (pNode != NULL)
    {
        sprintf(ppData[i], "%s %d\0", pNode->addr, pNode->port);
        debug("pasrer sprintf :%s\n", ppData[i]);
        pNode = (tServerNode*)GetNextLNode(serverList, (tLNode*)pNode);
        i++;
    }
    return 0;  
}