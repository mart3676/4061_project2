/* CSci4061 F2018 Assignment 2
* section: 98
* date: 11/10/2018
* name: Ganzorig Battur, Patrice Martineau, Raymond Holidjaja
* id: battu010, mart3676, holid019
* /

The purpose of our program was to use multi-process architecture to create a local multi-party chat application using 3 different OS system concepts. It will have a single centralized server. Moreover, it will have one parent process and several child processes that each communicate with their associated user. 

Ganzorig handled the creation of the user program as well as the p2p message function, list function, add user function, and broadcast message. Patrice completed the documentation, formatting/style, worked on the broadcast message, add user function . Raymond worked on the kill/cleanup and add user functions, as well as overall debugging.

To compile the program, you can use a makefile located in the project directory to run "make" and "make clean" commands from the terminal. To run the program after compilation, type "./server" to initialize the server process in one terminal, and "./client userid" in another, and replace "userid" with whatever you choose for a username. This will start the chat.

There are 5 syntax that we implement: \list, \p2p <username> <message>, \exit, \kick, <any-other-text>. List will basically list all of the active users that are still connected to the server. P2P will be only available on the user so they can send messages to a particular user. 
Exit will basically remove the user from the process. If exits is called from the server, the child will also kicked out from the server. If any other text, it will just broadcast the chat to everyone. 
  
For error handling, we must to make sure that all of the pipes are closed. If all of the pipes are not closed, we could not do the kick function properly. 
