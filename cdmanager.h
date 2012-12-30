/********************************************************************/
/* Copyright (C) MC2Lab-USTC, 2012                                  */
/*                                                                  */
/*  FILE NAME             : cdmanager.h                             */
/*  PRINCIPAL AUTHOR      : SLK                                     */
/*  SUBSYSTEM NAME        : network                                 */
/*  MODULE NAME           : cdmanager                               */
/*  LANGUAGE              : C                                       */
/*  TARGET ENVIRONMENT    : Linux                                   */
/*  DATE OF FIRST RELEASE : 2012/12/24                              */
/*  DESCRIPTION           : relevant sockfd to tchdb                */
/********************************************************************/

/*
* Revision log:
*
* Created by SLK,2012/12/24
*
*/

/*
* store cliend fd and database mapping table
*/
tDatabase CDManager;
#define InitCDManager()     CDManager = CreateMDB();
#define ShutCDManager()     CloseMDB(CDManager);
#define AttachCD(c, mdb)                                   \
        tValue v;                                          \
        v.str = (char*)&mdb;                               \
        v.len = sizeof(tDatabase);                         \
        SetRecordMDB(CDManager, c, v);

#define DettachCD(c)                                       \
        DeleteRecordMDB(CDManager, c);

#define GetMdb(c, mdb)                                     \
        tValue v;                                          \
        v.str = (char*)&mdb;                               \
        v.len = sizeof(tDatabase);                         \
        GetRecordMDB(CDManager, c, &v);
        