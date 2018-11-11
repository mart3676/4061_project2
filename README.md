/* CSci4061 F2018 Assignment 2
* section: 02
* date: 11/10/2018
* /

## Documentation for Project 2
###### CSCI 4061
###### Team Members:
* Ganzorig Battur (battu010)
* Patrice Martineau (mart3676)
* Raymond Holidjaja (holid019)

The purpose of our program was to use multi-process architecture to create a local multi-party chat application using 3 different OS system concepts; as laid out in the project description.

Ganzorig handled the creation of the user program as well as the p2p message function, list function, add user function, and broadcast message. Patrice completed the documentation, formatting/style, worked on the broadcast message, add user function . Raymond worked on the kill/cleanup and add user functions, as well as overall debugging.

To compile the program, you can use a makefile located in the project directory to run "make" and "make clean" commands from the terminal. To run the program after compilation, type "./server" to initialize the server process in one terminal, and "./client userid" in another, and replace "userid" with whatever you choose for a username. This will start the chat.

We assumed that after sending a p2p message, the admin would get the notification, but the only ones that can see the content of the message are the sender & receiver. Also, the \list command specifically needs a single space after the command for it to work properly.
