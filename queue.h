/********************************************************************/
/* Copyright (C) SSE-USTC, 2010                                     */
/*                                                                  */
/*  FILE NAME             : queue.h                                 */
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

#ifndef _QUEUE_H_
#define _QUEUE_H_


/*
* Queue Node Type
*/
typedef struct Node
{
    struct Node * pNext;
}QueueNode;

/*
* Queue Type
*/
typedef struct Queue
{
    QueueNode *pHead;
    QueueNode *pTail;
    int	queueLen;
}Queue;

/*
* Create a Queue
*/
int QueueCreate(Queue *pQ);
/*
* Delete a Queue
*/
int QueueDelete(Queue *pQ);
/*
* Put in a Node to Queue
*/
int QueueIn(Queue *pQ, QueueNode *pNode);
/*
* Get out a Node from Queue
*/
int QueueOut(Queue *pQ, QueueNode **ppNode);

#endif /* _QUEUE_H_ */

