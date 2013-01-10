/********************************************************************/
/* Copyright (C) MC2Lab-USTC, 2012                                  */
/*                                                                  */
/*  FILE NAME               :  engine.c                             */
/*  PRINCIPAL AUTHOR        :  SLK                                  */
/*  SUBSYSTEM NAME          :  main                                 */
/*  MODULE NAME             :  engine                               */
/*  LANGUAGE                :  C                                    */
/*  TARGET ENVIRONMENT      :  Linux                                */
/*  DATE OF FIRST RELEASE   :  2012/1/4                             */
/*  DESCRIPTION             :  Impement of Socket Server Engine     */
/*                             to handle clients requests.          */
/********************************************************************/

/*
 * Revision log:
 *
 * Created by SLK,2012/1/4
 *
 */

#ifndef _ENGINE_H_
#define _ENGINE_H_

/*
 * Server Engine for Clients' Requests
 */
int ServiceEngine(char *addr, int port);
/* create cluster in master */
int CreateCluster(char *addr, int port);
/* loading cluster nodes in list */
int LoadingClusterNodes(char *addr, int port);
/* unloading cluster nodes from list */
int UnLoadingClusterNodes(char *addr, int port);

#endif /* _ENGINE_H_ */


