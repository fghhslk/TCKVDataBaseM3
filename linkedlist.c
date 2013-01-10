/********************************************************************/
/* Copyright (C) SSE-USTC, 2012-2013                                */
/*                                                                  */
/*  FILE NAME             :  linkedList.h                           */
/*  PRINCIPAL AUTHOR      :  SLK                                    */
/*  SUBSYSTEM NAME        :  LinkedList                             */
/*  MODULE NAME           :  LinkedList                             */
/*  LANGUAGE              :  C                                      */
/*  TARGET ENVIRONMENT    :  ANY                                    */
/*  DATE OF FIRST RELEASE :  2012/1/2                               */
/*  DESCRIPTION           :  interface of Linked List               */
/********************************************************************/

/*
 * Revision log:
 *
 * Created by SLK,2012/1/2
 *
 */

#include "linkedlist.h"
#include <stdio.h>
#include <stdlib.h>

#define SUCCESS 0
#define FAILURE -1

/*
 * Create a LinkedList
 */
tLinkedList *CreateLinkedList()
{
	tLinkedList *pLinkedList = (tLinkedList*) malloc(sizeof(tLinkedList));
	if(pLinkedList == NULL)
	{
		return NULL;
    }
	pLinkedList->pHead = NULL;
	pLinkedList->pTail = NULL;
	pLinkedList->len = 0;
	return pLinkedList;
}
/*
 * Delete a LinkedList
 */
int DeleteLinkedList(tLinkedList *pLinkedList)
{
	if (pLinkedList == NULL)
	{
		return FAILURE;
    }
	while (pLinkedList->pHead != NULL)
	{
		tLNode * p = pLinkedList->pHead;
		pLinkedList->pHead = pLinkedList->pHead->pNext;
		pLinkedList->len -= 1 ;
		free(p);
	}
	pLinkedList->pHead = NULL;
	pLinkedList->pTail = NULL;
	pLinkedList->len = 0;
	free(pLinkedList);
	return SUCCESS;		
}
/*
 * Add a LNode to LinkedList
 */
int AddLNode(tLinkedList *pLinkedList, tLNode *pNode)
{
	if (pLinkedList == NULL || pNode == NULL)
	{
		return FAILURE;
    }
	pNode->pNext = NULL;
	if (pLinkedList->pHead == NULL)
	{
		pLinkedList->pHead = pNode;
	}
	if (pLinkedList->pTail == NULL)
	{
		pLinkedList->pTail = pNode;
	}
	else
	{
		pLinkedList->pTail->pNext = pNode;
		pLinkedList->pTail = pNode;
	}
	pLinkedList->len++;
	return SUCCESS;		
}
/*
 * Delete a LNode from LinkedList
 */
int DelLNode(tLinkedList *pLinkedList, tLNode *pNode)
{
	if (pLinkedList == NULL || pNode == NULL)
	{
		return FAILURE;
    }
    if (pLinkedList->pHead == pNode)
    {
        pLinkedList->pHead = pLinkedList->pHead->pNext;
    	pLinkedList->len--;
    	if( pLinkedList->len == 0)
    	{
    		pLinkedList->pTail = NULL;	
    	}
    	return SUCCESS;
    }
    tLNode *pTempNode = pLinkedList->pHead;
	while (pTempNode != NULL)
    {    
		if (pTempNode->pNext == pNode)
		{
		    pTempNode->pNext = pTempNode->pNext->pNext;
    		pLinkedList->len--;
    		if (pLinkedList->len == 0)
    		{
    			pLinkedList->pTail = NULL;	
    		}
    		return SUCCESS;				    
		}
		pTempNode = pTempNode->pNext;
	}
	return FAILURE;		
}

/*
 * Search a LNode from LinkedList
 * tLNode * CmpFn(tLinkedList *pLinkedList);
 */
tLNode *SearchLNode(tLinkedList *pLinkedList, tLNode *pNode, SearchFunc Search)
{
    if (Search == NULL)
	{
		return NULL;
    }
    return Search(pLinkedList, pNode);
}

/*
 * get LinkedListHead
 */
tLNode *GetLHead(tLinkedList *pLinkedList)
{
    if (pLinkedList == NULL)
	{
		return NULL;
    }    
	return pLinkedList->pHead;
}

/*
 * get next LNode
 */
tLNode *GetNextLNode(tLinkedList *pLinkedList, tLNode * pNode)
{
	if (pLinkedList == NULL || pNode == NULL)
	{
		return NULL;
    }
    tLNode *pTempNode = pLinkedList->pHead;
	while (pTempNode != NULL)
    {    
		if (pTempNode == pNode)
        {
    		return pTempNode->pNext;				    
		}
		pTempNode = pTempNode->pNext;
	}
	return NULL;
}

