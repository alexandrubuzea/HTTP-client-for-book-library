// Copyright 2022 Buzea Alexandru-Mihai-Iulian 321CAb

#include <stdio.h>
#include "command_type.h"
#include "utils.h"

int main(void)
{
    char command[NMAX];
    char *cookie = NULL, *token = NULL;
    char *tmp;

    while (fgets(command, NMAX, stdin)) {
        if (command[strlen(command) - 1] == '\n')
            command[strlen(command) - 1] = '\0';

        // open a new connection - HTTP is stateless
        int sockfd = open_connection((char *)IP, PORT, AF_INET, SOCK_STREAM, 0);

        // each command type
        switch (parse_from_string(command)) {
            case REGISTER:
                register_user(sockfd, cookie);
                break;
            
            case LOGIN:
                if (!cookie) {
                    cookie = login(sockfd, cookie);
                } else {
                    free(cookie);
                    cookie = login(sockfd, cookie);
                }

                break;

            case GET_BOOK:
                get_book(sockfd, cookie, token);
                break;

            case GET_BOOKS:
                get_books(sockfd, cookie, token);
                break;
            
            case ENTER_LIBRARY:
                tmp = enter_library(sockfd, cookie);
                if (!token) {
                    free(token);
                    token = tmp;
                }

                break;
            
            case ADD_BOOK:
                add_book(sockfd, cookie, token);
                break;
            
            case DELETE_BOOK:
                delete_book(sockfd, cookie, token);
                break;
            
            case LOGOUT:
                logout(sockfd, cookie);
                token = NULL;
                cookie = NULL;
                break;
            
            case EXIT:
                my_exit(sockfd);
                break;
            
            case OTHER:
                break;
        }

        // Close the connection on the TCP socket.
        close_connection(sockfd);
    }

    return 0;
}