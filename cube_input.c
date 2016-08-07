#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

int cfg_enter = 0;

// UF UR UB UL DF DR DB DL FR FL BR BL UFR URB UBL ULF DRF DFL DLB DBR
// WR WB WO WG YR YB YO YG RB RG OB OG WRB WBO WOG WGR YBR YRG YGO YOB

// A=0, B=1, C=2, D=3, E=4, F=5, G=6, H=7, I=8, J=9, K=10,
// L=11, M=12, N=13, O=14, P=15, Q=16, R=17, S=18, T=19, U=20,
// V=21, W=22, X=23, Y=24, Z=25

// Up = White
// Down = Yellow
// Front = Red
// Back = Orange
// Left = Green
// Right = Blue

// color to face
char map[30] = {0, 'R', 2, 3, 4, 5, 'L', 7, 8, 9, 10,
                11, 12, 13, 'B', 15, 16, 'F', 18, 19, 20,
                21, 'U', 23, 'D', 25, 0};

// face to color
char f2c_map[30] = {0, 'O', 2, 'Y', 4, 'R', 6, 7, 8, 9, 10,
                'G', 12, 13, 14, 15, 16, 'B', 18, 19, 'W',
                21, 22, 23, 24, 25, 0};


/*
char up[3][5] = {{'W', 'W', 'W'},
                 {'W', 'W', 'W'},
                 {'W', 'W', 'W'}};
char down[3][5] = {{'Y', 'Y', 'Y'},
                   {'Y', 'Y', 'Y'},
                   {'Y', 'Y', 'Y'}};
char front[3][5] = {{'R', 'R', 'R'},
                    {'R', 'R', 'R'},
                    {'R', 'R', 'R'}};
char back[3][5] = {{'O', 'O', 'O'},
                   {'O', 'O', 'O'},
                   {'O', 'O', 'O'}};
char left[3][5] = {{'G', 'G', 'G'},
                   {'G', 'G', 'G'},
                   {'G', 'G', 'G'}};
char right[3][5] = {{'B', 'B', 'B'},
                    {'B', 'B', 'B'},
                    {'B', 'B', 'B'}};
*/

char up[3][5];
char down[3][5];
char front[3][5];
char back[3][5];
char left[3][5];
char right[3][5];

int status = 0;

#define UP 1
#define DOWN 2
#define FRONT 4
#define BACK 8
#define LEFT 16
#define RIGHT 32
#define CHECK_STATE(a, b) ((a) & (b) ? 'O' : 'X')

int connect_control_server(int portno);

void print_faces(void)
{
    int i;
    for (i = 0; i < 3; i++)
        printf("      %c%c%c\n", up[i][0], up[i][1], up[i][2]);

    printf("\n");

    for (i = 0; i < 3; i++)
        printf("%c%c%c   %c%c%c   %c%c%c   %c%c%c\n",
               left[i][0], left[i][1], left[i][2],
               front[i][0], front[i][1], front[i][2],
               right[i][0], right[i][1], right[i][2],
               back[i][0], back[i][1], back[i][2]);

    printf("\n");

    for (i = 0; i < 3; i++)
        printf("      %c%c%c\n", down[i][0], down[i][1], down[i][2]);
    
    printf("=====================================================\n");

    int numColor[30] = {0, };
    int j;
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            numColor[up[i][j]-'A']++;
            numColor[front[i][j]-'A']++;
            numColor[left[i][j]-'A']++;
            numColor[right[i][j]-'A']++;
            numColor[back[i][j]-'A']++;
            numColor[down[i][j]-'A']++;
        }
    }

    printf("White = %d, Red = %d, Green = %d,\n"
           "Blue = %d, Orange = %d, Yellow = %d\n",
           numColor['W'-'A'], numColor['R'-'A'], numColor['G'-'A'],
           numColor['B'-'A'], numColor['O'-'A'], numColor['Y'-'A']);
    printf("=====================================================\n");
}

void print_menu(int status) 
{
    if (status) print_faces();
    
    printf("u(WHITE) : input UP => %c\n", CHECK_STATE(status, UP));
    printf("d(YELLOW) : input DOWN => %c\n", CHECK_STATE(status, DOWN));
    printf("f(RED) : input FRONT => %c\n", CHECK_STATE(status, FRONT));
    printf("b(ORANGE) : input BACK => %c\n", CHECK_STATE(status, BACK));
    printf("l(GREEN) : input LEFT => %c\n", CHECK_STATE(status, LEFT));
    printf("r(BLUE) : input RIGHT => %c\n", CHECK_STATE(status, RIGHT));
    printf("m(enu) : print menu\n");
    printf("e : need enter for motor movements => %c\n", cfg_enter ? 'O' : 'X');
    printf("i : input and analyze string notation for mixed cube\n");
    printf("a : analyze\n\n");
    printf("n : connect to Raspberry Pi\n");
    printf("x : mix the cube\n");
    printf("t : motor test\n");
    printf("q : exit\n");
}

void copy_arr(char d[3][5], char s[3][5])
{
    int i, j;
    for (i = 0; i < 3; i++)
        for (j = 0; j < 5; j++)
            d[i][j] = s[i][j];
}
    

void input_face(int face)
{
    char temp[3][5];

    switch (face) {
    case UP:        
        printf("** UP (white) **\n");
        break;
    case DOWN:
        printf("** DOWN (yellow) **\n");
        break;
    case FRONT:
        printf("** FRONT (red) **\n");
        break;
    case BACK:
        printf("** BACK (orange) **\n");
        break;
    case LEFT:
        printf("** LEFT (green) **\n");
        break;
    case RIGHT:
        printf("** RIGHT (blue) **\n");
        break;
    default:
        return;
        break;
    }

    int i;
    for (i = 0; i < 3; i++)
        scanf("%s", temp[i]);
    
    int j;
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            if ('a' <= temp[i][j] && temp[i][j] <= 'z')
                temp[i][j] -= 32;
        }
    }
    
    switch (face) {
    case UP:        
        copy_arr(up, temp);
        break;
    case DOWN:
        copy_arr(down, temp);
        break;
    case FRONT:
        copy_arr(front, temp);
        break;
    case BACK:
        copy_arr(back, temp);
        break;
    case LEFT:
        copy_arr(left, temp);
        break;
    case RIGHT:
        copy_arr(right, temp);
        break;
    default:
        break;
    }

    status |= face;
}

void analyze(char *str, int n)
{
    int len = 0;
    len += snprintf(str + len, n, "%c%c ", map[up[2][1]-'A'], map[front[0][1]-'A']); // UF
    len += snprintf(str + len, n, "%c%c ", map[up[1][2]-'A'], map[right[0][1]-'A']); // UR
    len += snprintf(str + len, n, "%c%c ", map[up[0][1]-'A'], map[back[0][1]-'A']); // UB
    len += snprintf(str + len, n, "%c%c ", map[up[1][0]-'A'], map[left[0][1]-'A']); // UL
    len += snprintf(str + len, n, "%c%c ", map[down[0][1]-'A'], map[front[2][1]-'A']); // DF
    len += snprintf(str + len, n, "%c%c ", map[down[1][2]-'A'], map[right[2][1]-'A']); // DR
    len += snprintf(str + len, n, "%c%c ", map[down[2][1]-'A'], map[back[2][1]-'A']); // DB
    len += snprintf(str + len, n, "%c%c ", map[down[1][0]-'A'], map[left[2][1]-'A']); // DL
    len += snprintf(str + len, n, "%c%c ", map[front[1][2]-'A'], map[right[1][0]-'A']); // FR
    len += snprintf(str + len, n, "%c%c ", map[front[1][0]-'A'], map[left[1][2]-'A']); // FL
    len += snprintf(str + len, n, "%c%c ", map[back[1][0]-'A'], map[right[1][2]-'A']); // BR
    len += snprintf(str + len, n, "%c%c ", map[back[1][2]-'A'], map[left[1][0]-'A']); // BL
    len += snprintf(str + len, n, "%c%c%c ", map[up[2][2]-'A'], map[front[0][2]-'A'], map[right[0][0]-'A']); // UFR
    len += snprintf(str + len, n, "%c%c%c ", map[up[0][2]-'A'], map[right[0][2]-'A'], map[back[0][0]-'A']); // URB
    len += snprintf(str + len, n, "%c%c%c ", map[up[0][0]-'A'], map[back[0][2]-'A'], map[left[0][0]-'A']); // UBL
    len += snprintf(str + len, n, "%c%c%c ", map[up[2][0]-'A'], map[left[0][2]-'A'], map[front[0][0]-'A']); // ULF
    len += snprintf(str + len, n, "%c%c%c ", map[down[0][2]-'A'], map[right[2][0]-'A'], map[front[2][2]-'A']); // DRF
    len += snprintf(str + len, n, "%c%c%c ", map[down[0][0]-'A'], map[front[2][0]-'A'], map[left[2][2]-'A']); // DFL
    len += snprintf(str + len, n, "%c%c%c ", map[down[2][0]-'A'], map[left[2][0]-'A'], map[back[2][2]-'A']); // DLB
    len += snprintf(str + len, n, "%c%c%c\n", map[down[2][2]-'A'], map[back[2][0]-'A'], map[right[2][2]-'A']); // DBR
}

void init(void)
{
    int i, j;
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 5; j++) {
            up[i][j] = down[i][j] = front[i][j] =
                back[i][j] = left[i][j] = right[i][j] = ' ';
        }
    }
    
    status = 0;
}

const int min_move = 5;
const int max_move = 100;

void mixing(int sockfd, int count)
{
    char FACE[6] = {'U', 'L', 'F', 'R', 'B', 'D'};
    char buf[256];
    int i;
    srand(time(NULL));

    int rotate;
    int idx = 0;
    int prev = rand() % 6;
    count = (count < min_move ? min_move : count);
    count = (count > max_move ? max_move : count);
    for (i = 0; i < count; i++) {
        do {
            rotate = rand() % 6;
        } while (prev == rotate);
        buf[idx++] = FACE[rotate];
        prev = rotate;
        
        rotate = rand();
        if (rotate & 0x1) buf[idx++] = '\'';
        buf[idx++] = ' ';
    }
    buf[idx] = 0;

    int n = write(sockfd, buf, idx);
    if (n < 0) printf("ERROR writing(%s)\n", buf);
    for (i = 0; i < idx; i++)
        if (buf[i] == '\'') buf[i] = '3';
    printf("Generated mixing movements:\n");
    printf("%s\n", buf);
}

int input_cube(int *sockfd, char *str, int n)
{
    init();
    int request_analyze = 0;

    while (1) {
        print_menu(status);

        fflush(stdout);
        fflush(stdin);

        int face = 0;
        char ch;
        request_analyze = 0;
        while (1) {
            printf("==> ");
            ch = getchar();
            if (ch == 'u' || ch == 'U') face = UP;
            else if (ch == 'd' || ch == 'D') face = DOWN;
            else if (ch == 'f' || ch == 'F') face = FRONT;
            else if (ch == 'b' || ch == 'B') face = BACK;
            else if (ch == 'l' || ch == 'L') face = LEFT;
            else if (ch == 'r' || ch == 'R') face = RIGHT;
            else if (ch == 'm' || ch == 'M') {print_menu(status); continue;}
            else if (ch == 'n' || ch == 'N') {
                printf("------------------------------------------------------\n");
                printf("input port no: ");
                char buf[256];
                scanf("%s", buf);
                int portno = atoi(buf);
                if (portno <= 0) portno = 51717;
                if (*sockfd > 0) {
                    printf("Closing socket(%d) for new connection\n", *sockfd);
                    close(*sockfd);
                }
                *sockfd = connect_control_server(portno);
                printf("------------------------------------------------------\n");

            } else if (ch == 'e' || ch == 'E') {
                printf("input cfg for enter necessity (Current cfg: %c): ", cfg_enter ? 'O' : 'X');
                scanf("%*[ \n\t\r]");
                ch = getchar();
                if (ch == 'O' || ch == 'o') cfg_enter = 1;
                else if (ch == 'X' || ch == 'x') cfg_enter = 0;
                
            } else if (ch == 'a' || ch == 'A') request_analyze = 1;
            else if (ch == 'x' || ch == 'X') {
                printf("------------------------------------------------------\n");
                if (*sockfd > 0) {
                    printf("input mix movement count( %d <= x <= %d): ", min_move, max_move);
                    char buf[256];
                    scanf("%s", buf);
                    int count = atoi(buf);
                    mixing(*sockfd, count);
                }
                else printf("Not connected to Raspberry Pi\n");
                printf("------------------------------------------------------\n");
                
            } else if (ch == 't' || ch == 'T') {
                printf("------------------------------------------------------\n");
                if (*sockfd < 0) {
                    printf("Not connected to Raspberry Pi\n");
                } else {
                    printf("input movements such as <R U F' D ...> : ");
                    char buf[256];
                    scanf("%*[ \n\t\r]");
                    fgets(buf, 256, stdin);
                    int n = write(*sockfd, buf, strlen(buf));
                    if (n < 0) printf("ERROR writing %s\n", buf);
                }
                printf("------------------------------------------------------\n");
                
            } else if (ch == 'i' || ch == 'I') {
                printf("input string notation for mixed cube : ");
                scanf("%*[ \n\t\r]");
                fgets(str, 256, stdin);
                request_analyze = 2;

            } else if (ch == 'q' || ch == 'Q') exit(0);
            else continue;
            printf("\n");
            
            break;
        }

        if (request_analyze) break;

        input_face(face);
    }

    if (request_analyze == 1)
        analyze(str, n);

    printf("%s\n", str);
    return 0;
}
