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
#ifndef _SERV_LIST_MGR_H_
#define _SERV_LIST_MGR_H_

#include "linkedlist.h"
#include "protocol.h"

#define PORT                6000
#define IP_ADDR             "127.0.0.1"

#define ADDR_STR_LEN        128              

typedef struct ServerNode
{
	tLNode *pNext;
    char addr[ADDR_STR_LEN];
    int  port;
    int  fd;
}tServerNode;

/* Init server list info */
tLinkedList* InitServerList(char* addr, int port);
//tLinkedList* InitServerList();
/* Register And Load serverList info */
tLinkedList* RegisterServer(char* addr, int port);
/* delete a server node from linked list */
int RevokeServer(char *addr, int port);
/* search a server node from linked list */
tLNode* SearchServerFunc(tLinkedList *pLinkedList, tLNode *pNode);
/* add recved buf(serverList info) to linked list */
int AddServer(tLinkedList *serverList, char ppData[MAX_DATA_NUM][MAX_DATA_LEN], int DataNum);
/* del recved buf(serverList info) from linked list */
int DelServer(tLinkedList *serverList, char ppData[MAX_DATA_NUM][MAX_DATA_LEN], int DataNum);
/* LinkedList(server info) to ppData array */
int PackServer(tLinkedList *serverList, char ppData[MAX_DATA_NUM][MAX_DATA_LEN], int *NodeNum);

#endif /* _SERV_LIST_MGR_H_ */
