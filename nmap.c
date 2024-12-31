#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/time.h>

#define MIN_PORT 0
#define MAX_PORT 3000

struct connection_info {
    char *target;
    int port;
    struct timeval timeout;
};

int scan_port(struct connection_info *info) {
    int sock;
    struct sockaddr_in addr;
    
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        return -1;
    }
    
    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &info->timeout, sizeof(info->timeout)) < 0) {
        close(sock);
        return -1;
    }
    if (setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &info->timeout, sizeof(info->timeout)) < 0) {
        close(sock);
        return -1;
    }
    
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(info->port);
    
    if (inet_pton(AF_INET, info->target, &addr.sin_addr) <= 0) {
        close(sock);
        return -1;
    }
    
    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) == 0) {
        close(sock);
        return 1;
    }
    
    close(sock);
    return 0;
}

int main(int argc, char *argv[]) {
    struct connection_info info;
    char target[256] = "127.0.0.1";
    int port;
    
    info.timeout.tv_sec = 1;
    info.timeout.tv_usec = 0;
    
    if (argc > 1) {
        strncpy(target, argv[1], sizeof(target) - 1);
    }
    info.target = target;
    
    printf("Scan des ports TCP de %s\n", target);
    printf("Ports %d à %d...\n\n", MIN_PORT, MAX_PORT);
    
    for (port = MIN_PORT; port <= MAX_PORT; port++) {
        info.port = port;
        if (scan_port(&info) == 1) {
            printf("Port %d : OUVERT\n", port);
        }
    }
    
    printf("\nScan terminé.\n");
    return 0;
}
