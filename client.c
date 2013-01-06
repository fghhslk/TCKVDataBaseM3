/********************************************************************/
/* Copyright (C) MC2Lab-USTC, 2012                                  */
/*                                                                  */
/*  FILE NAME             : client.c                                */
/*  PRINCIPAL AUTHOR      : SLK                                     */
/*  SUBSYSTEM NAME        : DB                                      */
/*  MODULE NAME           : client                                  */
/*  LANGUAGE              : C                                       */
/*  TARGET ENVIRONMENT    : Linux                                   */
/*  DATE OF FIRST RELEASE : 2012/12/21                              */
/*  DESCRIPTION           : The client of Nezha KV Database system  */
/********************************************************************/

/*
* Revision log:
* Created by SLK,2012/12/21
*
*/

#include "cmdline.h"
#include <stdio.h>

int main()
{
    printf("***************************************************************************\n");
    printf("****     Welcome to Vampire KV Database system!      ----version 5.0   ****\n");
    printf("***************************************************************************\n");
    printf("Please type CMDs: (You can type in [help] for help)\n");
    
    cmdline();
    return 0;
}