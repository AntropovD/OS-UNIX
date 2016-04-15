#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <netdb.h>
#include <netinet/in.h>

#include <unistd.h>

#define PORT 5001

#define HEIGHT 20
#define WIDTH 40

void doprocessing (int sock);

void make_step();
void print_board();

int count(int i, int j);
int get_value(int x, int y);

int main( int argc, char *argv[] )
{
    int i;    
    for (i = 0; i < 100; i++) {        
		print_board();
        make_step();    
        sleep(1);        
    }
    /*
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    //char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int pid; //,n;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = PORT;

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR on binding");
        exit(1);
    }

    listen(sockfd, 5);
    clilen = sizeof(cli_addr);

    while (1) {
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

        if (newsockfd < 0) {
            perror("ERROR on accept");
            exit(1);
        }
        pid = fork();
        if (pid < 0) {
            perror("ERROR on fork");
            exit(1);
        }
        if (pid == 0) {
            close(sockfd);
            doprocessing(newsockfd);
            exit(0);
        }
        else
            close(newsockfd);
    }*/
}
/*
void doprocessing (int sock)
{
    int n;
    char buffer[256];
    bzero(buffer,256);
    n = read(sock,buffer,255);

    if (n < 0) {
        perror("ERROR reading from socket");
        exit(1);
    }
    printf("Here is the message: %s\n",buffer);
    n = write(sock,"I got your message",18);
    if (n < 0) {
        perror("ERROR writing to socket");
        exit(1);
    }
}
*/

char board[HEIGHT][WIDTH] = { 	    "##*********##***************************",
                                    "##********#**#***##*****#*******###*****",
                                    "***********#*#***#*#****#******###******",
                                    "************#*****#*****#***************",
                                    "****************************************",
                                    "****************************************",
                                    "****************************************",
                                    "*************************#**************",
                                    "***********************#*#**************",
                                    "*************##******##**************##*",
                                    "************#***#****##**************##*",
                                    "*##********#*****#***##*****************",
                                    "*##********#***#*##****#*#**************",
                                    "***********#*****#*******#**************",
                                    "************#***#***********************",
                                    "*************##*************************",
                                    "****************************************",
                                    "****************************************",
                                    "****************************************",
                                    "****************************************"
                             };

int count(int i, int j)
{
    int x, y, count =0;

    for (x = -1; x <= 1; x++)
        for (y = -1; y <= 1; y++)
            if ((x || y) && get_value(x+i,y+j))
                count++;
    return count;
}

int get_value(int x, int y)
{
    if (x>=0 && x<HEIGHT && y>=0 && y<WIDTH && board[x][y]=='#')
        return 1;
    return 0;
}

void make_step()
{
    int	i, j, t;
    char tmpBoard[HEIGHT][WIDTH];

    for (i = 0; i < HEIGHT; i++)
        for (j = 0; j < WIDTH; j++)
        {
            t = count(i, j);
            if (t == 2) tmpBoard[i][j] = board[i][j];
            if (t == 3) tmpBoard[i][j] = '#';
            if (t < 2) tmpBoard[i][j] = '*';
            if (t > 3) tmpBoard[i][j] = '*';
        }
    for (i = 0; i < HEIGHT; i++)
        for (j = 0; j < WIDTH; j++)
            board[i][j] = tmpBoard[i][j];
}

void print_board()
{
	system("clear");
    int i,j;
    for (i = 0; i < HEIGHT; i++) {
        for (j = 0; j < WIDTH; j++)
            printf("%c", board[i][j]);
        printf("\n");
    }
}
