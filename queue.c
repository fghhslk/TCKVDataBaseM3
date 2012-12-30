/********************************************************************/
/* Copyright (C) SSE-USTC, 2010                                     */
/*                                                                  */
/*  FILE NAME             : queue.c                                 */
/*  PRINCIPAL AUTHOR      : SLK                                     */
/*  SUBSYSTEM NAME        : DB                                      */
/*  MODULE NAME           : queue                                   */
/*  LANGUAGE              : C                                       */
/*  TARGET ENVIRONMENT    : ANY                                     */
/*  DATE OF FIRST RELEASE : 2012/12/21                              */
/*  DESCRIPTION           : interface of Queue                      */
/********************************************************************/

/*
* Revision log:
*
* Created by SLK,2012/12/21
*
*/


#include"queue.h"
#include<stdio.h>
#include<stdlib.h>

#define SUCCESS 0
#define FAILURE (-1)

/*
* Create a Queue
*/
int QueueCreate(Queue *pQ)
{
    if(pQ == NULL)
        return FAILURE;
        
    pQ->pHead = NULL;
    pQ->pTail = NULL;
    pQ->queueLen = 0;
    return SUCCESS;
}
/*
* Delete a Queue
*/
int QueueDelete(Queue *pQ)
{
    if(pQ == NULL)
        return FAILURE;	
    while(pQ->pHead != NULL)
    {
        QueueNode * p = pQ->pHead;
        pQ->pHead = pQ->pHead->pNext;
        pQ->queueLen--;
        free(p);
    }
    pQ->pHead = NULL;
    pQ->pTail = NULL;
    pQ->queueLen = 0;
    return SUCCESS;	
}
/*
* Put in a Node to Queue
*/
int QueueIn(Queue *pQ, QueueNode *pNode)
{
    if(pQ == NULL || pNode == NULL)
        return FAILURE;	
    pNode->pNext = NULL;
    if(pQ->pHead == NULL)
        pQ->pHead = pNode;
    if(pQ->pTail == NULL)
        pQ->pTail = pNode;
    else
    {
        pQ->pTail->pNext = pNode;
        pQ->pTail = pNode;
    }
    pQ->queueLen++;
    return SUCCESS;	
}
/*
* Get out a Node from Queue
*/
int QueueOut(Queue *pQ, QueueNode **ppNode)
{
    if(pQ == NULL || ppNode == NULL)
        return FAILURE;	
    if(pQ->pHead != NULL)
    {
        *ppNode = pQ->pHead;
        pQ->pHead = pQ->pHead->pNext;
        pQ->queueLen--;
        if(pQ->queueLen == 0)
        {
            pQ->pTail = NULL;	
        }	
        return SUCCESS;	
    }
    return FAILURE;	
}