#include "healthcenterserver.h"

int readpils(pilsnode_t *head, char *filename){
	char buf[MAXDATALEN];
	FILE *fp = fopen(filename, "r");
	pilsnode_t *curr =head; 
	if(!fp){
		return -1;
	}
	while(fgets(buf, MAXDATALEN, fp)){
		pilsnode_t *plnode = (pilsnode_t *)malloc(sizeof(pilsnode_t));
		memset(plnode, 0, sizeof(pilsnode_t));
		patientinfo_t *pi = (patientinfo_t *)malloc(sizeof(patientinfo_t));
		memset(pi, 0, sizeof(patientinfo_t));
		plnode->pi = pi;
		const char delim[2] = " ";
		char *token;
		token = strtok(buf, delim);
		strncpy(plnode->pi->name, token, strlen(token));
		token = strtok(NULL, delim);
		if(token[strlen(token)-1] == '\n'){
			token[strlen(token)-1] = '\0';
		}
		strncpy(plnode->pi->password, token, strlen(token));
		curr->next = plnode;
		curr = plnode;
	}

	fclose(fp);
	return 0;
};

int readava(avaslot_t *avadb, char *filename){
	char buf[MAXDATALEN];
	char *dateandtime = (char *)malloc(MAXDATALEN * sizeof(char));
	char *docinfo = (char *)malloc(MAXDATALEN * sizeof(char));;
	int i = 0;
	const char delim[2] = " ";
	char *token;
	FILE *fp = fopen(filename, "r");
	if(!fp){
		return -1;
	}
	while(fgets(buf, MAXDATALEN, fp)){
		memset(dateandtime, 0, (MAXDATALEN * sizeof(char)));
		memset(docinfo, 0, (MAXDATALEN * sizeof(char)));
		avadb[i].reserved = 0;
		token = strtok(buf, delim);
		strncpy(avadb[i].index, token, strlen(token));

		token = strtok(NULL, delim);
		strcpy(dateandtime, token);
		token = strtok(NULL, delim);
		strcat(dateandtime, " ");
		strcat(dateandtime, token);
		strcpy(avadb[i].dateandtime, dateandtime);

		token = strtok(NULL, delim);
		strcpy(docinfo, token);
		token = strtok(NULL, delim);
		if(token[strlen(token)-1] == '\n'){
			token[strlen(token)-1] = '\0';
		}
		strcat(docinfo, " ");
		strcat(docinfo, token);
		strcpy(avadb[i].docinfo, docinfo);				

		i++;
	}
	free(dateandtime);
	free(docinfo);
	fclose(fp);
	return 0;
}

void sigchld_handler(int s)
{
	while(waitpid(-1, NULL, WNOHANG) > 0);
}

int main(int argc, char *argv[]){

	char *filename = "users.txt";
	pilsnode_t *head = (pilsnode_t *)malloc(sizeof(pilsnode_t));
	memset(head, 0, sizeof(pilsnode_t));
	if(readpils(head, filename)){
		fprintf(stderr, "Error in reading info of users.\n");
		return 3;
	}

	char *avafilename = "availabilities.txt";
	avaslot_t avadb[6];
	memset(avadb, 0, (6 * sizeof(avaslot_t)));
	if(readava(avadb, avafilename)){
		fprintf(stderr, "Error in reading info of availabilities.\n");
		return 4;
	}


	//fputs("This is c programming.", newfp);
	//fputs("This is a system programming language.", newfp);

	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_storage con_addr;
	socklen_t sin_size;
	struct sigaction sa;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	int sockfd, acpsockfd, yes = 1, numbytes;
	char ipaddr[INET_ADDRSTRLEN];
	char buf[MAXDATALEN];
	const char delim[2] = " ";
	char *token;//used in get patient name and password from the received msg
	char *sucmsg = "success";
	char *failmsg = "failure";

	int retval = getaddrinfo(NULL, SERVERPORTNUM, &hints, &servinfo);
	if(retval){
		fprintf(stderr, "Error in getaddrinfo: %s\n", gai_strerror(retval));
		return 1;
	}

	for(p = servinfo; p != NULL; p = p->ai_next){
		sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if(sockfd == -1){
			perror("server: socket");
			continue;
		}

		if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1){
			perror("setsockopt");
			exit(1);
		}

		if(bind(sockfd, p->ai_addr, p->ai_addrlen) == -1){
			close(sockfd);
			perror("server: bind");
			continue;
		}

		break;
	}

	if(!p){
		fprintf(stderr, "Error in server: failed to bind\n");
		return 2;
	}

	void *addr;
	struct sockaddr_in *ip = (struct sockaddr_in *)p->ai_addr;
	addr = &(ip->sin_addr);
	inet_ntop(p->ai_family, addr, ipaddr, sizeof(ipaddr));

	freeaddrinfo(servinfo);
	//fcntl(sockfd, F_SETFL, O_NONBLOCK);
	if(listen(sockfd, BACKLOG) == -1){
		perror("listen");
		exit(1);
	}

sa.sa_handler = sigchld_handler; // reap all dead processes
sigemptyset(&sa.sa_mask);
sa.sa_flags = SA_RESTART;
if (sigaction(SIGCHLD, &sa, NULL) == -1) {
	perror("sigaction");
	exit(1); }

	printf("The Health Center Server has port number %s and IP address %s\n\n", SERVERPORTNUM, ipaddr);
	while(1){
		//printf("breakpoint\n");
		//printf("server listening\n");
		sin_size = sizeof(con_addr);
		acpsockfd = accept(sockfd, (struct sockaddr *)&con_addr, &sin_size);
		if(acpsockfd == -1){
			perror("accept");
			continue;
		}
		if (!fork()) { // this is the child process
            //close(sockfd); // child doesn't need the listener
            const char *fnotava = "notavaindex.txt";
			FILE *newfp = fopen(fnotava, "a+");
			char *notavaind = (char *)malloc(MAXDATALEN * sizeof(char));
			memset(notavaind, 0, (MAXDATALEN * sizeof(char)));
			fgets(notavaind, MAXDATALEN, newfp);
			printf("notavaind%s\n\n", notavaind);
			/*int indarr[6];
			int k;
			for(k = 0; k < 6; k++){
				indarr[k] = -1;
			}*/
			//k = 0;

			token = strtok(notavaind, delim);
			if(token){
				char *ind = (char *)malloc(MAXDATALEN * sizeof(char));
				memset(ind, 0, (MAXDATALEN * sizeof(char)));
				strcpy(ind, token);	
				int store = atoi(ind);
									avadb[store - 1].reserved = 1;
				printf("atoi%d\n\n", store);
				while((token = strtok(NULL, delim)) != NULL){
					memset(ind, 0, (MAXDATALEN * sizeof(char)));
					strcpy(ind, token);	
					int store = atoi(ind);
				printf("atoi%d\n\n", store);

					avadb[store - 1].reserved = 1;
				}
								free(ind);	
			}

			if ((numbytes = recv(acpsockfd, buf, MAXDATALEN-1, 0)) == -1) {
				perror("recv");
				exit(1); 
			}
			buf[numbytes] = '\0';
            //printf("server: received '%s'\n", buf);
			pilsnode_t *curr = (pilsnode_t *)head->next;
			char *name = (char *)malloc(MAXDATALEN * sizeof(char));
			memset(name, 0, (MAXDATALEN * sizeof(char)));
			char *password = (char *)malloc(MAXDATALEN * sizeof(char));
			memset(password, 0, (MAXDATALEN * sizeof(char)));

			token = strtok(buf, delim);
			if(strcmp(token, "authenticate")){
				fprintf(stderr, "Error in receiving authentication header, expecting \"authenticate\"");
				exit(1);
			}
            printf( "thisis authernationing%s\n", token);
			token = strtok(NULL, delim);
			strcpy(name, token);
            //printf( "%s\n", name);
			token = strtok(NULL, delim);
			strcpy(password, token);
            //printf( "%s\n", password);
			printf("The Health Center Server has received request from a patient with username %s and password %s\n\n", name, password);
			char *nameindb;
			char *passwordindb;

			while(curr){
				nameindb = curr->pi->name;
            	//printf("111%s\n", nameindb);
				passwordindb = curr->pi->password;
            	//printf("111%s\n", passwordindb);
				if(!strcmp(nameindb, name)){
            		//printf("%s\n", nameindb);
					if(!strcmp(passwordindb, password)){
            			//printf("%s\n", passwordindb);
						if (send(acpsockfd, sucmsg, strlen(sucmsg), 0) == -1){
							perror("send");
							exit(1);
						}
						printf("The Health Center Server sends the response %s to patient with username %s\n\n", sucmsg, name);
						break;
					}

				}
				curr = (pilsnode_t *)curr->next;
			}

			if(!curr){
				if (send(acpsockfd, failmsg, strlen(failmsg), 0) == -1){
					perror("send");
					exit(1);
				}
				printf("The Health Center Server sends the response %s to patient with username %s\n\n", failmsg, name);
			}

			if ((numbytes = recv(acpsockfd, buf, MAXDATALEN-1, 0)) == -1) {
				perror("recv");
				exit(1); 
			}
			buf[numbytes] = '\0';

			if(!strcmp(buf, "available")){
				int i;
				char *avamsg = (char *)malloc(MAXDATALEN * sizeof(char)); 
				for(i = 0; i < 6; i++){
					if(avadb[i].reserved == 0){
						strcat(avamsg, avadb[i].index);
						strcat(avamsg, " ");
						strcat(avamsg, avadb[i].dateandtime);
						strcat(avamsg, "\n");
					}

				}
				printf("%s\n\n", avamsg);
				if (send(acpsockfd, avamsg, strlen(avamsg), 0) == -1){
					perror("send");
					exit(1);
				}

				free(avamsg);
				printf("-----------------\n");

			memset(buf, 0, (MAXDATALEN * sizeof(char)));	
			if ((numbytes = recv(acpsockfd, buf, MAXDATALEN-1, 0)) == -1) {
				perror("recv");
				exit(1); 
			}
			buf[numbytes] = '\0';
			printf("%s\n", buf);

			char index[2];
			char *token;
			token = strtok(buf, delim);
			printf("%s\n", token);
			printf("%d\n\n", strcmp(token, "selection"));
			if(!strcmp(token, "selection")){
				token = strtok(NULL, delim);
							printf("%s\n", token);
				strcpy(index, token);
							printf("%s\n", index);
			}else{
				fprintf(stderr, "Error in receive selection index.");
				exit(1);
			}

			char *docinfo = (char *)malloc(MAXDATALEN * sizeof(char));
			memset(docinfo, 0, (MAXDATALEN * sizeof(char)));
			const char *notavailable = "notavailable";
			if((avadb[atoi(index) - 1].reserved) == 0){
				char *reserve = (char *)malloc(MAXDATALEN * sizeof(char));
				memset(reserve, 0, (MAXDATALEN * sizeof(char)));
				strcpy(reserve, index);
				strcat(reserve, " ");
				fputs(reserve ,newfp);
				fclose(newfp);
				printf("reserve%s\n\n", reserve);
				strcpy(docinfo, avadb[atoi(index) - 1].docinfo);
				printf("%s\n\n", docinfo);
				if (send(acpsockfd, docinfo, strlen(docinfo), 0) == -1){
					perror("send");
					exit(1);
				}
			}else{
				if (send(acpsockfd, notavailable, strlen(notavailable), 0) == -1){
					perror("send");
					exit(1);
				}
			}



			}
			close(acpsockfd);
			_exit(EXIT_SUCCESS);
		}else{
			printf("breakpoint\n");
			close(acpsockfd);
			continue; 
		}
	}
	close(sockfd);
	printf("exited\n");
	return 0;
}
