#include "patient2.h"

char *readpi(char *filename){
	char *buf = (char *)malloc(MAXDATALEN * sizeof(char));
	memset(buf, 0, (MAXDATALEN * sizeof(char)));
	FILE *fp = fopen(filename, "r");
	if(!fp){
		return NULL;
	}
	if(!fgets(buf, MAXDATALEN, fp)){
		fclose(fp);
		return NULL;
	}
	fclose(fp);
	return buf;
};

int readinsurinfo(char *p2insur, char *filename){
		char *buf = (char *)malloc(MAXDATALEN * sizeof(char));
	memset(buf, 0, (MAXDATALEN * sizeof(char)));
		const char delim[2] = " ";
	char *token;
	FILE *fp = fopen(filename, "r");
	if(!fp){
		return -1;
	}
	if(!fgets(buf, MAXDATALEN, fp)){
		fclose(fp);
		return -1;
	}
	token = strtok(buf, delim);
	strcpy(p2insur, token);
	fclose(fp);
	free(buf);
	return 0;
}

void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
};

void getavaindex(int *avaindex, char *msg){
	char *token;
	char delim[2] = " ";
	char buffer[MAXDATALEN];
	int index;
	memset(buffer, 0, (MAXDATALEN * sizeof(char)));
	token = strtok(msg, delim);
	strcpy(buffer, token);
	index = atoi(buffer);
	avaindex[0] = index;


	int i = 1;
	while(token){
		int j;
		for(j = 0; j < 2; j++){
			token = strtok(NULL, delim);
		}
		if(token){
			strcpy(buffer, (token + 5));
			index = atoi(buffer);
			if(index != 0){
				avaindex[i] = index;
			}
			i++;
		}
	}

}


int do_phase3(char *PORT, char *insur){
	int sockfd;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	int numbytes;
	struct sockaddr_storage their_addr;
	char buf[MAXDATALEN];
	char s[INET6_ADDRSTRLEN];
	socklen_t addr_len;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	if ((rv = getaddrinfo("nunki.usc.edu", PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}
    // loop through all the results and make a socket
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
			p->ai_protocol)) == -1) {
			perror("talker: socket");
		continue;
	}
	break; }
	if (p == NULL) {
		fprintf(stderr, "talker: failed to bind socket\n");
		return 2;
	}
	if ((numbytes = sendto(sockfd, insur, strlen(insur), 0,
		p->ai_addr, p->ai_addrlen)) == -1) {
		perror("talker: sendto");
	exit(1); }
	freeaddrinfo(servinfo);
	addr_len = sizeof(their_addr);
	printf("talker: sent %d bytes\n", numbytes);
	if ((numbytes = recvfrom(sockfd, buf, MAXDATALEN-1 , 0, (struct sockaddr *)&their_addr, &addr_len)) == -1) {
		perror("recvfrom");
		exit(1);
	}
	printf("listener: got packet from %s\n", inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr),s, sizeof(s)));
	printf("listener: packet is %d bytes long\n", numbytes);
	buf[numbytes] = '\0';
	printf("listener: packet contains \"%s\"\n", buf);
	close(sockfd);
	return 0; 
}

int main(int argc, char *argv[]){
	/*read the name and password of patient1 from the file*/
	char msg[MAXDATALEN] = "authenticate ";	
	char *filename = "patient2.txt";
	char *patientinfo = readpi(filename);
	if(!patientinfo){
		fprintf(stderr, "Error in reading info of patient2.\n");
		return 3;
	}
	strcat(msg, patientinfo);
	char *name = (char *)malloc(MAXDATALEN * sizeof(char));
	memset(name, 0, (MAXDATALEN * sizeof(char)));
	char *password = (char *)malloc(MAXDATALEN * sizeof(char));
	memset(password, 0, (MAXDATALEN * sizeof(char)));
	const char delim[2] = " ";
	char *token;
	token = strtok(patientinfo, delim);
	strcpy(name, token);
	token = strtok(NULL, delim);
	strcpy(password, token);

	char *p2insurfile = "patient2insurance.txt";
	char *p2insur = (char *)malloc(MAXDATALEN * sizeof(char));
	memset(p2insur, 0, (MAXDATALEN * sizeof(char)));
	readinsurinfo(p2insur, p2insurfile);

	int sockfd;
	struct addrinfo hints, *servinfo, *p;
	int retval; 
	char s[INET6_ADDRSTRLEN];
	char buf[MAXDATALEN];
	int numbytes;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	if ((retval = getaddrinfo("nunki.usc.edu", SERVERPORTNUM, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(retval));
		return 1;
	}
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
			p->ai_protocol)) == -1) {
			perror("client: socket");
		continue; 
	}
	if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
		close(sockfd);
		perror("client: connect");
		continue;
	}
	break; 
}
int get;
struct sockaddr_in client_addr;
socklen_t clientSockLength = sizeof(struct sockaddr_in);
get=getsockname(sockfd,(struct sockaddr *) &client_addr,&clientSockLength);

if (get== -1) {
	perror("getsockname");
	exit(1);
}

printf("client IP:%s & port: %d\n\n",inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));

if (p == NULL) {
	fprintf(stderr, "client: failed to connect\n");
	return 2;
}
inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof s);
printf("client: connecting to %s\n", s);
freeaddrinfo(servinfo); 
if (send(sockfd, msg, strlen(msg), 0) == -1){
	perror("send");
	exit(1);
}
printf("Authentication request from Patient 1 with username %s and password %s has been sent to the Healthe Center Server\n\n", name, password);
if ((numbytes = recv(sockfd, buf, MAXDATALEN-1, 0)) == -1) {
	perror("recv");
	exit(1); 
}
buf[numbytes] = '\0';
printf("Patient 1 authentication result: %s\n\n", buf);
if(!strcmp(buf, "failure")){
	fprintf(stderr, "Authentication failed. Exiting...");
	close(sockfd);
	exit(0);
}
printf("END of Phase 1 for Patient1\n\n");

char *reqava = "available";
if(send(sockfd, reqava, strlen(reqava), 0) == -1){
	perror("send");
	exit(1);
}

if ((numbytes = recv(sockfd, buf, MAXDATALEN-1, 0)) == -1) {
	perror("recv");
	exit(1); 
}
buf[numbytes] = '\0';
printf("The following appointments are available for Patient 2:\n");
printf("%s\n", buf);
int avaindex[6];
int i;
for(i = 0; i < 6; i++){
	avaindex[i] = -1;
}
getavaindex(avaindex, buf);
int done = 0;
char index[2];
while(!done){
	printf("Please enter the preferred appointment index and press enter:");
	scanf("%s", index);
	int input;

	input = atoi(index);
	for(i = 0; i < 6; i++){
		if(input == avaindex[i]){
			done = 1;
			break;
		}
	}
}

char *indexmsg = (char *)malloc(MAXDATALEN * sizeof(char));
memset(indexmsg, 0, (MAXDATALEN * sizeof(char)));
strcpy(indexmsg, "selection ");
strcat(indexmsg, index);
if(send(sockfd, indexmsg, strlen(indexmsg), 0) == -1){
	perror("send");
	exit(1);
}

memset(buf, 0, (MAXDATALEN * sizeof(char)));
if ((numbytes = recv(sockfd, buf, MAXDATALEN-1, 0)) == -1) {
	perror("recv");
	exit(1); 
}
buf[numbytes] = '\0';


char *portnum = (char *)malloc(MAXDATALEN * sizeof(char));
memset(portnum, 0, (MAXDATALEN * sizeof(char)));
if(strcmp(buf, "notavailable")){
	token = strtok(buf, delim);
	token = strtok(NULL, delim);
	strcpy(portnum, token);
	printf("The requested appointment is available and reserved to Patient 1. The assigned doctor port number is %s\n\n", portnum);
}else{
	printf("The requested appointment from Patient 2 is not available. Exiting...\n\n");
}

close(sockfd);

do_phase3(portnum, p2insur);

return 0;
}



