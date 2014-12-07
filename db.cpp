#include <stdio.h>
#include <sqlite3.h>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <sstream>
#include <pthread.h>
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

std::unordered_map<std::int, bool> continueMap;

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

void createPartition(int partitionID, sqlite3 * db, int maxTableSize){
    int message;
    sqlite3 * db2;
    char name = char(partitionID);
    char *pName = &name;
    message = sqlite3_open(pName, &db2);
    if( message ){
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
	exit(0);
    }else{
	fprintf(stderr, "Opened database successfully\n");
    }
    Partition p(partitionID, db2, maxTableSize);
    pList.push_back(p);
}

void createDB(int numberOfPartitions, int numSeconds, int tSize){
    for(int i = 0; i< numberOfPartitions; i++){
	stringstream ss;
	ss << i;
        sqlite3 * db;
	std::string dName = "db" + ss.str();
        createPartition(i, db, tSize);
    }
    numberSeconds = numSeconds;
}

/* Function called to execute query on all partitions*/
void execQueryAll(char * SQLquery){
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

/* Function called to execute query on a single partition */
void execQueryOne(char * SQLquery, int partitionID){
    char *zErrMsg = 0;

    if(pList[partitionID].locked == true){    
        int status;
        status = sqlite3_exec(pList[partitionID].db, SQLquery, callback, 0, &zErrMsg);
        if( status != SQLITE_OK ){
            fprintf(stderr, "SQL error on partition %d: %s\n", partitionID, zErrMsg);
            sqlite3_free(zErrMsg);
        }else{
            fprintf(stdout, "Query executed successfully on partition %d \n", partitionID);
        }

    }
    else{
        fprintf(stderr, "Please lock partition %d before writing to it \n", partitionID);
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
    for (int i = 0; i < results.size(); i++) {
        printf("%s", results[i].c_str());
    }
}

/* Thread loop for continually updating table */
void endlessTwitterLoop(int tableID, string twitterArguments)
{

    int iterations = 0;
    cout << "Started twitter link for table " << tableID;

    while(continueThread){
        /*call twitter and load table partitions */
        iterations += 1;
    }
    cout << "Finished twitter link for table " << tableID;
    cout << "Iterations completed: " << iterations;

}

void stopTwitterLoop(int tableID){

    continueMap[tableID] = false;
    cout << "Stopping TwitterLoop for table: " << tableID;
}

/* Associate a table with a thread that continually updates it */
void linkTableToStream(int tableID, string twitterArguments){

    if (continueMap.find(tableID) == continueMap.end() ){
        /* Not found, so create thread */
        continueMap[tableID] = true;
        std::thread t1(endlessTwitterLoop, tableID, twitterArguments);
    }else{
        /* Found, so don't create thread */
        cout << "A loop already exists for TableID " << tableID;
    }
    
}
 
int main(){

    cout << "Welcome to the Firestream console\n";
    cout << "Firestream is a DB designed for streaming data \n \n";

    createDB(4, 2, 4);


    while(true){
        string query = "";
	string partition = "";
	results.clear();
        cout << "Please enter an SQL Query:";
        getline(cin, query);
	char q[1024];
	strcpy(q, query.c_str());
        execQueryAll(q);
        printResults();
    }

    return 0;
}
