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
#include <signal.h>
#include <fcntl.h>

#define SERVERPORTNUM "21426"
#define BACKLOG 10
#define MAXDATALEN 256
#define MAXINFOLEN 64

void hanlder(int s);

typedef struct patientinfo{
	char name[MAXINFOLEN];
	char password[MAXINFOLEN];
} patientinfo_t;

typedef struct pilsnode {
	patientinfo_t *pi;
	void *next;
}pilsnode_t;

typedef struct avaslot{
	int reserved;
	char index[MAXDATALEN];
	char dateandtime[MAXDATALEN];
	char docinfo[MAXDATALEN];
}avaslot_t;


