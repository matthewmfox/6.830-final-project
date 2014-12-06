#include<stdio.h>
#include<sqlite3.h>
#include<iostream>
#include<map>

int numberSeconds;


class Partition {
    public:
       Partition(int pID, sqlite3 * dBase, int tSize);
       void lock();
       void unlock(); 
       int maxTableSize;
       bool locked;
       sqlite3 * db;
       int partitionID;
};

Partition::Partition(int pID, sqlite3 * dBase, int tSize){
    this->maxTableSize = tSize;
    this->db = dBase;
    this->partitionID = pID;
    this->locked = false;
}

Partition::lock(){
    this->locked = true;
}

Partition::unlock(){
    this->locked = false;
}

map<int, Partition> pMap;

void createPartition(int partitionID, string partitionName, sqlite3 * db, int maxTableSize){
    int message;
    message = sqlite3_open(partitionName, &db);
    if( message ){
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
	exit(0);
    }else{
	fprintf(stderr, "Opened database successfully\n");
    }
    Partition p(partitionID, db, maxTableSize);
    pMap[partitionID] = p;
}

void createDB(int numberOfPartitions, int numberSeconds, int tSize){
    for(int i = 0; i< numberOfPartitions; i++){
	string pName = "partition" + to_string(i);
	string dName = "db" + to_string(i);
        createPartition(i, pName, dName, tSize);
    }
    numberSeconds = numSeconds;
}

/* Function called to execute query */
void execQuery(string SQLquery, int tableID, sqlite3 db1){
    int rc1;
    rc1 = sqlite3_exec(db1, SQLquery, callback, 0, &zErrMsg);

    if( rc1 != SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
    }else{
    fprintf(stdout, "Query executed successfully\n");
    }

}

/* Close connection to SQLite3 database */
void closeTable(sqlite3 db){
    sqlite3_close(db);
}


void lockPartition(int partitionID){
    pMap[partitionID].lock();
}

void unlockPartition(int partitionID){
    pMap[partitionID].unlock();
}


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
    sql = "CREATE TABLE COMPANY(" \
	  "ID INT PRIMARY KEY     NOT NULL," \
	  "NAME           TEXT    NOT NULL," \
	  "AGE            INT     NOT NULL," \
	  "ADDRESS        CHAR(50)," \
	  "SALARY         REAL );";

     /* Execute SQL statement */
    rc1 = sqlite3_exec(db1, sql, callback, 0, &zErrMsg);
    rc2 = sqlite3_exec(db2, sql, callback, 0, &zErrMsg);
    rc3 = sqlite3_exec(db3, sql, callback, 0, &zErrMsg);
    rc4 = sqlite3_exec(db4, sql, callback, 0, &zErrMsg);
    if( rc1 != SQLITE_OK ){
        fprintf(stderr, "SQL error 1: %s\n", zErrMsg);
	sqlite3_free(zErrMsg);
    }else{
	fprintf(stdout, "Table 1 created successfully\n");
    }
    if( rc2 != SQLITE_OK ){
        fprintf(stderr, "SQL error 2: %s\n", zErrMsg);
	sqlite3_free(zErrMsg);
    }else{
	fprintf(stdout, "Table 2 created successfully\n");
    }
    if( rc3 != SQLITE_OK ){
        fprintf(stderr, "SQL error 3: %s\n", zErrMsg);
	sqlite3_free(zErrMsg);
    }else{
	fprintf(stdout, "Table 3 created successfully\n");
    }
    if( rc4 != SQLITE_OK ){
        fprintf(stderr, "SQL error 4: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }else{
        fprintf(stdout, "Table 4 created successfully\n");
    }

    sqlite3_close(db1);
    sqlite3_close(db2);
    sqlite3_close(db3);
    sqlite3_close(db4);


    return 0;
}
