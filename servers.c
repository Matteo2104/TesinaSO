#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>

#define MAXLINE 10
#define SERV_PORT 5195

int sd;
int len;
long int port;
struct sockaddr_in addr;
char code[6], start[6];
char buff[100], buffer[100], mex[1024];
char *dynamic_buff;
char number[15], contact[50];
char username[15], passwd[15];
char c;
int result;
FILE *file;
char **search, *word, **token;
int count, i, args;
int n_token;

int mystrcmp(char *s1, char *s2) {
	int i;
	if (strlen(s2) > strlen(s1)) {
		return -1;
	} else {
		for (i=0;i<strlen(s2);i++) {
			if (s1[i] != s2[i]) {
				return -1;
			}
		}
		return 0;
	}
	
}

// processo figlio
int serverSon() {
	int fd;
	int port = SERV_PORT+1;
	
	// creo un nuovo socket connesso ad una porta di comunicazione con il client
			
			
			close(sd);
			
			if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
				printf("errore di creazione del socket\n");	
				close(sd);
				exit(0);
			}
		
			memset((void*)&addr, 0, sizeof(addr));
			
			addr.sin_family = AF_INET;
			addr.sin_addr.s_addr = htonl(INADDR_ANY);
			addr.sin_port = htons(port);
			
			// connetto il nuovo socket								
			if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
				printf("errore di binding\n");
				close(sd);
				exit(0);
			}
		

	while (1) {
		printf("figlio - sto ascoltando...\n");
		fflush(stdout);
		
		// mi metto in ascolto di richieste dal client che sto servendo
		len = sizeof(addr);
		memset((void*)mex, 0, sizeof(mex));
		if (recvfrom(fd, mex, 1024, 0, (struct sockaddr*)&addr, &len) == -1) {
			printf("errore di lettura\n");
			close(fd);
			exit(0);
		}
		
		mex[strlen(mex)] = '\0';
		//printf("%s\n", mex);
	
		i=0;
		n_token = 0;
		while (mex[i] != '\0') {
			if (mex[i] == ' ') {
				n_token++;
				if (i!=0 && mex[i-1] == ' ') {
					n_token--;
				}
				if (mex[i+1] == '\0') {
					n_token--;
				}
			}
			i++;
		}
		n_token++;
		
		token = malloc((n_token+1) * sizeof(char*));
		
		token[0] = strtok(mex, " ");
		i=0;
		do {
			i++;
			token[i] = strtok(NULL, " ");
		} while (token[i] != NULL);

		
		// discrimino il codice ricevuto
		if (strcmp(token[0], "login") == 0) {
		
		file = fopen("users", "r");
				
				strcpy(username, token[1]);
				strcpy(passwd, token[2]);
				
				
				printf("ricevuto nome utente e password - %s:%s - verifico che sia all'interno del sistema\n", username, passwd);
				
				// verifico che il nome utente esista all'interno del sistema
				fseek(file, 0, SEEK_SET);
				while (fscanf(file, "%ms", &dynamic_buff) != EOF) {
					if (strcmp(dynamic_buff, username) == 0) {
						// leggo la password corrispondente e la salvo
						fscanf(file, "%ms", &dynamic_buff);
						
						// verifico la password
						if (strcmp(dynamic_buff, passwd) == 0) {
							memset((void*)buff, 0, sizeof(buff));
							//sprintf(buff, "1");
							strcpy(buff, "1");
							sendto(fd, buff, strlen(buff), 0, (struct sockaddr*)&addr, sizeof(addr));
							goto end;
						} else {
							memset((void*)buff, 0, sizeof(buff));
							//sprintf(buff, "0");
							strcpy(buff, "0");
							sendto(fd, buff, strlen(buff), 0, (struct sockaddr*)&addr, sizeof(addr));
							goto end;
						}
						
					}
					
				}
				
				// non esiste nessun nome utente corrispondente, per cui invia un messaggio di fallimento
				memset((void*)buff, 0, sizeof(buff));
				strcpy(buff, "0");
				sendto(fd, buff, strlen(buff), 0, (struct sockaddr*)&addr, sizeof(addr));
			
			
end:		fclose(file);	
			continue;
			
		} else if (strcmp(token[0], "findd") == 0) {
		
			file = fopen("telephone", "r");
			
			
			// se la stringa inserita è la stringa vuota allora la richiesta viene ignorata (poi estendere)
			if (strlen(token[1]) == 0) {
				continue;
			}
			
			/*
			printf("si vogliono cercare le stringhe\n");
			for (i=1;i<n_token;i++) {
				printf("%s\n", token[i]);
			} 
			*/
			
			result=0;
			while (fscanf(file, "%ms", &word) != EOF) {
				word[strlen(word)] = '\0';
				
				for (i=1;i<n_token;i++) {
					if (mystrcmp(word, token[i]) == 0) {
						do {
							fseek(file, -1, SEEK_CUR);
							fread(&c, 1, 1, file);
							fseek(file, -1, SEEK_CUR);
							
						} while (c != '\n');
						
						// per inviare i risultati al client utilizzo un buffer diverso 
						fseek(file, 1, SEEK_CUR);
						memset((void*)buffer, 0, sizeof(buffer));
						fgets(buffer, sizeof(buffer), file);
						buffer[strlen(buffer)] = '\0';
						sendto(fd, buffer, strlen(buffer), 0, (struct sockaddr*)&addr, sizeof(addr));
						
						result++;
						break;
					}
				}
				
			}
			memset((void*)buffer, 0, sizeof(buffer));
			sprintf(buffer, "%d risultati trovati\n", result);
			sendto(fd, buffer, strlen(buffer), 0, (struct sockaddr*)&addr, sizeof(addr));
			
			fclose(file);
		
		
		} else if (strcmp(token[0], "input") == 0) {

			strcpy(number, token[1]);

			for (i=2;i<n_token;i++) {
				if (i!=2) {
					strcat(contact, " ");
				}
				strcat(contact, token[i]);
			}
			
			printf("ricevuto - %s:%s\n", number, contact);
			
			file = fopen("telephone", "a");
			fprintf(file, "%s 		%s\n", number, contact);
			fclose(file);
			
			
		} else if (strcmp(token[0], "print") == 0) {	
			
			file = fopen("telephone", "r");
			fseek(file, SEEK_SET, 0);
	
			memset((void*)buff, 0, sizeof(buff));
			while (fgets(buff, 100, file) != NULL) {
				sendto(fd, buff, strlen(buff), 0, (struct sockaddr*)&addr, sizeof(addr));
				memset((void*)buff, 0, sizeof(buff));
			}
		
			fclose(file);
			
		} else if (strcmp(code, "end") == 0) {
		
			printf("Connessione chiusa con il client\n");
			continue;
			
		}
	}
	
	
}


int main() {
	
	int pid;
	
	struct sockaddr_in new_addr;
	
	
	// creo il socket generico
retry:	if ((sd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		printf("errore di creazione del socket\n");
		return -1;
	}
	
	memset((void*)&addr, 0, sizeof(addr));
	
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(SERV_PORT);
	
	// connetto il socket
	if (bind(sd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
		
		printf("errore di binding - riprovo\n");
		close(sd);
		return -1;
	}	
	
	// creo un nuovo processo server figlio dedicato al client
		pid = fork();
		if (pid == -1) {
			printf("errore di creazione del processo\n");
			return -1;
		}
		if (pid == 0) {
			serverSon();
		}
	
	while (1) {
		printf("padre - sto ascoltando...\n");
		fflush(stdout);
		len = sizeof(addr);
		memset((void*)start, 0, sizeof(start));
		if (recvfrom(sd, start, 6, 0, (struct sockaddr*)&addr, &len) == -1) {
			printf("errore di lettura\n");
			close(sd);
			return -1;
		}
		
		start[5] = '\0';
		
		printf("Ricevuta richiesta di connessione da parte di - %s\n", start);
		
		
		memset((void*)start, 0, sizeof(start));
		sprintf(start, "0");
		sendto(sd, start, strlen(start), 0, (struct sockaddr*)&addr, sizeof(addr));
		
		
		
	}	

	
}
