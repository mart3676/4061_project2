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


	// poll pipe retrieved and print it to sdiout

	// Poll stdin (input from the terminal) and send it to server (child process) via pipe
	while(1){
	int flags = fcntl(0, F_SETFL, fcntl(0,F_GETFL) | O_NONBLOCK);
	int count;
	char buffer[MAX_MSG];
	count = read(0, buffer, MAX_MSG);

	if (count < 0 && errno == EAGAIN){
		// no data available
		// set errno to EAGAIN
	} else if(count >= 0 ){
		// there is data
		int a = write(pipe_user_writing_to_server[1], buffer, count);
		//printf(" what I read is %s, I read %d bytes, I wrote %d bytes\n", buffer, count, a);

	} else {
		// some error occurred during reading
 	}
}
return 0;
	/* -------------- YOUR CODE ENDS HERE -----------------------------------*/
}

/*--------------------------End of main for the client --------------------------*/
