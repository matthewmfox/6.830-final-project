#include <cstdio>
#include <iostream>
#include <fstream>
#include "twitcurl.h"
#include "oauthlib.h"
#include <string>
using namespace std;


int main(){
    string userName( "moxiefoxxx" );
    string passWord( "6830pass" );

    twitCurl twitterObj;
    char tmpBuf[1024];
    string replyMsg;

    twitterObj.setTwitterUsername( userName );
    twitterObj.setTwitterPassword( passWord );

    twitterObj.getOAuth().setConsumerKey(string("2aUj45Uf0i7e9UNqvMr3aSp9V"));
    twitterObj.getOAuth().setConsumerSecret(string("R8npGlJdDtrThsFfqQ1gqFNqAzdFEUX0a4XkqD60gfW31S5UXW"));    

    twitterObj.getOAuth().setOAuthTokenKey( string("2562501762-WyOP68BESntQdBE7RvviyCwdprI87dSbh3puWMh") );
    twitterObj.getOAuth().setOAuthTokenSecret( string("w1HpguJcGVQz3KiKV0TL6ozYzi01tkahs1w7VoyjbOVc8") );


    /* Account credentials verification */
    if( twitterObj.accountVerifyCredGet()){
        twitterObj.getLastWebResponse( replyMsg );
	printf( "\ntwitterClient:: twitCurl::accountVerifyCredGet web response:\n%s\n", replyMsg.c_str() );
    }
    else{
        twitterObj.getLastCurlError( replyMsg );
        printf( "\ntwitterClient:: twitCurl::accountVerifyCredGet error:\n%s\n", replyMsg.c_str() );
    }
}

