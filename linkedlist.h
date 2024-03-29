/********************************************************************/
/* Copyright (C) SSE-USTC, 2012-2013                                */
/*                                                                  */
/*  FILE NAME             :  linkedList.h                           */
/*  PRINCIPAL AUTHOR      :  SLK                                    */
/*  SUBSYSTEM NAME        :  LinkedList                             */
/*  MODULE NAME           :  LinkedList                             */
/*  LANGUAGE              :  C                                      */
/*  TARGET ENVIRONMENT    :  ANY                                    */
/*  DATE OF FIRST RELEASE :  2013/1/2                               */
/*  DESCRIPTION           :  interface of Linked List               */
/********************************************************************/

/*
 * Revision log:
 *
 * Created by SLK,2013/1/2
 *
 */

#ifndef _LINKED_LIST_H_
#define _LINKED_LIST_H_


/*
 * LinkedList Node Type
 */
typedef struct LNode
{
	struct LNode * pNext;
}tLNode;

/*
 * LinkedList Type
 */
typedef struct LinkedList
{
	tLNode *pHead;
	tLNode *pTail;
	int		len;
	
}tLinkedList;

typedef tLNode* (*SearchFunc)(tLinkedList *pLinkedList, tLNode *pNode);
/*
 * Create a LinkedList
 */
tLinkedList *CreateLinkedList();
/*
 * Delete a LinkedList
 */
int DeleteLinkedList(tLinkedList *pLinkedList);
/*
 * Add a LNode to LinkedList
 */
int AddLNode(tLinkedList *pLinkedList, tLNode *pNode);
/*
 * Delete a LNode from LinkedList
 */
int DelLNode(tLinkedList *pLinkedList, tLNode * pNode);
/*
 * Search a LNode from LinkedList
 * tLNode * SearchFunc(tLinkedList *pLinkedList, tLNode *pNode)
 */
tLNode *SearchLNode(tLinkedList *pLinkedList, tLNode *pNode, SearchFunc Search);
/*
 * get LinkTableHead
 */
tLNode *GetLHead(tLinkedList *pLinkedList);
/*
 * get next LNode
 */
tLNode *GetNextLNode(tLinkedList *pLinkedList, tLNode *pNode);

#endif /* _LINKED_LIST_H_ */


