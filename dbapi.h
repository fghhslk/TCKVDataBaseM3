/********************************************************************/
/* Copyright (C) SSE-USTC, 2012                                     */
/*                                                                  */
/*  FILE NAME             :  dbapi.h                                */
/*  PRINCIPAL AUTHOR      :  SLK                                    */
/*  SUBSYSTEM NAME        :  system                                 */
/*  MODULE NAME           :  dbapi                                  */
/*  LANGUAGE              :  C                                      */
/*  TARGET ENVIRONMENT    :  Linux                                  */
/*  DATE OF FIRST RELEASE :  2012/11/28                             */
/*  DESCRIPTION           :  the interface to Tokyo Cabinet(tc)     */
/********************************************************************/

/*
 * Revision log:
 *
 * Created by SLK,2012/11/28
 * Support Memory Database(no file) Operations,by SLK,2012/12/19
 *
 */

#ifndef _DBAPI_H_
#define _DBAPI_H_

/* Public Macro */
#define SUCCESS     0
#define FAILURE     -1

typedef void* tDatabase;
typedef unsigned int tKey;
typedef struct Value{
    int len;                    /* Length of string */
    char *str;                  /* Used for Value Content */
}tValue;

/* Public Functions */
/*
 * Create a database and open a specific database               
 * input  : database filename
 * output : none
 * in/out : none
 * return : tcDatebase
 */
tDatabase CreateDB(const char*);

/* 
 * close database
 * input  : database object
 * output : none
 * in/out : none
 * return : SUCCESS(0)/FAILURE(-1)
 */                         
int CloseDB(tDatabase);

/* 
 * set a record to database
 * input  : database object, tcKey, tcValue
 * output : none
 * in/out : none
 * return : SUCCESS(0)/FAILURE(-1)
 */                            
int SetRecord(tDatabase, tKey, tValue);

/*
 * get a record from database
 * input  : database object, tcKey, tcValue*
 * output : none
 * in/out : tcValue;
 * return : SUCCESS(0)/FAILURE(-1)
 */               
int GetRecord(tDatabase, tKey, tValue*);

/* 
 * delete a record by key 
 * input  : database object, tcKey
 * output : none
 * in/out : hdb
 * return : SUCCESS(0)/FAILURE(-1)
 */                        
int DeleteRecord(tDatabase hdb, tKey key);                

/**********************************************/
/* Memory Database(no file) Operations
/**********************************************/
/*
* Create an Memory Database
* input  : None
* output : None
* in/out : None
* return : if SUCCESS return (tDatabase *)Database handler
* : if FAILURE exit(-1)
*/
tDatabase CreateMDB();

/*
* Delete the Database
* input  : tDatabase db
* output : None
* in/out : None
* return : SUCCESS(0)/exit(-1)
*/
int CloseMDB(tDatabase mdb);


/*
* Set key/value
* input  : tKey key,tValue value - one key/value
* output : None
* in/out : None
* return : SUCCESS(0)/FAILURE(-1)
*/	
int SetRecordMDB(tDatabase mdb, tKey key, tValue value);

/*
* get key/value
* input  : tKey key
* output : None
* in/out : tValue *pvalue MUST BE initialized,it means pvalue->str is malloced,
: and pvalue->len is the length of pvalue->str
: if return SUCCESS(0),value will stored in pvalue(str,len).
* return : SUCCESS(0)/FAILURE(-1)
*/
int GetKRecordMDB(tDatabase mdb, tKey key, tValue *pvalue);

/*
* delete key/value
* input  : tKey key
* output : None
* in/out : None
* return : SUCCESS(0)/FAILURE(-1)
*/
int DeleteRecordMDB(tDatabase mdb, tKey key);

#endif
