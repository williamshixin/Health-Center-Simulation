#include "doctor.h"

int sockfd;
struct addrinfo hints, *servinfo, *p;
int rv;
int numbytes;
struct sockaddr_storage their_addr;
char buf[MAXDATALEN];
socklen_t addr_len;
char s[INET6_ADDRSTRLEN];
insurinfo_t insurdb[3];
char *doc1insurf = "doc1.txt";
char *doc2insurf = "doc2.txt";

void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
};

int readinsur(insurinfo_t *insurdb, char *filename){
	memset(insurdb, 0, (INSURITEMNUM * sizeof(insurinfo_t)));
	char buf[MAXDATALEN];
	char *name = (char *)malloc(MAXDATALEN * sizeof(char));
	char *amon = (char *)malloc(MAXDATALEN * sizeof(char));;
	int i = 0;
	const char delim[2] = " ";
	char *token;
	FILE *fp = fopen(filename, "r");
	if(!fp){
		return -1;
	}
	while(fgets(buf, MAXDATALEN, fp)){
		memset(name, 0, (MAXDATALEN * sizeof(char)));
		memset(amon, 0, (MAXDATALEN * sizeof(char)));
		token = strtok(buf, delim);
		strncpy(insurdb[i].insurname, token, strlen(token));

		token = strtok(NULL, delim);
		if(token[strlen(token)-1] == '\n'){
			token[strlen(token)-1] = '\0';
		}
		strcpy(insurdb[i].amount, token);				
		i++;
	}
	free(name);
	free(amon);
	fclose(fp);
	return 0;
}

int do_UDP(char *PORT, char *filename){
	if(readinsur(insurdb, filename)){
		fprintf(stderr, "Error in read insurance information\n\n");
		return 1;
	}
	if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}
    // loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
			p->ai_protocol)) == -1) {
			perror("listener: socket");
		continue; }
		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("listener: bind");
			continue; }
			break; }
			if (p == NULL) {
				fprintf(stderr, "listener: failed to bind socket\n");
				return 2;
			}
			freeaddrinfo(servinfo);
			printf("listener: waiting to recvfrom...\n");
			addr_len = sizeof(their_addr);
			while(1){
				if ((numbytes = recvfrom(sockfd, buf, MAXDATALEN-1 , 0, (struct sockaddr *)&their_addr, &addr_len)) == -1) {
					perror("recvfrom");
					exit(1);
				}
				printf("listener: got packet from %s\n", inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr),s, sizeof(s)));
				printf("listener: packet is %d bytes long\n", numbytes);
				buf[numbytes] = '\0';
				printf("listener: packet contains \"%s\"\n", buf);
				int i;
				char *amontosend = (char *)malloc(MAXDATALEN * sizeof(char));
				for(i = 0; i < INSURITEMNUM; i++){
					if(!strcmp(insurdb[i].insurname, buf)){
						strcpy(amontosend, insurdb[i].amount);
						break;
					}
				}
				if (sendto(sockfd, amontosend, strlen(amontosend), 0, (struct sockaddr *)&their_addr, addr_len) < 0){
					perror("sendto");
					return 1;
				}
				free(amontosend);
			}
			return 0;
		}

		int main(int argc, char *argv[]){


			memset(&hints, 0, sizeof hints);
			hints.ai_family = AF_UNSPEC;
			hints.ai_socktype = SOCK_DGRAM;
			hints.ai_flags = AI_PASSIVE; 


			if(!fork()){
				do_UDP(DOC1PORTNUM, doc1insurf);
			}else{
				do_UDP(DOC2PORTNUM, doc2insurf);
			}

			close(sockfd);
			return 0;
		}
