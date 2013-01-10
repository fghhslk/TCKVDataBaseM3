/********************************************************************/
/* Copyright (C) SSE-USTC, 2012                                     */
/*                                                                  */
/*  FILE NAME             : protocol.c                              */
/*  PRINCIPAL AUTHOR      : SLK                                     */
/*  SUBSYSTEM NAME        : network                                 */
/*  MODULE NAME           : protocol                                */
/*  LANGUAGE              : C                                       */
/*  TARGET ENVIRONMENT    : Linux                                   */
/*  DATE OF FIRST RELEASE : 2012/12/14                              */
/*  DESCRIPTION           : Protocol between Vampire Sever and      */
/*  Client                                                          */
/********************************************************************/

/*
* Revision log:
*
* Created by SLK,2012/12/14
*
*/

#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_
/*
 * Data format
    0---------------------------------------------------31
    |         operation command(int-4bytes)             |
    -----------------------------------------------------
    |         number of data(int-4bytes) = 0..x         |
    -----------------------------------------------------
    |            data-1 length(int-4bytes)              |
    -----------------------------------------------------
    |                                                   |
    /                   data-1                          /
    |                                                   |
    -----------------------------------------------------
    |          data-2..x length(int-4bytes)             |
    -----------------------------------------------------
    |                                                   |
    /                   data-2..x                       /
    |                                                   |
    0---------------------------------------------------31
 */
 
/* operation commands */
#define OPEN_CMD        1 /* With one data - database file name */
#define CLOSE_CMD       2 /* With zero data */
#define GET_CMD         3 /* With one data - key */
#define SET_CMD         4 /* With two data - key and value */
#define DELETE_CMD      5 /* With one data - key */
#define ERROR_RSP       0 /* With one data - Error infomation */
#define OPEN_RSP        OPEN_CMD    /* if success,With zero data */
#define CLOSE_RSP       CLOSE_CMD   /* if success,With zero data */
#define GET_RSP         GET_CMD     /* if success,With two data - key and value */
#define SET_RSP         SET_CMD     /* if success,With zero data */
#define DELETE_RSP      DELETE_CMD  /* if success,With zero data */

/* Control operation commands */
#define CTRL_CMD          100          /* Control cmd starts here */
/* Registe Node info */
#define CTRL_REG_CMD      101          /* with zero or one data(IP:PORT) */
#define CTRL_RVK_CMD      102          /* with zero or one data(IP:PORT) */
#define CTRL_REG_RSP      CTRL_REG_CMD /* with N data(IP:PORT) */
#define CTRL_RVK_RSP	  CTRL_RVK_CMD /* with N data(IP:PORT) */

#define MAX_DATA_NUM        128
#define MAX_DATA_LEN        128
/*
 * FormatData - with 0-2 data
 * input	: cmd,data and data size
 * in/out	: pBuf - to store formated packet 
 * in/out	: pBufSize - size of the memory pointed by pBuf,and output the size be used
 * return	: if SUCCESS return 0
 *          : if FAILURE return (-1)
 */
int FormatData(char *pBuf, int *pBufSize, int cmd);
int FormatData1(char *pBuf, int *pBufSize, int cmd, char* pData1, int Data1Size);
int FormatData2(char *pBuf, int *pBufSize, int cmd, char* pData1, int Data1Size, char* pData2, int Data2Size);
int FormatDataN(char *pBuf, int *pBufSize, int cmd, char ppData[MAX_DATA_NUM][MAX_DATA_LEN], int DataNum);

/*
 * ParseData - with 0-2 data
 * input	: pBuf - point to recved packet 
 * input	: BufSize - size of packet pointed by pBuf
 * output   : pcmd,pDataNum
 * output   : pData1,pData1Size,pData2,pData2Size,if no data,*pDataX == '\0',*pData1Size == 0
 * return	: if SUCCESS return 0
 *          : if FAILURE return (-1)
 */
int ParseCmd(char * pBuf, int BufSize, int *pcmd, int *pDataNum);
int ParseData(char *pBuf, int BufSize, int *pcmd, int *pDataNum, char *pData1, int *pData1Size, char* pData2, int *pData2Size);
int ParseDataN(char *pBuf, int BufSize, int *pcmd, int *pDataNum, char ppData[MAX_DATA_NUM][MAX_DATA_LEN]);

        
#endif /* _PROTOCOL_H_ */
