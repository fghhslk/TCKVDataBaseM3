
/********************************************************************/
/* Copyright (C) SSE-USTC, 2012-2013                                */
/*                                                                  */
/*  FILE NAME             :  testlinkedlist.c                       */
/*  PRINCIPAL AUTHOR      :  SLK                                    */
/*  SUBSYSTEM NAME        :  LinkedList                             */
/*  MODULE NAME           :  LinkedList                             */
/*  LANGUAGE              :  C                                      */
/*  TARGET ENVIRONMENT    :  ANY                                    */
/*  DATE OF FIRST RELEASE :  2012/1/2                               */
/*  DESCRIPTION           :  test of Linked List                    */
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

#define debug   

typedef struct Node
{
	tLNode *pNext;
	int data;
}tNode;

tLNode *Search(tLinkedList *pLinkedList);

int main()
{
    int i;
    tLinkedList *pLinkedList = CreateLinkedList();
    for (i = 0; i < 10; i++)
    {
        tNode *pNode = (tNode*) malloc(sizeof(tNode));
        pNode->data = i;
        debug("AddLinkedListNode\n");
        AddLNode(pLinkedList, (tLNode*)pNode);
    }
    debug("SearchLinkedListNode\n");
    tNode *pTempNode = (tNode*)SearchLNode(pLinkedList,Search);
    printf("%d\n", pTempNode->data);
    debug("DelLNode\n");
    DelLNode(pLinkedList, (tLNode*)pTempNode);
    free(pTempNode);
    DeleteLinkedList(pLinkedList);
}

tLNode *Search(tLinkedList *pLinkedList)
{
    debug("Search GetLinkedListHead\n");
    tNode *pNode = (tNode*)GetLHead(pLinkedList);
    while (pNode != NULL)
    {
        if (pNode->data == 5)
        {
            return  (tLNode*)pNode;  
        }
        debug("GetNextLNode\n");
        pNode = (tNode*)GetNextLNode(pLinkedList, (tLNode*)pNode);
    }
    return NULL;
}