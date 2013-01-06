/********************************************************************/
/* Copyright (C) SSE-USTC, 2012                                     */
/*                                                                  */
/*  FILE NAME             : net.c                                   */
/*  PRINCIPAL AUTHOR      : SLK                                     */
/*  SUBSYSTEM NAME        : network                                 */
/*  MODULE NAME           : net                                     */
/*  LANGUAGE              : C                                       */
/*  TARGET ENVIRONMENT    : Linux                                   */
/*  DATE OF FIRST RELEASE : 2012/12/14                              */
/*  DESCRIPTION           : the interface to Linux system(socket)   */
/********************************************************************/

/*
* Revision log:
*
* Created by SLK,2012/12/14
* Support epoll,by SLK,2012/12/19
*
*/

#ifndef _NET_H_
#define _NET_H_

#define MAX_CONNECT_QUEUE   4
#define MAX_CONNECT_FD      1024
typedef int tServiceHandler;
/*
* InitServer
* input : addr - server ip address
* input : port - server port to open
* output : None
* in/out : None
* return : if SUCCESS return 0
* : if FAILURE return (-1)
*/
int InitializeNetService(char *addr, short int port);
int ShutdownNetService();
/*
* OpenRemoteService - Only used in Client side,it connects Server.
* input : addr - server ip address
* input : port - server port to open
* output : None
* in/out : None
* return : if SUCCESS return tServiceHandler
* : if FAILURE return (-1)
*/
tServiceHandler OpenRemoteService(char *addr, short int port);
/*
* CloseRemoteService - Only used in Client side,it terminate connection.
* input : h - it MUST BE what OpenRemoteService returns.
* output : None
* in/out : None
* return : if SUCCESS return 0
* : if FAILURE return (-1)
*/
int CloseRemoteService(tServiceHandler h);
/*
* ServiceStart - Only used in Sever side,when client connects it return.
* input : None
* output : None
* in/out : None
* return : if SUCCESS return tServiceHandler
* : if FAILURE return (-1)
*/
tServiceHandler ServiceStart();
/*
* ServiceStop - Only used in Sever side,when client connects it return.
* input : h - it MUST BE what ServiceStart OR returns.
* output : None
* in/out : None
* return : if SUCCESS return 0
* : if FAILURE return (-1)
*/
int ServiceStop(tServiceHandler h);
/*
* SendData - send data to peer side
* input : h - it MUST BE what ServiceStart or OpenRemoteService returns.
* input : pBuf - point to data
* input : BufSize - the length of data bytes
* output : None
* in/out : None
* return : if SUCCESS return BufSize sended
* : if FAILURE return (-1)
* : if Connection Terminals return 0
*/
int SendData(tServiceHandler h, char *pBuf, int BufSize);
/*
* SendData - send data to peer side
* input : h - it MUST BE what ServiceStart or OpenRemoteService returns.
* in/out : pBuf - point to memory that will store recved data
* in/out : pBufSize - the max length of data to recv
* return : if SUCCESS return BufSize recved
* : if FAILURE return (-1)
* : if Connection Terminals return 0
*/
int RecvData(tServiceHandler h, char *pBuf, int *pBufSize);
/*
* GetSelfAddr - get local IP address
* input : None
* output : None
* in/out : None
* return : IP address
*/
char* GetSelfAddr();
        
#endif /* _NET_H_ */