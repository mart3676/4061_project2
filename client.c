#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include "comm.h"

/* -------------------------Main function for the client ----------------------*/
int main(int argc, char * argv[]) {

	int pipe_user_reading_from_server[2], pipe_user_writing_to_server[2];

	// You will need to get user name as a parameter, argv[1].

	if (connect_to_server("YOUR_UNIQUE_ID", argv[1], pipe_user_reading_from_server, pipe_user_writing_to_server) == -1) {
		exit(-1);
	}

	/* -------------- YOUR CODE STARTS HERE -----------------------------------*/
	char buffer[MAX_MSG];
	int count;
	int flags2 = fcntl(pipe_user_reading_from_server[0], F_SETFL, fcntl(0,F_GETFL) | O_NONBLOCK);
	int flags = fcntl(0, F_SETFL, fcntl(0,F_GETFL) | O_NONBLOCK);
	close(pipe_user_reading_from_server[1]);
	close(pipe_user_writing_to_server[0]);
	// poll pipe retrieved and print it to sdiout

	// Poll stdin (input from the terminal) and send it to server (child process) via pipe
	memset(buffer, '\0', MAX_MSG);
	while(1){

		//************ USER read from Server**************/
		count = read(pipe_user_reading_from_server[0], buffer, MAX_MSG);
	//	printf("reading count %d\n", count);
		if(count > 0 ){
			printf("%s\n", buffer);
			memset(buffer, '\0', MAX_MSG);
		}else if(count == 0){
			close(pipe_user_reading_from_server[0]);
			close(pipe_user_writing_to_server[1]);
			printf("you are exited from the chat\n" );
			exit(-1);
		}


		//************ USER sending to Server**************/
		count = read(0, buffer, MAX_MSG);
		if(count > 0 ){
			buffer[strlen(buffer)-1] = '\0';
			int a = write(pipe_user_writing_to_server[1], buffer, count-1);
			memset(buffer, '\0', MAX_MSG);
		}

	usleep(500);
}
return 0;
	/* -------------- YOUR CODE ENDS HERE -----------------------------------*/
}

/*--------------------------End of main for the client --------------------------*/
