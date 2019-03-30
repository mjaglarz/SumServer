#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>

#define MAX_SIZE 2048

void message(int sockfd){ 																										
	char error[] = "ERROR\r\n";
	int bytesWritten = write(sockfd, &error, sizeof(error) - 1);
	if(bytesWritten == -1){
		perror("WRITE ERROR");
		exit(1);
	}
}

size_t intToString(char *string, int number){ 												
	char *ptr = string; 
  int value = number;

  do{ 
  	*ptr = value % 10 + '0';
		ptr++;
    value /= 10;
  }while(value);

	size_t writtenCharacters = ptr - string; 

  while(--ptr > string){ 
  	char temp = *string;
    *string = *ptr;
		string++;
    *ptr = temp;
  }

  return writtenCharacters;
}

void chat(int sockfd){
	char buffer[MAX_SIZE];
	memset(&buffer, 0, sizeof(buffer));

	int bufferSize = 0; 
	while(1){
		int bytesRead = read(sockfd, buffer, sizeof(buffer));
		if(bytesRead == -1){
		  perror("READ ERROR");
		  exit(1);
		}

		if(bytesRead != 0){
			bufferSize = bytesRead;
		}

		if(bufferSize == 0){
			break;
		}

		int lastElement; 
		bool correctLine = false; 
		for(int i = 0; i < bufferSize && correctLine == false; ++i){
			if((buffer[i] == '\r') && (i+1 != bufferSize)){
				if(buffer[i+1] == '\n'){ 
					lastElement = i;
					correctLine = true;
				}
			}
		}

		if(correctLine == false){
			memset(&buffer, 0, sizeof(buffer));
			bufferSize = 0;
			message(sockfd);
			continue;
		}

		if(lastElement == 0){
			memmove(buffer, buffer + 2, bufferSize - 2);
			bufferSize = bufferSize - 2;
			message(sockfd);
			continue;
		}

		if(buffer[0] == ' ' || buffer[lastElement-1] == ' '){
			memmove(buffer, buffer + lastElement + 2, bufferSize - lastElement - 2);
			bufferSize = bufferSize - lastElement - 2;
			message(sockfd);
			continue;
		}

		for(int i = 0; i < lastElement && correctLine == true; ++i){
			if(buffer[i] == 32 && i+1 != lastElement){
				if(buffer[i+1] == 32){ 
					correctLine = false;
					continue;
				}
			}

			if((buffer[i] < 48 && buffer[i] != 32) || buffer[i] > 57){
				correctLine = false;
			}
		}

		if(correctLine == false){
			memmove(buffer, buffer + lastElement + 2, bufferSize - lastElement - 2);
			bufferSize = bufferSize - lastElement - 2;
			message(sockfd);
			continue;
		}

		char string[lastElement+1];
		strncpy(string, buffer, lastElement);
		string[lastElement] = '\0';

		int sum = 0;
		char *token = strtok(string, " "); 
		bool convsersionError = false; 
		while(token != NULL && convsersionError == false){
			errno = 0;
			long number = strtol(token, (char**)NULL, 10); 
			if(errno != 0 || number > INT_MAX){ 
				convsersionError = true;
				break;
			}

			if(sum <= INT_MAX - number){
				sum += number;
			}else{
				convsersionError = true;
				break;
			}
			token = strtok(NULL, " ");
		}

		if(convsersionError){
			memmove(buffer, buffer + lastElement + 2, bufferSize - lastElement - 2);
			bufferSize = bufferSize - lastElement - 2;
			message(sockfd);
			continue;
		}

		size_t writtenCharacters = intToString(string, sum); //Konwertuję sumę na string.
		string[writtenCharacters] = '\r';
		string[writtenCharacters+1] = '\n';

		int bytesWritten = write(sockfd, &string, writtenCharacters + 2);
		if(bytesWritten == -1){
			perror("WRITE ERROR");
			exit(1);
		}

		if(lastElement + 2 != bufferSize){
			memmove(buffer, buffer + lastElement + 2, bufferSize - lastElement - 2);
			bufferSize = bufferSize - lastElement - 2;
		}else{
			memset(&buffer, 0, sizeof(buffer));
			bufferSize = 0;
		}
	}
}

int main(int argc, char* argv[]){
	int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(serverSocket == -1){
		perror("SOCKET ERROR");
		exit(1);
	}

	struct sockaddr_in serverAddress;
	memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(2019);
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);

	if(bind(serverSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) == -1){
		perror("BIND ERROR");
		exit(1);
	}

	if(listen(serverSocket, 1) == -1){
		perror("LISTEN ERROR");
		exit(1);
	}

	while(1){
		int clientSocket = accept(serverSocket, NULL, 0);
		if(clientSocket == -1){
			perror("ACCEPT ERROR");
			exit(1);
		}

		chat(clientSocket);

		int closeSocket = close(clientSocket);
		if(closeSocket == -1){
			perror("CLOSE ERROR");
			exit(1);
		}
	}

	return 0;
}
