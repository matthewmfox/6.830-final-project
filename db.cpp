#include<stdio.h>
#include<sqlite3.h>
#include<iostream>
#include<map>
#include<vector>
#include<string>
using namespace std;


/*
Firestream: A Database Designed For Streaming Data

Notes:
DB has many partitions
Partitions in a DB many host N tables where N >= 1

*/

int numberSeconds;

/* Results of last query stored here, row by row */
std::vector<string>results;

static int callback(void *NotUsed, int argc, char **argv, char **azColName){
    int i;
    std::string str = "";
    for(i=0; i<argc; i++){
	std::string arg = argv[i] ? argv[i] : "NULL";
        str +=  arg+"\t";
    }
    str += "\n";
    results.push_back(str);
    return 0;
}


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

void Partition::lock(){
    this->locked = true;
}

void Partition::unlock(){
    this->locked = false;
}

/* All partitions  stored here*/
std::vector<Partition> pList;

void createPartition(int partitionID, const char * partitionName, sqlite3 * db, int maxTableSize){
    int message;
    message = sqlite3_open(partitionName, &db);
    if( message ){
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
	exit(0);
    }else{
	fprintf(stderr, "Opened database successfully\n");
    }
    Partition p(partitionID, db, maxTableSize);
    pList.push_back(p);
}

void createDB(int numberOfPartitions, int numSeconds, int tSize){
    for(int i = 0; i< numberOfPartitions; i++){
	const char * pName = "partition" + to_string(i);
        sqlite3 * db;
	std::string dName = "db" + to_string(i);
        createPartition(i, pName, db, tSize);
    }
    numberSeconds = numSeconds;
}

/* Function called to execute query */
void execQuery(char * SQLquery){
    char *zErrMsg = 0;
    for ( int j = 0; j < pList.size(); j++ )
    {
        if(pList[j].locked == false){
            
            int status;
            status = sqlite3_exec(pList[j].db, SQLquery, callback, 0, &zErrMsg);
            if( status != SQLITE_OK ){
                fprintf(stderr, "SQL error on partition %d: %s\n", pList[j].partitionID, zErrMsg);
                sqlite3_free(zErrMsg);
            }else{
                fprintf(stdout, "Query executed successfully on partition %d \n", pList[j].partitionID);
            }

        }
    }
}

/* Close connection to SQLite3 database */
void closeTable(sqlite3 * db){
    sqlite3_close(db);
}


void lockPartition(int partitionID){
    pList[partitionID].lock();
}

void unlockPartition(int partitionID){
    pList[partitionID].unlock();
}

int howManyPartitions(){
    return pList.size();
}

 

void printResults(){
    vector<std::string>::iterator row;
    for (int i = 0; i < results.size(); i++) {
        printf("%s", results[i].c_str());
    }
}


int main(){

    cout << "Welcome to the Firestream console";
    cout << "Firestream is a DB designed for streaming data \n \n";

    while(true){
        char * query;
        cout << "Please enter an SQL Query:";
        cin >> query;
        execQuery(query);
        printResults();
    }

    return 0;
}
