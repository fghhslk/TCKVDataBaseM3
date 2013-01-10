/********************************************************************/
/* Copyright (C) MC2Lab-USTC, 2012                                  */
/*                                                                  */
/*  FILE NAME             : remotedbapi.c                           */
/*  PRINCIPAL AUTHOR      : SLK                                     */
/*  SUBSYSTEM NAME        : network                                 */
/*  MODULE NAME           : remotedbapi                             */
/*  LANGUAGE              : C                                       */
/*  TARGET ENVIRONMENT    : Linux                                   */
/*  DATE OF FIRST RELEASE : 2012/12/14                              */
/*  DESCRIPTION           : Impement of Cliet Side Interface        */
/*  to access database                                              */
/********************************************************************/

/*
* Revision log:
*
* Created by SLK,2012/12/14
*
*/

#include "net.h"
#include "protocol.h"
#include "remotedbapi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define MAX_BUF_LEN 1024
#define debug

/*
* Create an Database
*/
int CreateRemoteDB(char *addr, int port, const char * dbName)
{
    if(dbName == NULL)
    {
        fprintf(stderr, "filename Error,%s:%d\n", __FILE__, __LINE__);
        dbName = "default.hdb";
    }
    /* connect server */
    tServiceHandler h = OpenRemoteService(addr, port);
    if(h == -1)
    {
        return -1;
    }
    /* open database file */
    char Buf[MAX_BUF_LEN] = "\0";
    int BufSize = MAX_BUF_LEN;
    FormatData1(Buf, &BufSize, OPEN_CMD, (char*)dbName, strlen(dbName));
    SendData(h, Buf, BufSize);
    BufSize = MAX_BUF_LEN;
    RecvData(h, Buf, &BufSize);
    int cmd = -1;
    int DataNum = -1;
    char Data1[MAX_BUF_LEN] = "\0";
    int Data1Size = MAX_BUF_LEN;
    char Data2[MAX_BUF_LEN] = "\0";
    int Data2Size = MAX_BUF_LEN;
    ParseData(Buf, MAX_BUF_LEN, &cmd, &DataNum, Data1, &Data1Size, Data2, &Data2Size);
    if(cmd != OPEN_RSP || DataNum != 0)
    {
        fprintf(stderr, "CreateRemoteDB Error,%s:%d\n", __FILE__, __LINE__);
        return -1;
    }
    return h;
}

/*
* Delete the Database
*/
int CloseRemoteDB(int db)
{
    /* close database file */
    char Buf[MAX_BUF_LEN] = "\0";
    int BufSize = MAX_BUF_LEN;
    FormatData(Buf, &BufSize, CLOSE_CMD);
    //SendData(*(tServiceHandler*)db, Buf, BufSize);
    SendData(db, Buf, BufSize);
    BufSize = MAX_BUF_LEN;
    if(RecvData(db, Buf, &BufSize) == 0)
    {
        fprintf(stderr, "Connection Error,%s:%d\n", __FILE__, __LINE__);
        return -1;
    }
    int cmd = -1;
    int DataNum = -1;
    char Data1[MAX_BUF_LEN] = "\0";
    int Data1Size = MAX_BUF_LEN;
    char Data2[MAX_BUF_LEN] = "\0";
    int Data2Size = MAX_BUF_LEN;
    ParseData(Buf, MAX_BUF_LEN, &cmd, &DataNum, Data1, &Data1Size, Data2, &Data2Size);
    if(cmd != CLOSE_RSP || DataNum != 0)
    {
        fprintf(stderr, "Remote DBDelete Error,%s:%d\n", __FILE__, __LINE__);
        return -1;
    }
    /* close connection */
    if(CloseRemoteService(db) == -1)
    {
        return -1;
    }
    return 0;
}


/*
* Set key/value
*/
int SetRecordRemote(int db, tKey key, tValue value)
{
    debug("SET_CMD:%d -> %s\n", key, value.str);
    char Buf[MAX_BUF_LEN] = "\0";
    int BufSize = MAX_BUF_LEN;
    int ret = FormatData2(Buf, &BufSize, SET_CMD, (char*)&key, sizeof(tKey), value.str,value.len);
    if(ret == -1)
    {
        return -1;
    }
    SendData(db, Buf, BufSize);
    BufSize = MAX_BUF_LEN;
    if(RecvData(db, Buf, &BufSize) == 0)
    {
        fprintf(stderr, "Connection Error,%s:%d\n", __FILE__, __LINE__);
        return -1;
    }
    int cmd = -1;
    int DataNum = -1;
    char Data1[MAX_BUF_LEN] = "\0";
    int Data1Size = MAX_BUF_LEN;
    char Data2[MAX_BUF_LEN] = "\0";
    int Data2Size = MAX_BUF_LEN;
    ParseData(Buf, BufSize, &cmd, &DataNum, Data1, &Data1Size, Data2, &Data2Size);
    if(cmd != SET_RSP || DataNum != 0)
    {
        fprintf(stderr, "SetRecordRemote Error,%s:%d\n", __FILE__, __LINE__);
        return -1;
    }
    return 0;
}

/*
* get key/value
*/
int GetRecordRemote(int db, tKey key, tValue *pvalue)
{
    if(db == -1 || pvalue == NULL)
    {
        return -1;
    }
    char Buf[MAX_BUF_LEN] = "\0";
    int BufSize = MAX_BUF_LEN;
    int ret = FormatData1(Buf, &BufSize, GET_CMD, (char*)&key, sizeof(tKey));
    if(ret == -1)
    {
        return -1;
    }
    SendData(db, Buf, BufSize);
    BufSize = MAX_BUF_LEN;
    if(RecvData(db, Buf, &BufSize) == 0)
    {
        fprintf(stderr, "Connection Error,%s:%d\n", __FILE__, __LINE__);
        return -1;
    }
    int cmd = -1;
    int DataNum = -1;
    char Data1[MAX_BUF_LEN] = "\0";
    int Data1Size = MAX_BUF_LEN;
    char Data2[MAX_BUF_LEN] = "\0";
    int Data2Size = MAX_BUF_LEN;
    ParseData(Buf, BufSize, &cmd, &DataNum, Data1, &Data1Size, Data2, &Data2Size);
    if(cmd == ERROR_RSP && DataNum == 1)
    {
        return -1;
    }
    if(cmd != GET_RSP || DataNum != 2
        || key != *(tKey*)Data1 || Data1Size != sizeof(tKey))
    {
        fprintf(stderr, "GetRecordRemote Error,%s:%d\n", __FILE__, __LINE__);
        return -1;
    }
    pvalue->len = Data2Size;
    pvalue->str = Data2;
    return 0;
}

/*
* delete key/value
*/
int DeleteRecordRemote(int db, tKey key)
{
    if(db == -1)
    {
        return -1;
    }
    char Buf[MAX_BUF_LEN] = "\0";
    int BufSize = MAX_BUF_LEN;
    int ret = FormatData1(Buf, &BufSize, DELETE_CMD, (char*)&key, sizeof(tKey));
    if(ret == -1)
    {
        return -1;
    }
    SendData(db, Buf, BufSize);
    BufSize = MAX_BUF_LEN;
    if(RecvData(db, Buf, &BufSize) == 0)
    {
        fprintf(stderr, "Connection Error,%s:%d\n", __FILE__, __LINE__);
        return -1;
    }
    int cmd = -1;
    int DataNum = -1;
    char Data1[MAX_BUF_LEN] = "\0";
    int Data1Size = MAX_BUF_LEN;
    char Data2[MAX_BUF_LEN] = "\0";
    int Data2Size = MAX_BUF_LEN;
    ParseData(Buf, BufSize, &cmd, &DataNum, Data1, &Data1Size, Data2, &Data2Size);
    if(cmd == ERROR_RSP && DataNum == 1)
    {
        fprintf(stderr, "%s,%s:%d\n", Data1, __FILE__, __LINE__);
        return -1;
    }
    return 0;
}