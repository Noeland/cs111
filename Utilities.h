#ifndef UTILITLES_H
#define UTILITIES_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <mcrypt.h>
#include <math.h>
#include <time.h>

void unix_error(char *msg, int status);
// void timer();

/////////////////////////////////////////////////////////////
// 				Syscall Wrapper Declaration				   //
/////////////////////////////////////////////////////////////
int Pipe(int pipefd[2]);
pid_t Fork();
int Dup2(int old, int new);
ssize_t Write(int fd, const void *buf, size_t size);
ssize_t Read(int fd, void *buf, size_t size);
int Kill(pid_t pid, int sig);
int Pthread_create(pthread_t *thread, const pthread_attr_t *attr,
				   void *(*start_routine)(void *), void *arg);
int Socket(int domain, int type, int protocal);
struct hostent *Gethostbyname(const char *name);
void Connect(int sockfd, const struct sockaddr *addr,
                   socklen_t addrlen);
void Shutdown(int socket, int how);
void Bind(int sockfd, const struct sockaddr *addr,
		 socklen_t addrlen);
int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
int Open(const char *pathname, int flags, int mode);
FILE* Fopen(const char *path, const char *mode);
ssize_t Pread(int fd, void *buf, size_t count, off_t offset);

#endif
