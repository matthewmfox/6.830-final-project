#include <sqlite3.h>
#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <fstream>
#include <ctime>
#include <time.h>
//#include "twitcurl.h"
//#include "oauthlib.h"

#include <pthread.h>
using namespace std;

int numTweets = 0;


std::vector<string>results;
sqlite3 * db;

static int callback(void *NotUsed, int argc, char **argv, char **azColName){
    int i;
    std::string str = "";
    for(i=0; i<argc; i++){
	std::string arg = argv[i] ? argv[i] : "NULL";
        str +=  arg+"\t";
    }
    str += "\n";
   // results.push_back(str);
    return 0;
}


void *performanceWrite(void *var){
    int id = 0;
    clock_t t;
    int inserted = 0;

    char *zErrMsg = 0;

    while(true){
	stringstream ss;
	ss << id;
	if(inserted < numTweets){
	    inserted++;
	}
	else{
	    int j = id - numTweets;
	    int mess;
	    stringstream s;
	    s << j;
	    string sql = "DELETE from TWEETS where ID = " + s.str() + ";";
	    mess = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);

	    if( mess != SQLITE_OK ){
                fprintf(stderr, "SQL error: %s\n", zErrMsg);
                sqlite3_free(zErrMsg);
            }
	}

	string query = "INSERT INTO TWEETS (ID,SCREENNAME,TEXT) VALUES (" + ss.str() + ", 'moxiefoxxx', 'This');";

        int status;
	t = clock();
        status = sqlite3_exec(db, query.c_str(), callback, 0, &zErrMsg);
	t = clock() - t;
        if( status != SQLITE_OK ){
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        }else{
            fprintf(stdout, "Write executed successfully\n");
        }

	printf("Write took %f seconds\n",((float)t)/CLOCKS_PER_SEC);
	id++;
    }
    return 0;
}    


void performanceRead(int var){
    string query = "SELECT * from TWEETS;";
    clock_t t;
    while(true){

        int status;
	char *zErrMsg = 0;
	t = clock();
        status = sqlite3_exec(db, query.c_str(), callback, 0, &zErrMsg);
	t = clock() - t;
        if( status != SQLITE_OK ){
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        }else{
            fprintf(stdout, "Read executed successfully\n");
        }
	printf("Read took %f seconds\n",((float)t)/CLOCKS_PER_SEC);
	//cout << "Read\n";
	//printResults();
    }
}


int main(){

    numTweets = 4;

    int message;
    message = sqlite3_open("SingleDB", &db);

    if( message ){
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
	exit(0);
    }else{
	fprintf(stderr, "Opened database successfully\n");
    }


    string query = "CREATE TABLE TWEETS(ID INT PRIMARY KEY NOT NULL, SCREENNAME CHAR(50) NOT NULL, TEXT CHAR(100) NOT NULL);";


    int status;
    char *zErrMsg = 0;
    status = sqlite3_exec(db, query.c_str(), callback, 0, &zErrMsg);
    if( status != SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }else{
        fprintf(stdout, "Query executed successfully\n");
    }


    pthread_t write_thread; 
    int var = 0;
    pthread_create(&write_thread, NULL, performanceWrite, (void *)var);

    performanceRead(var);
    return 0;    
}
