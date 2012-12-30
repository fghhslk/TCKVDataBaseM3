/********************************************************************/
/* Copyright (C) MC2Lab-USTC, 2012                                  */
/*                                                                  */
/*  FILE NAME             :  remotedbapi.h                          */
/*  PRINCIPAL AUTHOR      :  SLK                                    */
/*  SUBSYSTEM NAME        :  DB                                     */
/*  MODULE NAME           :  remotedbapi                            */
/*  LANGUAGE              :  C                                      */
/*  TARGET ENVIRONMENT    :  ANY                                    */
/*  DATE OF FIRST RELEASE :  2012/12/30                             */
/*  DESCRIPTION           :  Remote Interface of NoSQL DB API       */
/********************************************************************/

/*
 * Revision log:
 *
 * Created by SLK,2012/12/30
 *
 */

#ifndef _REMOTE_DB_API_H_
#define _REMOTE_DB_API_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "dbapi.h" /* use data types */

/**********************************************/
/* Database(have a file) Operations
/**********************************************/
/*
 * Create an Database
 * input	: filename,addr,port
 * output	: None
 * in/out	: None
 * return	: if SUCCESS return (int)Database handler
 *          : if FAILURE return (-1)
 */
int CreateRemoteDB(char *addr, int port, const char *dbName);
	

/*
 * Delete the Database
 * input	: int db
 * output	: None
 * in/out	: None
 * return	: SUCCESS(0)/exit(-1)
 */
int CloseRemoteDB(int db);


/*
 * Set key/value
 * input	: tKey key,tValue value - one key/value
 * output	: None
 * in/out	: None
 * return	: SUCCESS(0)/FAILURE(-1)
 */	
int SetRecordRemote(int db, tKey key, tValue value);

/*
 * get key/value
 * input	: tKey key
 * output	: None
 * in/out	: tValue *pvalue MUST BE initialized,it means pvalue->str is malloced,
            : and pvalue->len is the length of pvalue->str 
            : if return SUCCESS(0),value will stored in pvalue(str,len).
 * return	: SUCCESS(0)/FAILURE(-1)
 */
int GetRecordRemote(int db, tKey key, tValue *pvalue);

/*
 * delete key/value
 * input	: tKey key
 * output	: None
 * in/out	: None
 * return	: SUCCESS(0)/FAILURE(-1)
 */
int DeleteRecordRemote(int db, tKey key);



#ifdef __cplusplus
}
#endif
#endif /* _REMOTE_DB_API_H_ */


