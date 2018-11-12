#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include "comm.h"
#include "util.h"

/* -----------Functions that implement server functionality -------------------------*/

/*
 * Returns the empty slot on success, or -1 on failure
 */
int find_empty_slot(USER * user_list) {
	// iterate through the user_list and check m_status to see if any slot is EMPTY
	// return the index of the empty slot
    int i = 0;
	for (i=0;i < MAX_USER; i++) {
    	if (user_list[i].m_status == SLOT_EMPTY) {
			return i;
		}
	}
	return -1;
}

/*
 * list the existing users on the server shell
 */
int list_users(int idx, USER * user_list)
{
	// iterate through the user list
	// if you find any slot which is not empty, print that m_user_id
	// if every slot is empty, print "<no users>""
	// If the function is called by the server (that is, idx is -1), then printf the list
	// If the function is called by the user, then send the list to the user using write() and passing m_fd_to_user
	// return 0 on success
	int i, flag = 0;
	char buf[MAX_MSG] = {}, *s = NULL;

	/* construct a list of user names */
	s = buf;
	strncpy(s, "---connecetd user list---\n", strlen("---connecetd user list---\n"));
	s += strlen("---connecetd user list---\n");
	for (i = 0; i < MAX_USER; i++) {
		if (user_list[i].m_status == SLOT_EMPTY)
			continue;
		flag = 1;
		strncpy(s, user_list[i].m_user_id, strlen(user_list[i].m_user_id));
		s = s + strlen(user_list[i].m_user_id);
		strncpy(s, "\n", 1);
		s++;
	}
	if (flag == 0) {
		strcpy(buf, "<no users>\n");
	} else {
		s--;
		strncpy(s, "\0", 1);
	}

	if(idx < 0) {
		printf(buf);
		printf("\n");
	} else {
		/* write to the given pipe fd */
		if (write(user_list[idx].m_fd_to_user, buf, strlen(buf) + 1) < 0)
			perror("writing to server shell");
	}

	return 0;
}

/*
 * add a new user
 */
int add_user(int idx, USER * user_list, int pid, char * user_id, int pipe_to_child, int pipe_to_parent)
{                                                                 //server to child  , child to serverd
	// populate the user_list structure with the arguments passed to this function
	// return the index of user added
  user_list[idx].m_pid = pid;
  memcpy(user_list[idx].m_user_id, user_id, MAX_USER_ID);
  user_list[idx].m_fd_to_user = pipe_to_child;
  user_list[idx].m_fd_to_server = pipe_to_parent;
  user_list[idx].m_status = SLOT_FULL;

	return 0;
}

/*
 * Kill a user
 */
 void kill_user(int idx, USER * user_list) {
   close(user_list[idx].m_fd_to_user);
   close(user_list[idx].m_fd_to_server);

   // kill a user (specified by idx) by using the systemcall kill()
   int kill_result = kill(user_list[idx].m_pid, SIGKILL);

   if(kill_result == -1){
     printf("USER is not killed\n" );
   }

 	// then call waitpid on the user
   int status;
   waitpid(user_list[idx].m_pid, &status, WNOHANG);


 }

 /*
  * Perform cleanup actions after the used has been killed
  */
 void cleanup_user(int idx, USER * user_list)
 {
   user_list[idx].m_pid = -1;
   memset(user_list[idx].m_user_id, '\0', MAX_USER_ID); // memset(void *ptr, int x, size_t n)
   user_list[idx].m_fd_to_user = -1;
   user_list[idx].m_fd_to_server = -1;
   user_list[idx].m_status = SLOT_EMPTY;
 }

/*
 * Kills the user and performs cleanup
 */
void kick_user(int idx, USER * user_list) {
	// should kill_user()
	// then perform cleanup_user()
  kill_user(idx, user_list);
  cleanup_user(idx, user_list);

}

/*
 * broadcast message to all users
 */
int broadcast_msg(USER * user_list, char *buf, char *sender)
{
	//iterate over the user_list and if a slot is full, and the user is not the sender itself,
	//then send the message to that user
	//return zero on success

  for (int i = 0; i < MAX_USER; i++) {
    if(strcmp(user_list[i].m_user_id, sender) != 0 && user_list[i].m_status == 0){
      char send[MAX_MSG];
      sprintf(send,"%s : %s",sender, buf );
      int a = write(user_list[i].m_fd_to_user, send, strlen(send));
    }
  }
	return 0;
}

/*
 * Cleanup user chat boxes
 */
void cleanup_users(USER * user_list)
{
	// go over the user list and check for any empty slots
	// call cleanup user for each of those users.
    for (int i =0 ; i < MAX_USER; i ++){
      if(user_list[i].m_status == 0){
        kick_user(i, user_list);
      }
    }
}

/*
 * find user index for given user name
 */
int find_user_index(USER * user_list, char * user_id)
{
	// go over the  user list to return the index of the user which matches the argument user_id
	// return -1 if not found
	int i, user_idx = -1;

	if (user_id == NULL) {
		fprintf(stderr, "NULL name passed.\n");
		return user_idx;
	}
	for (i=0;i<MAX_USER;i++) {
		if (user_list[i].m_status == SLOT_EMPTY)
			continue;
		if (strcmp(user_list[i].m_user_id, user_id) == 0) {
			return i;
		}
	}

	return -1;
}

/*
 * given a command's input buffer, extract name
 */
int extract_name(char * buf, char * user_name)
{
	char inbuf[MAX_MSG];
    char * tokens[16];
    strcpy(inbuf, buf);

    int token_cnt = parse_line(inbuf, tokens, " ");

    if(token_cnt >= 2) {
        strcpy(user_name, tokens[1]);
        return 0;
    }

    return -1;
}

int extract_text(char *buf, char * text)
{
    char inbuf[MAX_MSG];
    char * tokens[16];
    strcpy(inbuf, buf);

    int token_cnt = parse_line(buf, tokens, " ");

    if(token_cnt >= 3) {
        strcpy(text, tokens[2]);
        return 0;
    }

    return -1;
}

/*
 * send personal message
 */
void send_p2p_msg(int idx, USER * user_list, char *buf)
{

	// get the target user by name using extract_name() function
  char target_name[MAX_USER];
  int res  = extract_name(buf, target_name);

	// find the user id using find_user_index()
  int indx = find_user_index(user_list, target_name);
  if(indx == -1){
    // if user not found, write back to the original user "User not found", using the write()function on pipes.
    printf("p2p receiving user not found\n" );
    write(user_list[idx].m_fd_to_user, "User not found \n", sizeof("User not found \n"));
  }else{
    //counting offset for text
    int j =0;
    int count =0;
    while(buf[j] != '\0' && count <2   ){
      if(buf[j] == ' '){
        count++;
      }
      j++;
    }
    buf = buf + j;
    char send[MAX_MSG];
    sprintf(send,"p2p %s : %s",user_list[idx].m_user_id, buf );
    int wirten = write(user_list[indx].m_fd_to_user, send,strlen(send));
  }
}

//takes in the filename of the file being executed, and prints an error message stating the commands and their usage
void show_error_message(char *filename)
{
}


/*
 * Populates the user list initially
 */
void init_user_list(USER * user_list) {

	//iterate over the MAX_USER
	//memset() all m_user_id to zero
	//set all fd to -1
	//set the status to be EMPTY
	int i=0;
	for(i=0;i<MAX_USER;i++) {
		user_list[i].m_pid = -1;
		memset(user_list[i].m_user_id, '\0', MAX_USER_ID); // memset(void *ptr, int x, size_t n)
		user_list[i].m_fd_to_user = -1;
		user_list[i].m_fd_to_server = -1;
		user_list[i].m_status = SLOT_EMPTY;
	}
}

/* ---------------------End of the functions that implementServer functionality -----------------*/


/* ---------------------Start of the Main function ----------------------------------------------*/
int main(int argc, char * argv[])
{
	int nbytes;
	setup_connection("YOUR_UNIQUE_ID"); // Specifies the connection point as argument.

	USER user_list[MAX_USER];
	init_user_list(user_list);   // Initialize user list

	char buf[MAX_MSG];
	fcntl(0, F_SETFL, fcntl(0, F_GETFL)| O_NONBLOCK);
	print_prompt("admin");



  int parentPID = getpid();
	//
  int pipe_user_to_child[2];
  int pipe_child_to_user[2];
  char user_id[MAX_USER_ID];
  int _pid;
  int pipe_SERVER_reading_from_child[2];
  int pipe_SERVER_writing_to_child[2];

	while(1) {
		/* ------------------------YOUR CODE FOR MAIN--------------------------------*/



    // Handling a new connection using get_connection

    if(parentPID == getpid()){

      /************   Parent process ********************/
      /************   parent checking new user ********************/

        int result = get_connection(user_id, pipe_child_to_user, pipe_user_to_child);
        fcntl(pipe_user_to_child[0], F_SETFL, fcntl(0,F_GETFL) | O_NONBLOCK); //nonblocking after get connection
        int index;
        if (result != -1) {
          if(find_user_index(user_list, user_id) != -1){
            printf("User already exist\n" );
            write(pipe_child_to_user[1], "User already exist", sizeof("User already exist"));
            close(pipe_user_to_child[1]);
            close(pipe_child_to_user[1]);
            close(pipe_user_to_child[0]);
            close(pipe_child_to_user[0]);
          } else if((index =find_empty_slot(user_list)) == -1){
            printf("chat is full\n" );
            write(pipe_child_to_user[1], "Chat is full", sizeof("Chat is full"));
            close(pipe_user_to_child[1]);
            close(pipe_child_to_user[1]);
            close(pipe_user_to_child[0]);
            close(pipe_child_to_user[0]);

          }else{
              //adding new user
              pipe(pipe_SERVER_writing_to_child);
              pipe(pipe_SERVER_reading_from_child);
              fcntl(pipe_SERVER_writing_to_child[0], F_SETFL, fcntl(0,F_GETFL) | O_NONBLOCK);
              fcntl(pipe_SERVER_reading_from_child[0], F_SETFL, fcntl(0,F_GETFL) | O_NONBLOCK);

              _pid = fork();
              if(_pid > 0){
                // parent process is closing all unused ends and add new user to list
                add_user(index, user_list, _pid, user_id, pipe_SERVER_writing_to_child[1], pipe_SERVER_reading_from_child[0]);
                printf(" %d new user added %s\n", index, user_id );
                close(pipe_user_to_child[1]);
                close(pipe_child_to_user[1]);
                close(pipe_user_to_child[0]);
                close(pipe_child_to_user[0]);
                close(pipe_SERVER_writing_to_child[0]);
                close(pipe_SERVER_reading_from_child[1]);
              }else{
                // child process closing not used pipe ends
                close(pipe_user_to_child[1]);
                close(pipe_child_to_user[0]);
                close(pipe_SERVER_writing_to_child[1]);
                close(pipe_SERVER_reading_from_child[0]);
              }
            }
          }

        /*********polling: parent read from child ********/
        for(int i = 0 ; i< MAX_USER; i++){
          if(user_list[i].m_status == 0){
            memset(buf, '\0', MAX_MSG);
            int readbytes = read(user_list[i].m_fd_to_server,buf,MAX_MSG);

            if(readbytes >0){
              printf("%s: %s\n",user_list[i].m_user_id, buf );
              int commandTypeU = get_command_type(buf);
              if(commandTypeU == 0){
                int res = list_users(i, user_list);
                if(res != 0){
                  printf("Error occurred in printing list\n" );
                }
              }else if(commandTypeU == 2){
                  send_p2p_msg(i, user_list, buf);
              }else if(commandTypeU == 4){
                kick_user(i, user_list);
              }else if(commandTypeU == 5){
                broadcast_msg(user_list , buf , user_list[i].m_user_id);
                memset(buf, '\0', MAX_MSG);
              }
            }
          }
        }

        /********* parent read from stdin ********/
         int readbytesSTDINp = read(0,buf,MAX_MSG);

         if(readbytesSTDINp >1){
            buf[strlen(buf)-1] = '\0';
            int commandType = get_command_type(buf);

            if(commandType == 0){
              int res = list_users(-1, user_list);
              if(res != 0){
                printf("error occurred in printing list\n" );
              }
            }else if(commandType == 1){
              char kicking_user_name[MAX_USER_ID];
              int foundName = extract_name(buf, kicking_user_name);
              if(foundName == -1){
                printf("Cannot find user name in input\n");
              }else{
                int indx = find_user_index(user_list, kicking_user_name);
                write(user_list[indx].m_fd_to_user, "Admin kicked you", sizeof("Admin kicked you"));
                if(indx != -1){
                  kick_user(indx, user_list);
                }else{
                  printf("User you want to kick does not exist\n");
                }
              }
            }else if(commandType == 4){
              for(int kk = 0; kk < MAX_USER; kk++){
                  if(user_list[kk].m_status == 0){
                    write(user_list[kk].m_fd_to_user, "Admin exited", sizeof("Admin exited"));
                  }
              }
              cleanup_users(user_list);
              exit(-1);
            }else if(commandType ==5){
              char send[MAX_MSG];
              sprintf(send,"admin : %s", buf );
              for(int kk = 0; kk < MAX_USER; kk++){
                  if(user_list[kk].m_status == 0){
                    write(user_list[kk].m_fd_to_user, send, strlen(send));
                  }
              }
              memset(buf, '\0', MAX_MSG);
            }
         }
    }else {
      /************   Child process ********************/
      /********* child read from user ********/
      memset(buf, '\0', MAX_MSG);
      int readbytes2 = read(pipe_user_to_child[0], buf, MAX_MSG);
      if(readbytes2 >0){
        int resultWriret = write(pipe_SERVER_reading_from_child[1],buf, readbytes2);
      } else if(readbytes2 == 0){
        // User end of pipe closed. We need to close all pipe and let parent kick that user form user list
        printf("User %s process is lost connection\n", user_id);
        write(pipe_SERVER_reading_from_child[1], "\\exit", sizeof("\\exit"));
        close(pipe_SERVER_writing_to_child[0]);
        close(pipe_SERVER_reading_from_child[1]);
        close(pipe_child_to_user[0]);
        close(pipe_user_to_child[1]);
      }
      memset(buf, '\0', MAX_MSG);

      /********* child read from parent ********/
      int readbytes3 = read(pipe_SERVER_writing_to_child[0], buf, MAX_MSG);
      if(readbytes3 >0){
        int resultW2 = write(pipe_child_to_user[1],buf, readbytes3);
      }else if(readbytes3 == 0){
        // server end of pipe is closed. So we close all pipes
        close(pipe_SERVER_writing_to_child[0]);
        close(pipe_SERVER_reading_from_child[1]);

        close(pipe_child_to_user[0]);
        close(pipe_user_to_child[1]);
      }
      memset(buf, '\0', MAX_MSG);
    }
    usleep(500);
  }
		/* ------------------------YOUR CODE FOR MAIN--------------------------------*/

}

/* --------------------End of the main function ----------------------------------------*/
