// Copyright 2022 Buzea Alexandru-Mihai-Iulian 321CAb

#ifndef UTILS_H_
#define UTILS_H_

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "requests.h"
#include "helpers.h"
#include "buffer.h"
#include "parson.h"

/**
 * Useful macros for handling the urls of the server, IP address, port and
 * other similar known data.
 */ 
#define NMAX 100
#define IP "34.241.4.235"
#define PORT 8080
#define REGISTER_PATH "/api/v1/tema/auth/register"
#define CONTENT_TYPE "application/json"
#define LOGIN_PATH "/api/v1/tema/auth/login"
#define GET_PATH "/api/v1/tema/library/books"
#define LOGOUT_PATH "/api/v1/tema/auth/logout"
#define ENTER_LIBRARY_PATH "/api/v1/tema/library/access"
#define ADD_BOOK_PATH GET_PATH

/**
 * The functions that basically solve our homework, one for each task
 */ 
void register_user(int sockfd, char *cookie);
char *login(int sockfd, char *cookie);
void get_book(int sockfd, char *cookie, char *token);
void get_books(int sockfd, char *cookie, char *token);
char *enter_library(int sockfd, char *cookie);
void add_book(int sockfd, char *cookie, char *token);
void delete_book(int sockfd, char *cookie, char *token);
void logout(int sockfd, char *cookie);
void my_exit(int sockfd);

#endif