/********************************************************************/
/* Copyright (C) MC2Lab-USTC, 2012                                  */
/*                                                                  */
/*  FILE NAME             : cmdline.c                               */
/*  PRINCIPAL AUTHOR      : SLK                                     */
/*  SUBSYSTEM NAME        : DB                                      */
/*  MODULE NAME           : cmdline                                 */
/*  LANGUAGE              : C                                       */
/*  TARGET ENVIRONMENT    : Linux                                   */
/*  DATE OF FIRST RELEASE : 2012/12/14                              */
/*  DESCRIPTION           : The cmd of Vampire KV Database system   */
/********************************************************************/

/*
* Revision log:
* Created by SLK,2012/12/14
* Extract cmdline method by SLK,2012/12/21
*
*/

#include "dbapi.h"
#include "cmdline.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

#define CMD_NUM     6
#define MAX_STR_LEN 1024

char cmdbuf[MAX_STR_LEN];
const char *prompt = "Vampire";
char dbname[MAX_STR_LEN] = "\0";
tDatabase db = NULL;

int GetCmd(char * cmdbuf,int size);
int ExecCmd(char * cmdbuf);

int cmdline()
{
    /*start cmdline here*/
    while(1)
    {
        printf("%s::%s>>", prompt, dbname);
        GetCmd(cmdbuf, MAX_STR_LEN);
        ExecCmd(cmdbuf);
    }
    return 0;
}

int GetCmd(char * cmdbuf, int size)
{
    fgets(cmdbuf, size, stdin);
    setbuf(stdin, NULL);
    cmdbuf[strlen(cmdbuf) - 1] = '\0';
    return 0;
}
int CheckCmd(char * cmdbuf, char * pattern)
{
    regex_t reg;
    regmatch_t pmatch[1];
    const size_t nmatch = 1;
    regcomp(&reg, pattern, REG_EXTENDED | REG_ICASE | REG_NOSUB);
    if (regexec(&reg, cmdbuf, nmatch, pmatch, 0) == 0)
    {
        regfree(&reg);
        return 0;
    }
        
}
int ExecCmd(char * cmdbuf)
{
    char temp[MAX_STR_LEN] = "\0";
    if(CheckCmd(cmdbuf,"help") == 0 )
    {
        printf("===========================================================================\n");
        printf("CMDs Guide:\n");
        printf("\"open [dbname].hdb\" : to open a hash database.\n");
        printf("\"set [key] [value]\" : to add a key-value record to database.\n");
        printf("\"get [key]\"         : to get a record by key from database.\n");
        printf("\"del [key]\"         : to delete a record by key from database.\n");
        printf("\"close\"             : to close the opened database.\n");
        printf("\"exit\"              : to exit this application.\n");
        printf("Remember, you should open a database first!\n");
        printf("===========================================================================\n");
    }
    else if(CheckCmd(cmdbuf, "open\\s*\\w*") == 0)
    {
        if(strlen(dbname) > 0)
        {
            printf("Please close %s first.\n", dbname);
        }
        else
        {
            sscanf(cmdbuf, "%s%s", temp, dbname);
            char *pdbname = dbname;
            pdbname = strcat(pdbname, ".hdb");
            db = CreateDB(dbname);
            //printf("db: %d\n", db);
        }
    }
    else if(db == NULL && CheckCmd(cmdbuf, "set|get|del|close") == 0)
    {
        printf("Please open a database file first.\n");
    }
    else if(CheckCmd(cmdbuf, "close") == 0)
    {
        if(db != NULL)
        {
            CloseDB(db);
            db = NULL;
        }
        dbname[0] = '\0';
    }
    else if(CheckCmd(cmdbuf, "exit") == 0)
    {
        if(db != NULL)
        {
            CloseDB(db);
            dbname[0] = '\0';
        }
        exit(0);
    }
    else if(CheckCmd(cmdbuf, "set\\s*[0-9]+\\s*.*") == 0)
    {
        tKey key;
        tValue value;
        char str[MAX_STR_LEN] = "\0";
        sscanf(cmdbuf,"%s%d%s", temp, &key, str);
        value.str = strstr(cmdbuf, str);
        value.len = strlen(value.str);          //cmdbuf + MAX_STR_LEN - value.str;
        if(SetRecord(db, key, value) != 0)
        {
            printf("ERROR:set %d %s\n", (int)key, value.str);
        }
        //printf("set %d %s\n",(int)key,value.str);
    }
    else if(CheckCmd(cmdbuf, "get\\s*[0-9]+") == 0)
    {
        tKey key = -1;
        tValue value;
        char str[MAX_STR_LEN] = "\0";
        value.str = str;
        value.len = MAX_STR_LEN;
        sscanf(cmdbuf, "%s%d", temp, &key);
        if(GetRecord(db, key, &value) == 0)
        {
            printf("%d -> %s\n", key, value.str);
        }
        else
        {
            printf("ERROR:get %d Not found!\n", (int)key);
        }
    }
    else if(CheckCmd(cmdbuf, "del\\s*[0-9]+") == 0)
    {
        tKey key = -1;
        tValue value;
        char str[MAX_STR_LEN] = "\0";
        value.str = str;
        value.len = MAX_STR_LEN;
        sscanf(cmdbuf, "%s%d", temp, &key);
        if(DeleteRecord(db, key) != 0)
        {
            printf("ERROR:delete %d\n", (int)key);
        }
    }
    else
    {
        printf("Unknow Command!\n");
    }
    return 0;
}