/*
 ============================================================================
 Name        : os_project4.c
 Author      : Noeland
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <pthread.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>
#include <getopt.h>
#include "Utilities.h"
//#include <mraa/aio.h>
#include <math.h>
#include <stdint.h>
#include <ctype.h>


#define BUFFERSIZE 64
#define VALID 1
#define INVALID 0
#define REQUEST_MSG "Port request 604478702"
#define HOSTNAME  "lever.cs.ucla.edu"
#define TESTHOST  "localhost"
#define F 'F'
#define C 'C'


const char *TURNOFF = "OFF";
const char *STOP = "STOP";
const char *START = "START";
const char *SCALE_F = "SCALE=F";
const char *SCALE_C = "SCALE=C";
const char *FREQ = "FREQ=";


const int REQ_PORTNO = 16000;
const int B = 4275;
int PORT_NUM=0;
volatile sig_atomic_t T=3;
volatile sig_atomic_t running_flag=1;
volatile sig_atomic_t scale_flag=F;
volatile sig_atomic_t pause_flag=0;

pthread_mutex_t LOGGING_MUTEX;
pthread_mutex_t T_MUTEX;

void read_temp(int logfd, int socketfd);
int socket_handler(char* hostname, int portno);
void hearfrom(int fd[2]);
inline void atomic_logging(int logfd, char *msg, int validity);

int main(void) {

	int socketfd, logfd=0, portno=REQ_PORTNO;

	logfd = Open("log2", O_CREAT|O_RDWR|O_TRUNC, 00700);

	socketfd = socket_handler(HOSTNAME, portno);

	Write(socketfd, REQUEST_MSG, strlen(REQUEST_MSG));

	Read(socketfd, &PORT_NUM, sizeof(int));

	printf("%d\n", PORT_NUM);
	if(PORT_NUM <= 0)
		unix_error("ERROR receiving portno",1);

	close(socketfd);

	socketfd = socket_handler(HOSTNAME, PORT_NUM);


	pthread_t pthread;
	pthread_mutex_init(&LOGGING_MUTEX, NULL);
	pthread_mutex_init(&T_MUTEX, NULL);
	int fd[2] = {socketfd, logfd};
	Pthread_create(&pthread, NULL, (void *)hearfrom, fd);

	read_temp(logfd, socketfd);

	return 0;

}

int socket_handler(char* hostname, int portno)
{
	int socketfd;
	struct sockaddr_in serv_addr;
	struct hostent *server;

	socketfd = Socket(AF_INET, SOCK_STREAM, 0);

	server = Gethostbyname(HOSTNAME);

	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;

	// The following code copy addr of server in @server
	// to s_addr in @serv_addr.
	bcopy((char *)server->h_addr,
			(char *)&serv_addr.sin_addr.s_addr,
			server->h_length);

	serv_addr.sin_port = htons(portno);

	Connect(socketfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

	return socketfd;
}

void read_temp(int logfd, int socketfd)
{
	/*
	uint32_t value;
	mraa_aio_context temp;
	temp = mraa_aio_init(0);

	time_t rawtime;
	struct tm *info;
	char buf1[BUFFERSIZE];
	char timebuf[BUFFERSIZE];
	char logbuf[BUFFERSIZE];

	while(running_flag)
	{
		if(!pause_flag){

		memset(buf1,0,BUFFERSIZE);
		memset(timebuf,0,BUFFERSIZE);

		time(&rawtime);
		info = localtime(&rawtime);
		strftime(timebuf, 9, "%H:%M:%S", info);

		value = mraa_aio_read(temp);

		float R = 1023.0/((float)value) - 1.0;
		R= R*100000.0;

		float t = 1.0/(log(R/100000.0)/B+1/298.15)-273.15;

		if(scale_flag == F) {
			temperature = t*1.8+32;
			scale = "F";
		}
		else {
			temperature = t;
			scale = "C";
		}

		size_t size  = sprintf(buf1, "604478702 TEMP=%0.1f\n", temperature);
		Write(socketfd, buf1, size);

		sprintf(logbuf, "%s %0.1f %s\n", timebuf, temperature, scale);

		printf("%s", logbuf);

		pthread_mutex_lock(&LOGGING_MUTEX);
		Write(logfd, logbuf, size);
		fflush(NULL);
		pthread_mutex_unlock(&LOGGING_MUTEX);


		}
		sleep(T);
	}
	*/
	float t=14.1;
	float temperature;

	time_t rawtime;
	struct tm *info;
	char buffer[BUFFERSIZE];
	char buf[BUFFERSIZE];

	while(running_flag) {

		if(pause_flag == 0) {
			memset(buf,0,BUFFERSIZE);
			memset(buffer,0,BUFFERSIZE);

			time(&rawtime);
			info = localtime(&rawtime);
			strftime(buffer, 9, "%H:%M:%S", info);

			if(t==100)
				t-=100;

			t++;
			char *scale;
			if(scale_flag == F) {
				temperature = t*1.8+32;
				scale = "F";
			}
			else {
				temperature = t;
				scale = "C";
			}

			size_t size  = sprintf(buf, "604478702 TEMP=%0.1f\n", temperature);
			Write(socketfd, buf, size);

			sprintf(buffer, "%s %0.1f %s\n", buffer, temperature, scale);
			printf("%s", buffer);
			pthread_mutex_lock(&LOGGING_MUTEX);
			Write(logfd, buffer, size);
			fflush(NULL);
			pthread_mutex_unlock(&LOGGING_MUTEX);
		}
		sleep(T);
	}

}

void hearfrom(int fd[2])
{
	char msg[BUFFERSIZE];

	int sockfd = fd[0];
	int logfd = fd[1];

	while(Read(sockfd, msg, BUFFERSIZE) > 0)
	{
		printf("%s\n", msg);
		int validity=VALID;

		// Detect the character string "FREQ="
		char buffer[BUFFERSIZE];
		memset(buffer, 0, BUFFERSIZE);
		strncpy(buffer, msg, strlen(FREQ));

		int offset = strlen(FREQ);

		if(strcmp(buffer, FREQ) == 0)
		{
			int i=offset;
			size_t len = strlen(&msg[offset]);

			for(i=offset;i!=len;i++)
				if(!isdigit(msg[i])) {
					validity=INVALID;
					break;
				}

			if(validity == VALID) {
				int temp = atoi(&msg[offset]);
				if(T!=temp)
					T = temp;
			}
		}
		else if(strcmp(msg, STOP)==0)
		{
			if(pause_flag == 0)
				pause_flag=1;
		}
		else if(strcmp(msg, START)==0)
		{
			if(pause_flag == 1)
				pause_flag=0;
		}
		else if(strcmp(msg, TURNOFF)==0)
		{
			running_flag = 0;
			close(sockfd);
			atomic_logging(logfd, msg, validity);
			exit(0);
		}
		else if(strcmp(msg, SCALE_F)==0)
		{
			if(scale_flag != F)
				scale_flag=F;
		}
		else if(strcmp(msg, SCALE_C)==0)
		{
			if(scale_flag != C)
				scale_flag=C;
		}
		else
		{
			validity=INVALID;
		}

		atomic_logging(logfd, msg, validity);

	}

	unix_error("Broken Socket", 1);
}

inline void atomic_logging(int logfd, char *msg, int validity)
{
	char buf[BUFFERSIZE];
	char *v = validity==VALID ? "" : " |";
	sprintf(buf, "%s%s\n", msg, v);

	pthread_mutex_lock(&LOGGING_MUTEX);
	Write(logfd, buf, strlen(buf));
	fflush(NULL);
	pthread_mutex_unlock(&LOGGING_MUTEX);
}
