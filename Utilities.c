
/////////////////////////////////////////////////////////////
//				    	Utilities                          //
/////////////////////////////////////////////////////////////
#include <pthread.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "Utilities.h"

void unix_error(char *msg, int status) {
	fprintf(stderr,"\n");
	perror(msg);
	exit(status);
}

/////////////////////////////////////////////////////////////
// 				Syscall Wrapper Declaration				   //
/////////////////////////////////////////////////////////////
int Pipe(int pipefd[2]) {
	int ret;
	if ((ret = pipe(pipefd)) == -1) {
		unix_error("pipe error", EXIT_FAILURE);
	}

	return ret;
}

pid_t Fork() {
	pid_t pid;
	if ((pid = fork()) < 0)
		unix_error("fork error", EXIT_FAILURE);

	return pid;
}

int Dup2(int oldfd, int newfd) {
	int ret;
	if ((ret = dup2(oldfd, newfd)) == -1) {
		unix_error("dup2 error", EXIT_FAILURE);
	}

	return ret;
}

ssize_t Write(int fd, const void *buf, size_t size) {
	ssize_t count;

	if ((count = write(fd, buf, size)) < 0)
		unix_error("write error", EXIT_FAILURE);

	return count;
}

ssize_t Read(int fd, void *buf, size_t size) {
	ssize_t count;

	if ((count = read(fd, buf, size)) < 0)
		unix_error("read error", EXIT_FAILURE);

	return count;
}

int Kill(pid_t pid, int sig) {
	int ret;
	if ((ret = kill(pid, sig)) < 0)
		unix_error("kill error", EXIT_FAILURE);
	return ret;
}

int Pthread_create(pthread_t *thread, const pthread_attr_t *attr,
		void *(*start_routine)(void *), void *arg) {
	if (pthread_create(thread, attr, start_routine, arg) != 0)
		unix_error("ERROR Thread creation", EXIT_FAILURE);

	return 0;
}

int Socket(int domain, int type, int protocol) {
	int socketfd;

	socketfd = socket(domain, type, protocol);
	if(socketfd < 0)
		unix_error("ERROR open socket", 1);

	return socketfd;
}

struct hostent *Gethostbyname(const char *name)
{
	struct hostent *srv;
	srv = gethostbyname(name);
	if(srv == NULL)
		unix_error("ERROR no such host", 1);

	return srv;
}

void Connect(int sockfd, const struct sockaddr *addr,
                   socklen_t addrlen)
{
	if (connect(sockfd, addr, addrlen) < 0)
	        unix_error("ERROR connecting",1);

}

void Shutdown(int socket, int how) {
	if (shutdown(socket, how) < 0)
		unix_error("ERROR close socket",1);
}

void Bind(int sockfd, const struct sockaddr *addr,
		 socklen_t addrlen) {
	if (bind(sockfd, (struct sockaddr *) addr,
	              addrlen) < 0)
		unix_error("ERROR on binding", 1);
}

int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
	int newsockfd;
	newsockfd = accept(sockfd, addr, addrlen);
	if(newsockfd <0)
		unix_error("ERROR on accept",1);

	return newsockfd;
}

int Open(const char *pathname, int flags, int mode)
{
	int newfd;
	if((newfd = open(pathname, flags, mode)) < 0)
		unix_error("ERROR open file",1);
	return newfd;
}

FILE* Fopen(const char *pathname, const char *mode)
{
	FILE* newfd;
	if((newfd = fopen(pathname, mode)) == NULL)
		unix_error("ERROR open file",1);

	return newfd;
}

ssize_t Pread(int fd, void *buf, size_t count, off_t offset)
{
	size_t ret = pread(fd, buf, count, offset);
	if(ret < count)
		unix_error("ERROR incomplete reading", 1);

	return ret;
}
