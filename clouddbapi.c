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
#include "servlistmgr.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define MAX_NODE_NUM        serverList->len

#define debug  printf

/* find the node in linked list */
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
        pNode = (tServerNode*)GetNextLNode(serverList, (tLNode*)pNode);
        i++;
    }
    return NULL;
}

/*
 * Create an Database
 */
tDatabase CreateDB(const char *dbName)
{
    tLinkedList *serverList = RegisterServer(NULL, 0);
    tServerNode *pNode = (tServerNode*)GetLHead(serverList);
    while (pNode != NULL)
    {
    	debug("%s:%d\n", pNode->addr, pNode->port);
    	pNode->fd = CreateRemoteDB(pNode->addr, pNode->port, dbName);
    	if (pNode->fd == -1)
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
int CloseDB(tDatabase fd)
{
	tLinkedList *serverList = (tLinkedList*)fd;
    tServerNode *pNode = (tServerNode*)GetLHead(serverList);
    //printf("remote db closing\n");
    while (pNode != NULL)
    {    
    	if (pNode->fd != -1)
    	{
        	CloseRemoteDB(pNode->fd);
        	printf("remote db closed\n");
        }
        pNode = (tServerNode*)GetNextLNode(serverList, (tLNode*)pNode);
    }
    return 0;
}


/*
 * Set key/value
 */
int SetRecord(tDatabase fd, tKey key, tValue value)
{
    tLinkedList *serverList = (tLinkedList*)fd;
    int nodeIndex = key % MAX_NODE_NUM;	/* distribute strategy */
    tServerNode *pNode = FindNode(serverList, nodeIndex);
    debug("key=%d,nodeindex=%d\n", key, nodeIndex);
    return SetRecordRemote(pNode->fd, key, value);
}

/*
 * get key/value
 */
int GetRecord(tDatabase fd, tKey key, tValue *pvalue)
{
    tLinkedList *serverList = (tLinkedList*)fd;
    int nodeIndex = key % MAX_NODE_NUM;	/* distribute strategy */
    tServerNode *pNode = FindNode(serverList, nodeIndex);
    debug("key=%d,nodeindex=%d\n", key, nodeIndex); 
    return GetRecordRemote(pNode->fd, key, pvalue);
}

/*
 * delete key/value
 */
int DeleteRecord(tDatabase fd, tKey key)
{
    tLinkedList *serverList = (tLinkedList*)fd;
    int nodeIndex = key % MAX_NODE_NUM;	/* distribute strategy */
    tServerNode *pNode = FindNode(serverList, nodeIndex);
    debug("key=%d,nodeindex=%d\n", key, nodeIndex); 
    return DeleteRecordRemote(pNode->fd, key);  
}
