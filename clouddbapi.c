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

//#define PORT                5001
#define IP_ADDR             "192.168.10.197"
#define MAX_BUF_LEN         1024
#define ADDR_STR_LEN        128
#define MAX_NODE_NUM        3

#define debug  printf
              
typedef struct ServerNode
{
    char addr[ADDR_STR_LEN];
    int  port;
    int  db;
}tServerNode;

/* Initialize servers info */
tServerNode nodes[MAX_NODE_NUM] = 
{
    {IP_ADDR, 5000, -1},
    {IP_ADDR, 5001, -1},
    {IP_ADDR, 5002, -1}   
};

/*
 * Create an Database
 */
tDatabase CreateDB(const char *dbName)
{
    int i;
    for(i = 0; i < MAX_NODE_NUM; i++)
    {
        nodes[i].db = CreateRemoteDB(nodes[i].addr, nodes[i].port, dbName);
        if(nodes[i].db == -1)
        {
            return NULL;
        }
    }      
    return (tDatabase)nodes;
}

/*
 * Delete the Database
 */
int CloseDB(tDatabase db)
{
    tServerNode *pnodes = (tServerNode*)db;
    int i;
    for(i = 0; i < MAX_NODE_NUM; i++)
    {    
        CloseRemoteDB(pnodes[i].db);
    }
    return 0;
}


/*
 * Set key/value
 */
int SetRecord(tDatabase db, tKey key, tValue value)
{
    tServerNode *pnodes = (tServerNode*)db;
    int nodeindex = key % MAX_NODE_NUM;/* distribute strategy */
    debug("key=%d,nodeindex=%d\n", key, nodeindex);
    return SetRecordRemote(pnodes[nodeindex].db, key, value);
}

/*
 * get key/value
 */
int GetRecord(tDatabase db, tKey key, tValue *pvalue)
{
    tServerNode *pnodes = (tServerNode*)db;
    int nodeindex = key % MAX_NODE_NUM;/* distribute strategy */ 
    debug("key=%d,nodeindex=%d\n", key, nodeindex);  
    return GetRecordRemote(pnodes[nodeindex].db, key, pvalue);
}

/*
 * delete key/value
 */
int DeleteRecord(tDatabase db, tKey key)
{
    tServerNode *pnodes = (tServerNode*)db;
    int nodeindex = key % MAX_NODE_NUM;/* distribute strategy */ 
    debug("key=%d,nodeindex=%d\n", key, nodeindex);   
    return DeleteRecordRemote(pnodes[nodeindex].db, key);
}
