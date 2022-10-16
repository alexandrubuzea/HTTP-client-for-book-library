// Copyright 2022 Buzea Alexandru-Mihai-Iulian 321CAb

#include "command_type.h"

command_type_t parse_from_string(char *command)
{
    if (!strcmp(command, "register"))
        return REGISTER;
    
    if (!strcmp(command, "login"))
        return LOGIN;

    if (!strcmp(command, "get_book"))
        return GET_BOOK;

    if (!strcmp(command, "get_books"))
        return GET_BOOKS;

    if (!strcmp(command, "enter_library"))
        return ENTER_LIBRARY;

    if (!strcmp(command, "add_book"))
        return ADD_BOOK;
    
    if (!strcmp(command, "delete_book"))
        return DELETE_BOOK;
    
    if (!strcmp(command, "logout"))
        return LOGOUT;
    
    if (!strcmp(command, "exit"))
        return EXIT;
    
    return OTHER;
}