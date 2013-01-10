/********************************************************************/
/* Copyright (C) MC2Lab-USTC, 2012                                  */
/*                                                                  */
/*  FILE NAME             : server.c                                */
/*  PRINCIPAL AUTHOR      : SLK                                     */
/*  SUBSYSTEM NAME        : network                                 */
/*  MODULE NAME           : server                                  */
/*  LANGUAGE              : C                                       */
/*  TARGET ENVIRONMENT    : Linux                                   */
/*  DATE OF FIRST RELEASE : 2012/12/14                              */
/*  DESCRIPTION           : Impement of Socket Server Engine        */
/*  to handle clients reqs.                                         */
/********************************************************************/

/*
* Revision log:
*
* Created by SLK,2012/12/14
* Support pthread,by SLK,2012/12/19
*/

#include "dbapi.h"
#include "net.h"
#include "protocol.h"
#include "event.h"
#include "queue.h"
#include "cmdline.h"
#include "engine.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>
#include <time.h>

#define PORT                6000
#define IP_ADDR             "127.0.0.1"

#define debug           printf

int main(int argc, char *argv[])
{
    printf("***************************************************************************\n");
    printf("****     Welcome to Vampire KV Database system!      ----version 5.0   ****\n");
    printf("***************************************************************************\n");
    char *addr;
    int port;
    if (argc != 2) 
    {
        addr = IP_ADDR;
        port = PORT;
        CreateCluster(addr, port);
        ServiceEngine(addr, port);
    }
    else
    {
    	int i;
	    addr = GetSelfAddr();
	    //port = atoi(argv[1]);
	    port = 5000;
	    for (i = 0; i < 100; i++)
	    {
	    	LoadingClusterNodes(addr, port + i);
	    	if (ServiceEngine(addr, port + i) == -1)
	    		/* Delete Wrong Server Node */
	    		UnLoadingClusterNodes(addr, port + i);
		}
	}
    /* start command line console */
    /*pthread_t cmdline_id;
    if(pthread_create(&cmdline_id, NULL, (void*)cmdline, (void*)0) != 0)
    {
        fprintf(stderr, "cmdline pthread_create Error,%s:%d\n", __FILE__, __LINE__);
        exit(-1);
    }*/
    //ServiceEngine(addr, port);
    return 0;
}