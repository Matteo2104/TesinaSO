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
#include <signal.h>

#define SERV_PORT 5195
#define MAXLINE 100


long int PORT;
char code[6];
char buff[100];
char *dynamic_buff;
char username[15], passwd[15];
char buffer[15], buffer2[50];
char *buffer3;
int sd, len; 
int prio;
struct sockaddr_in addr;
int pid, status;
int x;



void exitf() {
	exit(0);
}

int stampa() {
	signal(SIGINT, exitf);

	memset((void*)code, 0, sizeof(code));
	sprintf(code, "print");
	if (sendto(sd, code, strlen(code), 0, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
		printf("errore nella comunicazione\n");
		return -1;
	}

	system("clear");
	
	printf("Premere CTRL+c in qualunque momento per tornare alla Home\n\n");
	
	while (1) {
		memset((void*)buff, 0, sizeof(buff));
		if (recvfrom(sd, buff, MAXLINE, 0, NULL, NULL) == -1) {
			printf("errore di lettura\n");
			return -1;
		}
		if (strcmp(buff, "end") == 0) {
			break;
		}
		printf("%s", buff);
		
	}
	
	while (1);
}

int ricerca() {
	
	signal(SIGINT, exitf);

	system("clear");
	
	printf("Premere CTRL+c in qualunque momento per tornare alla Home\n\n");
	
	printf("Inserire un nome o un numero da cercare (max 50 caratteri): ");
	fflush(stdout);
	
	scanf("%m[^\n]", &buffer3);
	while (getchar() != '\n');
	
	// controlli su comando inserito da utente
	if (buffer3 == NULL) {
		strcpy(dynamic_buff, "");
	} else {
		dynamic_buff = malloc(10+sizeof(buffer3));
		sprintf(dynamic_buff, "findd %s", buffer3);
	}
	printf("%s\n", dynamic_buff);
	
	if (sendto(sd, dynamic_buff, (size_t)50, 0, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
		printf("errore nella comunicazione\n");
		return -1;
	}
	
	// mi metto in ascolto di eventuali risultati trovati
	while (1) {
		memset((void*)buff, 0, sizeof(buff));
		if (recvfrom(sd, buff, sizeof(buff), 0, (struct sockaddr*)&addr, &len) == -1) {
			printf("errore di lettura\n");
			return -1;
		}
		
		if (strcmp(buff, "end") == 0) {
			break;
		}
		
		printf("%s\n", buff);
	}
	
	// attendo finchè l'utente non esce
	while (1);
	
}

int aggiungi() {

	int i, k;
	
	signal(SIGINT, exitf);
	
	system("clear");
	printf("Premere CTRL+c in qualunque momento per tornare alla Home\n\n");
	
	// chiedo il telefono
	printf("Numero di Telefono (max 15 caratteri): ");
	fflush(stdout);
	scanf("%m[^\n]", &dynamic_buff);
	while (getchar() != '\n');
	
	// controlli su comando inserito da utente
	if (dynamic_buff == NULL) {
		strcpy(buffer, "");
	} else {
		k=0;
		for (i=0;i<strlen(dynamic_buff);i++) {
			if (dynamic_buff[i] != ' ') {
				buffer[k] = dynamic_buff[i];
				k++;
			}
		}
		buffer[k] = '\0';
		//strcpy(buffer, dynamic_buff);
	}
	
	
	// chiedo il nome 
	printf("Nome e Cognome (max 50 caratteri): ");
	fflush(stdout);
	scanf("%m[^\n]", &dynamic_buff);
	while (getchar() != '\n');
	
	// controlli su comando inserito da utente
	if (dynamic_buff == NULL) {
		strcpy(buffer2, "");
	} else {
		strcpy(buffer2, dynamic_buff);				
	}
	
	
	// invio il pacchetto al server
	memset((void*)dynamic_buff, 0, sizeof(buffer)+sizeof(buffer2)+10);
	sprintf(dynamic_buff, "input %s %s", buffer, buffer2);
	//printf("%s - %d\n", dynamic_buff, (int)sizeof(dynamic_buff));
		
	sendto(sd, dynamic_buff, sizeof(buffer)+sizeof(buffer2)+10, 0, (struct sockaddr*)&addr, sizeof(addr));
	
	while (1);
	
}

int login() {
	signal(SIGINT, exitf);
	
	while (1) {
	
		system("clear");
	
		printf("Premere CTRL+c in qualunque momento per tornare alla home\n\n");
		
		printf("Nome Utente: ");
		fflush(stdout);
		scanf("%m[^\n]", &dynamic_buff);
		while (getchar() != '\n');
		
		// controlli su comando inserito da utente
		if (dynamic_buff == NULL) {
			//sprintf(username, "");
			strcpy(username, "");
		} else {
			strcpy(username, dynamic_buff);
		}
		
		
		
		printf("Password: ");
		fflush(stdout);
		scanf("%m[^\n]", &dynamic_buff);
		while (getchar() != '\n');
		
		// controlli su comando inserito da utente
		if (dynamic_buff == NULL) {
			strcpy(username, "");
		} else {
			strcpy(passwd, dynamic_buff);
		}
		
		
		memset((void*)dynamic_buff, 0, sizeof(username)+sizeof(passwd)+10);
		sprintf(dynamic_buff, "login %s %s", username, passwd);
		//printf("%s - %d\n", dynamic_buff, (int)sizeof(dynamic_buff));
		
		sendto(sd, dynamic_buff, sizeof(username)+sizeof(passwd)+10, 0, (struct sockaddr*)&addr, sizeof(addr));	
	
		memset((void*)buff, 0, sizeof(buff));
		if (recvfrom(sd, buff, sizeof(buff), 0, (struct sockaddr*)&addr, &len) == -1) {
			printf("errore di lettura\n");
			return -1;
		}
		
		if (strcmp(buff, "0") == 0) {
			
			x=3;
			while (x > 0) {
				system("clear");
				printf("!!! Nessun utente corrisponde a quello inserito !!! - nuovo tentativo tra %d secondi\n", x);
				sleep(1);
				x--;
			}
			
		} else {
			system("clear");
			break;
		} 
		
	}

	exit(1);

}


int user_login() {
	char *op;
	
	signal(SIGINT, SIG_IGN);
	
	while (1) {
		system("clear");

		printf("Stampa Elenco telefonico  -  Ricerca record  -  Aggiungi record  -  Esci  \n");
		printf("           (1)                    (2)                 (3)            (4)\n\n");
		
		printf("Scegliere operazione: ");
		scanf("%m[^\n]", &op);
		while (getchar() != '\n');
		
		// controlli su comando inserito da utente
		if (op == NULL || strlen(op) != 1) {
			x=3;
			while (x > 0) {
				system("clear");
				printf("comando non riconosciuto - ritorno alla home fra %d secondi\n", x);
				sleep(1);
				x--;
			}
			continue;
		} else {
			op[1] = '\0';
		}
		
		
		if (strcmp(op, "1") == 0) {
			pid = fork();
			if (pid == 0) {
				stampa();
			} else {
				wait(&status);
				continue;
			}
			
			
			//goto home_login;
			
		} else if (strcmp(op, "2") == 0) {
			pid = fork();
			if (pid == 0) {
				ricerca();
			} else {
				wait(&status);
				continue;
			}
			
		} else if (strcmp(op, "3") == 0) {
			pid = fork();
			if (pid == 0) {
				aggiungi();
			} else {
				wait(&status);
				continue;
			}
			//goto home_login;
		
		} else if (strcmp(op, "4") == 0) {
			
			memset((void*)code, 0, sizeof(code));
			sprintf(code, "end");
			sendto(sd, code, strlen(code), 0, (struct sockaddr*)&addr, sizeof(addr));
			
			
			system("clear");
			close(sd);
			break;
				
		} else {
			x=3;
			while (x > 0) {
				system("clear");
				printf("comando non riconosciuto - ritorno alla home fra %d secondi\n", x);
				sleep(1);
				x--;
			}
			
			continue;
			//goto home_login;
		}
	}
	
	exit(0);
}


int user_guest() {
	char *op;
	signal(SIGINT, SIG_IGN);
	
	while (1) {
		system("clear");
		printf("Stampa Elenco Telefonico     -     Log in     -     Esci     \n");
		printf("          (1)                        (2)             (3)   \n\n");
		
		printf("Scegliere operazione: ");
		scanf("%m[^\n]", &op);
		while (getchar() != '\n');
		
		// controlli su comando inserito da utente
		if (op == NULL || strlen(op) != 1) {
			x=3;
			while (x > 0) {
				system("clear");
				printf("comando non riconosciuto - ritorno alla home fra %d secondi\n", x);
				sleep(1);
				x--;
			}
			continue;
		} else {
			op[1] = '\0';
		}
		
			
		if (strcmp(op, "1") == 0) {
			pid = fork();
			if (pid == 0) {
				stampa();
			} else {
				wait(&status);
				continue;
			}
			
			
			
			//goto home_guest;
			
		} else if (strcmp(op, "2") == 0) {
			pid = fork();
			if (pid == 0) {
				login();
			} else {
				wait(&status);
				prio = WEXITSTATUS(status);
				return 0;
			}
			
		} else if (strcmp(op, "3") == 0) {
			
			memset((void*)code, 0, sizeof(code));
			sprintf(code, "end");
			sendto(sd, code, strlen(code), 0, (struct sockaddr*)&addr, sizeof(addr));
		
			close(sd);
			system("clear");
			exit(0);
			
		} else {
			x=3;
			while (x > 0) {
				system("clear");
				printf("comando non riconosciuto - ritorno alla home fra %d secondi\n", x);
				sleep(1);
				x--;
			}
			
			continue;
		}
	}
}

int main() {

	// creo il socket
	if ((sd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		printf("errore di creazione del socket\n");
		return -1;
	}
	
	memset((void*)&addr, 0, sizeof(addr));
	
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(SERV_PORT);
	
	if (inet_pton(AF_INET, "127.000.000.001", &addr.sin_addr) == -1) {
		printf("errore di connessione\n");
		return -1;
	}
	
	// invio al server un messaggio di apertura
	memset((void*)code, 0, sizeof(code));
	sprintf(code, "%d", (int)getpid());
	sendto(sd, code, sizeof(code), 0, (struct sockaddr*)&addr, sizeof(addr));
	
	// AGGIUNGERE controllo sul server
	
	printf("Attendo codice della nuova socket di conferma\n");
	
	// attendo la conferma che il server abbia creato la nuova socket
	memset((void*)code, 0, sizeof(code));
	if (recvfrom(sd, code, sizeof(code), 0, (struct sockaddr*)&addr, &len) == -1) {
		printf("errore di lettura\n");
		return -1;
	}
	
	close(sd);
	// creo nuova socket
	if ((sd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		printf("errore di creazione del socket\n");
		return -1;
	}
	
	
	// connetto al nuovo socket
	memset((void*)&addr, 0, sizeof(addr));
	
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(SERV_PORT+1);
	
	if (inet_pton(AF_INET, "127.000.000.001", &addr.sin_addr) == -1) {
		printf("errore di connessione\n");
		return -1;
	}
	
	
	prio = 0;
	while (1) {
		if (prio == 1) {
			user_login();
		} else if (prio == 0) {
			user_guest();
		}
	}
	
}
