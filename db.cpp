#include<stdio.h>
#include<sqlite3.h>
#include<iostream>

static int callback(void *NotUsed, int argc, char **argv, char **azColName){
    int i;
    for(i=0; i<argc; i++){
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}



int main(){
    sqlite3 *db1, *db2, *db3, *db4;
    int rc1, rc2, rc3, rc4;
    char *sql;
    char *zErrMsg = 0;
    

    rc1 = sqlite3_open("test1.db", &db1);
    rc2 = sqlite3_open("test2.db", &db2);
    rc3 = sqlite3_open("test3.db", &db3);
    rc4 = sqlite3_open("test4.db", &db4);

    if( rc1 ){
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db1));
	exit(0);
    }else{
	fprintf(stderr, "Opened database successfully\n");
    }

    /* Create SQL statement */
    sql = "CREATE TABLE COMPANY("  \
	  "ID INT PRIMARY KEY     NOT NULL," \
	  "NAME           TEXT    NOT NULL," \
	  "AGE            INT     NOT NULL," \
	  "ADDRESS        CHAR(50)," \
	  "SALARY         REAL );";

     /* Execute SQL statement */
    rc1 = sqlite3_exec(db1, sql, callback, 0, &zErrMsg);
    if( rc1 != SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
	sqlite3_free(zErrMsg);
    }else{
	fprintf(stdout, "Table created successfully\n");
    }
    sqlite3_close(db1);
    sqlite3_close(db2);
    sqlite3_close(db3);
    sqlite3_close(db4);


    return 0;
}
