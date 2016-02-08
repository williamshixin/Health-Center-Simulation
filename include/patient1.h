#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVERPORTNUM "21426"
#define DOC1PORTNUM "41426"
#define DOC2PORTNUM "42426"
#define MAXDATALEN 256
#define MAXINFOLEN 64

