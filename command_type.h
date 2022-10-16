// Copyright 2022 Buzea Alexandru-Mihai-Iulian 321CAb

#ifndef COMMAND_TYPE_H_
#define COMMAND_TYPE_H_

#include <string.h>

/**
 * My enum type in order to identify each possible command type
 */ 
typedef enum {
    REGISTER,
    LOGIN,
    GET_BOOK,
    GET_BOOKS,
    ENTER_LIBRARY,
    ADD_BOOK,
    DELETE_BOOK,
    LOGOUT,
    EXIT,
    OTHER
} command_type_t;

/**
 * A function that performs the conversion from a string to an enum type.
 */ 
command_type_t parse_from_string(char *command);

#endif