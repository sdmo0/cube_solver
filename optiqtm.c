//FILE OPTIQTM.C

#define _LINUX_
//Comment this line out if you compile with MinGW under Windows.
//This disables the SIGNAL handling.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include <assert.h>
#include <signal.h>
#include <time.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "cubedefs.h"

#define MAX_STR 256
#define SERVER "192.168.123.9"
#define PORT_NO 51717

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

int must_connect = FALSE;

char manString[MAX_STR];
int subOptLev;
int symRed;

#ifdef _LINUX_
static sigjmp_buf jump_buf;
#endif

CubieCube cc_a;

int input_cube(int *sockfd, char *str, int n);

void error(const char *msg)
{
    perror(msg);
    if (must_connect) exit(0);
}

int connect_control_server(int port_no)
{
    int sockfd;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        error("ERROR opening socket");
        return -1;
    }
    server = gethostbyname(SERVER);
    
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        if (must_connect) exit(0);
        return -1;
    }
    
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(port_no);
    
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
        error("ERROR connecting");
        if (must_connect) exit(0);
        return -1;
    } else {
        printf("Success: Connecting to %s:%d\n", SERVER, port_no);
    }
    return sockfd;        
}


#ifdef _LINUX_ 
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void  user_break(int  n)
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
    printf("-- skipping cube --\n");
    fflush(stdout);
    siglongjmp(jump_buf, 1);
    return;
}
#endif

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void  pp()
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
    printf(".");
    fflush(stdout);
    return;
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int main(int argc, char * argv[])
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
{
    int i,l;
    int sockfd = -1;
    int port_no = PORT_NO;

    subOptLev=-1;
    symRed=1;
    for (i=1;i<argc;i++)
    {
        if (argv[i][0]=='+')//all optimal solutions/suboptimal solutions
        {
            subOptLev=0;
            if  (argv[i][1]>'0' && argv[i][1]<='9') subOptLev= argv[i][1]-48;
            
        } else if (argv[i][0]=='-') {
            if (argv[i][1]=='s') symRed=0;
            else if (argv[i][1] == 'c') {
                //must_connect = TRUE;
                int portno = -1;
                if (argc >= ++i)
                    portno = atoi(argv[i]);
                if (portno > 0) port_no = portno;
                sockfd = connect_control_server(port_no);
            }
            
        }
    }

    printf("initializing memory.\n");
    visitedA = (char *)calloc(NGOAL/8+1,1);//initialized to 0 by default
    visitedB = (char *)calloc(NGOAL/8+1,1);
    for (l=0;l<NTWIST;l++)
        movesCloserToTarget[l] = (short *)calloc(NFLIPSLICE*2,2);

    printf("initializing tables");fflush(stdout);
    initSymCubes();
    initMoveCubes();
    initInvSymIdx();
    initSymIdxMultiply();
    initMoveConjugate();
    initMoveBitsConjugate();
    initGESymmetries();
    initTwistConjugate();pp();
    initRawFLipSliceRep();pp();
    initTwistMove();pp();
    initCorn6PosMove();pp();
    initEdge4PosMove();pp();
    initEdge6PosMove();pp();
    initSymFlipSliceClassMove();pp();
    initMovesCloserToTarget();pp();
    initNextMove();pp();
    printf("\r\n");


    while (1)
    {

/**** original code ****
      if (fgets(manString,sizeof(manString),stdin)==NULL) break;
      if (manString[0]=='x') exit(EXIT_SUCCESS);
*/

        input_cube(&sockfd, manString, MAX_STR);

        time_t start_time = time(NULL);
    
        l=strlen(manString);
        if (manString[l-1]=='\n') manString[l-1]=0;//remove LF
        if (l>1 && manString[l-2]=='\r') manString[l-2]=0;//remove CR if present
        if (strlen(manString)==0) continue;//ignore empty lines
        printf("\nsolving optimal: %s\n",manString);fflush(stdout);
        cc_a = stringToCubieCube(manString);
#ifdef _LINUX_ 
        if (sigsetjmp(jump_buf, 1) == 0)
        {
            signal(SIGINT, user_break);
            solveOptimal(cc_a, sockfd);
        }
        signal(SIGINT, SIG_IGN);
#else
        solveOptimal(cc_a, sockfd);
#endif

        time_t end_time = time(NULL);

        printf("Elapsted solving time: %ld seconds\n", end_time - start_time);
    }
    exit(EXIT_SUCCESS);
    return 0;
}
