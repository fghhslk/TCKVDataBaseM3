/********************************************************************/
/* Copyright (C) MC2Lab-USTC, 2012                                  */
/*                                                                  */
/*  FILE NAME             :  clouddbapi.c                           */
/*  PRINCIPAL AUTHOR      :  SLK                                    */
/*  SUBSYSTEM NAME        :  cloud                                  */
/*  MODULE NAME           :  clouddbapi                             */
/*  LANGUAGE              :  C                                      */
/*  TARGET ENVIRONMENT    :  Linux                                  */
/*  DATE OF FIRST RELEASE :  2012/12/30                             */
/*  DESCRIPTION           :  Impement of Client Side Interface      */
/*                           to access cloud database(multi-servers)*/
/*                           distribute strategy inside here        */
/********************************************************************/

/*
 * Revision log:
 *
 * Created by SLK,2012/12/30
 *
 */

#include "dbapi.h"
#include "remotedbapi.h"
#include "net.h"
#include "protocol.h"
#include "linkedlist.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define IP_ADDR             "192.168.100.233"
#define MAX_BUF_LEN         1024
#define ADDR_STR_LEN        128
#define MAX_NODE_NUM        serverList->len

#define debug  printf
              
typedef struct ServerNode
{
	tLNode *pNext;
    char addr[ADDR_STR_LEN];
    int  port;
    int  db;
}tServerNode;

/* 
 * Add a Server to Server List 
 */
int addServer(tLinkedList *serverList, char *addr, int port)
{
	tServerNode *pNode = (tServerNode*) malloc(sizeof(tServerNode));
    memcpy(pNode->addr, addr, strlen(addr) + 1);
    pNode->port = port;
    pNode->db = -1;
    AddLNode(serverList, (tLNode*)pNode);
    return 0;
}

/* 
 * Initialize Server List 
 */
static tLinkedList* InitServerList()
{
	int i;
	tLinkedList *serverList = CreateLinkedList();
	for (i = 0; i < 3; i++)
	{
		addServer(serverList, IP_ADDR, 5000 + i);
	}
	return serverList;
}

static tServerNode *FindNode(tLinkedList *serverList, int index)
{
    int i = 0;
    tServerNode *pNode = (tServerNode*)GetLHead(serverList);
    while (pNode != NULL)
    {
        if (i == index)
        {
            return pNode;;
        }
        pNode = (tServerNode*)GetNextLNode(serverList,(tLNode*)pNode);
        i++;
    }
    return NULL;
}

int updateServerList(int num, tServerNode *recvList, tServerNode *serverList)
{
    return 0;   
}


/*
 * Create an Database
 */
tDatabase CreateDB(const char *dbName)
{
    tLinkedList *serverList = InitServerList();
    tServerNode *pNode = (tServerNode*)GetLHead(serverList);
    while (pNode != NULL)
    {
    	debug("%s:%d\n", pNode->addr, pNode->port);
    	pNode->db = CreateRemoteDB(pNode->addr, pNode->port, dbName);
    	if (pNode->db == -1)
    	{
    		return NULL;
    	}
    	pNode = (tServerNode*)GetNextLNode(serverList,(tLNode*)pNode);	
    }     
    return (tDatabase)serverList;
}

/*
 * Delete the Database
 */
int CloseDB(tDatabase db)
{
	tLinkedList *serverList = (tLinkedList*)db;
    tServerNode *pNode = (tServerNode*)GetLHead(serverList);
    int i;
    while (pNode != NULL)
    {    
    	if (pNode->db != -1)
    	{
        	CloseRemoteDB(pNode->db);
        }
        pNode = (tServerNode*)GetNextLNode(serverList,(tLNode*)pNode);
    }
    return 0;
}


/*
 * Set key/value
 */
int SetRecord(tDatabase db, tKey key, tValue value)
{
    tLinkedList *serverList = (tLinkedList*)db;
    int nodeIndex = key % MAX_NODE_NUM;	/* distribute strategy */
    tServerNode *pNode = FindNode(serverList, nodeIndex);
    debug("key=%d,nodeindex=%d\n", key, nodeIndex);
    return SetRecordRemote(pNode->db, key, value);
}

/*
 * get key/value
 */
int GetRecord(tDatabase db, tKey key, tValue *pvalue)
{
    tLinkedList *serverList = (tLinkedList*)db;
    int nodeIndex = key % MAX_NODE_NUM;	/* distribute strategy */
    tServerNode *pNode = FindNode(serverList, nodeIndex);
    debug("key=%d,nodeindex=%d\n", key, nodeIndex); 
    return GetRecordRemote(pNode->db, key, pvalue);
}

/*
 * delete key/value
 */
int DeleteRecord(tDatabase db, tKey key)
{
    tLinkedList *serverList = (tLinkedList*)db;
    int nodeIndex = key % MAX_NODE_NUM;	/* distribute strategy */
    tServerNode *pNode = FindNode(serverList, nodeIndex);
    debug("key=%d,nodeindex=%d\n", key, nodeIndex); 
    return DeleteRecordRemote(pNode->db, key);  
}
