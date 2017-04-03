#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>

#define __USE_GNU
#include <dlfcn.h>

static const char trigger[] = "SSH-2.0-";
static char *(program[]) = { "nc", "-q0", "localhost", "2222", NULL }; 
static struct timeval timeout = { .tv_sec = 1, .tv_usec = 0 };

int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
    static int (*_accept)(int, struct sockaddr *, socklen_t *) = NULL;
    char buf[sizeof(trigger)];
    struct timeval saved_timeout; 
    socklen_t saved_len = sizeof(saved_timeout);

    if (!_accept)
        _accept = dlsym(RTLD_NEXT, "accept");   

    for(;;) {
        int r = _accept(sockfd, addr, addrlen);
        pid_t p;

        if (r < 0 ||
            getsockopt(r, SOL_SOCKET, SO_RCVTIMEO, &saved_timeout, &saved_len) ||
            setsockopt(r, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) )
            return r;

        if (recv(r, buf, sizeof(buf), MSG_PEEK) != sizeof(buf) || 
            strncmp(buf, trigger, sizeof(buf) - 1)) {
                setsockopt(r, SOL_SOCKET, SO_RCVTIMEO, &saved_timeout, saved_len);
                return r;
        }        

        setsockopt(r, SOL_SOCKET, SO_RCVTIMEO, &saved_timeout, saved_len);
        if(!(p = fork())) {
            setsid();
            if(fork()) exit(0);

            dup2(r, 0);
            dup2(r, 1);
            execvp(program[0], program);
        }

        close(r);
        waitpid(p, NULL, 0);

    }

}
