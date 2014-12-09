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
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
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
    results.push_back(str);
    return 0;
}


void printResults(){
    for (int i = 0; i < results.size(); i++) {
        printf("%s", results[i].c_str());
    }
    printf("Done\n");
    results.clear();
}


void *performanceWrite(void *var){
    sqlite3 * db2;
    sqlite3_open("SingleDB", &db2);
    int id = 0;
    clock_t t;
    int inserted = 0;

    char *zErrMsg = 0;

    while(true){
	while(fmod((float)(clock())/CLOCKS_PER_SEC, 2) != 0){}
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
	    mess = sqlite3_exec(db2, sql.c_str(), callback, 0, &zErrMsg);

	    if( mess != SQLITE_OK ){
                fprintf(stderr, "SQL error: %s\n", zErrMsg);
                sqlite3_free(zErrMsg);
            }
	}

	string query = "INSERT INTO TWEETS (ID,SCREENNAME,TEXT) VALUES (" + ss.str() + ", 'moxiefoxxx', 'This');";

        int status;
	t = clock();
        status = sqlite3_exec(db2, query.c_str(), callback, 0, &zErrMsg);
	t = clock() - t;
        if( status != SQLITE_OK ){
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        }else{
            fprintf(stdout, "Write executed successfully\n");
        }

	printf("Write took %f seconds\n",((float)t)/CLOCKS_PER_SEC);
	id++;
	printf("ID: %d\n", id);
	//sleep(1);
    }
    return 0;
}    


void performanceRead(int var){
    string query = "SELECT * from TWEETS;";
    clock_t t;
    int count = 0;
    while(true){
	while(fmod((float)(clock())/CLOCKS_PER_SEC, 2) != 0){} 
	count ++;
        int status;
	char *zErrMsg = 0;
	t = clock();
        status = sqlite3_exec(db, query.c_str(), callback, 0, &zErrMsg);
	t = clock() - t;
        if( status != SQLITE_OK ){
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        }else{
            fprintf(stdout, "Read executed successfully: %d \n", count);
        }
	printf("Read took %f seconds\n",((float)t)/CLOCKS_PER_SEC);
	//cout << "Read\n";
	printResults();
	//sleep(1);
    }
}


int main(){

    numTweets = 8;

    int message;
    message = sqlite3_open("SingleDB", &db);
    sqlite3_config(SQLITE_CONFIG_SERIALIZED);
    printf("Safe: %d\n", sqlite3_threadsafe());

    if( message ){
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
	exit(0);
    }else{
	fprintf(stderr, "Opened database successfully\n");
    }


    string query = "CREATE TABLE TWEETS(ID INT PRIMARY KEY NOT NULL, SCREENNAME CHAR(50) NOT NULL, TEXT CHAR(100) NOT NULL);";

    sleep(2);

    int status;
    char *zErrMsg = 0;
    status = sqlite3_exec(db, query.c_str(), callback, 0, &zErrMsg);
    if( status != SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }else{
        fprintf(stdout, "Creation executed successfully\n");
    }

    int id = -1;
    stringstream ss;
    ss << id;

    query = "INSERT INTO TWEETS (ID,SCREENNAME,TEXT) VALUES (" + ss.str() + ", 'moxiefoxxx', 'First');";
    query = "PRAGMA journal_mode = WAL;";

    status = sqlite3_exec(db, query.c_str(), callback, 0, &zErrMsg);
    if( status != SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }else{
        fprintf(stdout, "Entered WAL\n");
    }

    sleep(2);

    pthread_t write_thread; 
    int var = 0;
    pthread_create(&write_thread, NULL, performanceWrite, (void *)var);

    performanceRead(var);
    return 0;    
}
