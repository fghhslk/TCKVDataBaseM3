/********************************************************************/
/* Copyright (C) SSE-USTC, 2012                                     */
/*                                                                  */
/*  FILE NAME             :  dbapi.c                                */
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

#include <tcutil.h>
#include <tchdb.h>
#include <string.h>
#include "dbapi.h"

TCMDB *allOpenedDB = NULL;
pthread_mutex_t dbmutex;

typedef struct OpenedDB
{
    TCHDB *hdb;
    int counter;
}tOpenedDB;

/* Public Functions */
/*
 * Create a database and open a specific database               
 * input  : database filename
 * output : none
 * in/out : none
 * return : tcDatebase
 */
tDatabase CreateDB(const char *dbName)
{
    int ecode;
    TCHDB *hdb;
    if (dbName == NULL)
    {
        dbName = "vampire.hdb";
    }
    if (allOpenedDB == NULL)
    {
        allOpenedDB = tcmdbnew();
        pthread_mutex_init(&dbmutex, NULL);
    }
    else
    {
        int sp = -1;
        pthread_mutex_lock(&dbmutex);
        tOpenedDB *opendb = (tOpenedDB*)tcmdbget(allOpenedDB, (void*)dbName, strlen(dbName), &sp);
        if (opendb != NULL)
        {
            hdb = opendb->hdb;
            opendb->counter++;
            tcmdbput(allOpenedDB, (void*)dbName, strlen(dbName), (void*)opendb, sp);
            pthread_mutex_unlock(&dbmutex);
            free(opendb);
            return (tDatabase)hdb;
        }
        pthread_mutex_unlock(&dbmutex);
    }
    hdb = tchdbnew();
    /* set mutual exclusion control of a hash database object for threading. */
    tchdbsetmutex(hdb);
    /* open the database */
    if (!tchdbopen(hdb, dbName, HDBOWRITER | HDBOCREAT | HDBONOLCK))
    {
        ecode = tchdbecode(hdb);
        fprintf(stderr, "open error: %s\n", tchdberrmsg(ecode));
        exit(-1);
    }
    tOpenedDB db;
    db.hdb = hdb;
    db.counter = 1;
    tcmdbput(allOpenedDB, (void*)dbName, strlen(dbName), (void*)&db, sizeof(tOpenedDB));
    return (tDatabase)hdb;
}

/* 
 * close database
 * input  : database object
 * output : none
 * in/out : none
 * return : SUCCESS(0)/FAILURE(-1)
 */  
int CloseDB(tDatabase db)
{
    TCHDB *hdb = (TCHDB*)db;
    int i;
    long sum = (long)tcmdbrnum(allOpenedDB);
    tcmdbiterinit(allOpenedDB);
    for (i = 0; i < sum; i++)
    {
        int ksize = -1;
        char *dbName = (char*)tcmdbiternext(allOpenedDB, &ksize);
        if (dbName == NULL)
        {
            fprintf(stderr, "DBDelete:Can't find the database,%s:%d\n", __FILE__, __LINE__);
            break;
        }
        int sp = -1;
        pthread_mutex_lock(&dbmutex);
        tOpenedDB *opendb = (tOpenedDB*)tcmdbget(allOpenedDB, (void*)dbName, ksize, &sp);
        if (opendb != NULL && opendb->hdb == hdb)
        {
            opendb->counter--;
            if (opendb->counter <= 0)
            {
                /* remove this record */
                tcmdbout(allOpenedDB, (void*)dbName, ksize);
                pthread_mutex_unlock(&dbmutex);
                free(dbName);
                free(opendb);
                break;
            }
            /* update this record counter -- */
            tcmdbput(allOpenedDB, (void*)dbName, ksize, (void*)opendb, sp);
            pthread_mutex_unlock(&dbmutex);
            free(dbName);
            free(opendb);
            return SUCCESS;
        }
        pthread_mutex_unlock(&dbmutex);
        free(dbName);
        free(opendb);
    }
   
    int ecode;
    if (!tchdbclose(hdb))
    {
        ecode = tchdbecode(hdb);
        fprintf(stderr, "close error: %s\n", tchdberrmsg(ecode));
        return FAILURE;
    }
    tchdbdel(hdb);
    return SUCCESS;
}

/* 
 * set a record to database
 * input  : database object, tcKey, tcValue
 * output : none
 * in/out : none
 * return : SUCCESS(0)/FAILURE(-1)
 */  
int SetRecord(tDatabase hdb, tKey key, tValue value)
{
    int ecode;
    if (!tchdbput((TCHDB*)hdb, &key, sizeof(tKey), value.str, value.len))
    {
        ecode = tchdbecode((TCHDB*)hdb);
        fprintf(stderr, "put error: %s\n", tchdberrmsg(ecode));
        return FAILURE;
    } 
    //printf("set successfully!\n");
    return SUCCESS;  
}

/*
 * get a record from database
 * input  : database object, tcKey, tcValue*
 * output : none
 * in/out : tcValue;
 * return : SUCCESS(0)/FAILURE(-1)
 */ 
int GetRecord(tDatabase hdb, tKey key, tValue *pvalue)
{
    int ecode;
    int ret;

    ret = tchdbget3(hdb, &key, sizeof(tKey), pvalue->str, pvalue->len);
    printf("%d\n", ret);
    if (ret != -1)
    {
        pvalue->str[ret] = '\0';
        pvalue->len = ret;
        return SUCCESS;
    }	
    ecode = tchdbecode(hdb);
    fprintf(stderr, "get error: %s\n", tchdberrmsg(ecode));
    
    return FAILURE;  
}

/* 
 * delete a record by key 
 * input  : database object, tcKey
 * output : none
 * in/out : hdb
 * return : SUCCESS(0)/FAILURE(-1)
 */  
int DeleteRecord(tDatabase hdb, tKey key)
{
    int ecode;
    if (!tchdbout((TCHDB*)hdb, &key, sizeof(tKey)))
    {
        ecode = tchdbecode(hdb);
        fprintf(stderr, "delete error: %s\n", tchdberrmsg(ecode));
        return FAILURE;
    }
    return SUCCESS;
}

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
tDatabase CreateMDB()
{
    TCMDB * mdb = tcmdbnew();
    return (tDatabase)mdb;
}

/*
* Delete the Database
* input  : tDatabase db
* output : None
* in/out : None
* return : SUCCESS(0)/exit(-1)
*/
int CloseMDB(tDatabase mdb)
{
    tcmdbdel((TCMDB*)mdb);
    return SUCCESS;
}

/*
* Set key/value
* input  : tKey key, tValue value - one key/value
* output : None
* in/out : None
* return : SUCCESS(0)/FAILURE(-1)
*/	
int SetRecordMDB(tDatabase mdb, tKey key, tValue value)
{
    tcmdbput((TCMDB*)mdb,( void*)&key, sizeof(tKey), value.str, value.len);
    return SUCCESS;
}

/*
* get key/value
* input  : tKey key
* output : None
* in/out : tValue *pvalue MUST BE initialized,it means pvalue->str is malloced,
: and pvalue->len is the length of pvalue->str
: if return SUCCESS(0),value will stored in pvalue(str,len).
* return : SUCCESS(0)/FAILURE(-1)
*/
int GetRecordMDB(tDatabase mdb, tKey key, tValue *pvalue)
{
    int sp = -1;
    char *v = tcmdbget((TCMDB*)mdb, (void*)&key, sizeof(tKey), &sp);
    if (v != NULL && sp > 0 && sp <= pvalue->len)
    {
        memcpy(pvalue->str, v, sp);
        pvalue->len = sp;
        return SUCCESS;
    }
    return FAILURE;
}

/*
* delete key/value
* input  : tKey key
* output : None
* in/out : None
* return : SUCCESS(0)/FAILURE(-1)
*/
int DeleteRecordMDB(tDatabase mdb, tKey key)
{
    tcmdbout((TCMDB*)mdb, (void*)&key, sizeof(tKey));
    return SUCCESS;
}


