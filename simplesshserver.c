#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[])
{

    if (argc < 2) {
        printf("usage: %s port\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[1]);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in addr;

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port); 
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    if (bind(sockfd, (struct sockaddr *) &addr, sizeof (addr)) < 0) {
        perror("bind");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    if (listen(sockfd, 128) < 0) {
        perror("listen");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    int num_client = 0;

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t addrlen = sizeof (client_addr);
        int clientfd = accept(sockfd, (struct sockaddr *) &client_addr, &addrlen);
        if (clientfd < 0) {
            perror("accept");
            close(sockfd);
            exit(EXIT_FAILURE);
        }

        num_client++;

        printf("Connection #%d from %s:%d\n",
                num_client,
                inet_ntoa(client_addr.sin_addr),
                ntohs(client_addr.sin_port));

	int fd[2];
	int stde;
	pipe(fd); 
	int pid = fork();

	if (pid == 0) {
	
            close(fd[0]);
	    dup2(fd[1], STDOUT_FILENO); // write stdout to the pipe
	    dup2(clientfd, STDIN_FILENO); // get stdin from the client, even after the exec call!
            dup2(stde, STDERR_FILENO); 
	 
	    char *arg[2];
	    arg[0] = "bash";
	    arg[1] = "-i";
	    arg[2] = (char *) NULL;

	    if(execvp("bash", arg)){
		perror("exec");
		printf("WE GOT A PROBLEM\n");
		}
	}

	else { // parent, gets result of executed command
	    close(fd[1]);
	    ssize_t read_from_call = 0;
	    do {
	    char buf[4096];
	    read_from_call = read(fd[0], buf, 4096);
	    write(clientfd, buf, read_from_call); // print the result of the call

	    } while(read_from_call > 0);
	}
    }

    printf("bind complete, now waiting\n");
    fgetc(stdin);

    printf("done\n");

    close(sockfd);

    return EXIT_SUCCESS;
}
