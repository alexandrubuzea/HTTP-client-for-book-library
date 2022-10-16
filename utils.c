#include "utils.h"

/**
 * A function that performs deep copy on a given C string.
 * Parameters: src, the string to be copied.
 */
static char *my_strdup(const char *src)
{
    int len = strlen(src);

    char *copy = (char *)malloc((len + 1) * sizeof(char));

    if (!copy) {
        fprintf(stderr, "Malloc failed at line %s:%d\n", __FILE__, __LINE__);
        exit(-1);
    }

    memcpy(copy, src, (len + 1) * sizeof(char));

    return copy;
}

/**
 * A basic function that checks if a given string can be converted into
 * a positive integer.
 * Parameters: s, the string to be validated.
 */ 
static int is_number(char *s)
{
    int len = strlen(s);

    if (len == 0)
        return 0;

    for (int i = 0; i < len; ++i)
        if (s[i] < '0' || s[i] > '9')
            return 0;
    
    return 1;
}

/**
 * The register user function. The function does not perform registration if
 * an user is already logged in.
 * Parameters: the socket file descriptor and the cookie (to check if an user is logged
 * in).
 */ 
void register_user(int sockfd, char *cookie)
{
    if (cookie) {
        printf("User already logged in!\n");
        return;
    }

    char user[NMAX];

    printf("username:");
    scanf("%s", user);

    char passwd[NMAX];
    printf("password:");
    scanf("%s", passwd);

    JSON_Value *value = json_value_init_object();
    JSON_Object *obj = json_value_get_object(value);

    json_object_set_string(obj, "username", user);
    json_object_set_string(obj, "password", passwd);

    char *info = json_serialize_to_string_pretty(value);

    char *message = compute_post_request((char *)IP, (char *)REGISTER_PATH, (char *)CONTENT_TYPE, &info, 1, NULL, 0, 0);

    send_to_server(sockfd, message);

    char *response = receive_from_server(sockfd);
    char *token = strtok(response, "\n");

    printf("%s\n", token);

    free(message);
    free(response);

    json_free_serialized_string(info);
}

/**
 * The login function. The function does not perform log in if another user is already
 * logged in.
 * Parameters: the socket file descriptor, the cookie (to check if an user is logged in).
 */ 
char *login(int sockfd, char *cookie)
{
    if (cookie) {
        printf("User already logged in!\n");
        return cookie;
    }

    char user[NMAX];

    printf("username:");
    scanf("%s", user);

    char passwd[NMAX];
    printf("password:");
    scanf("%s", passwd);

    JSON_Value *value = json_value_init_object();
    JSON_Object *obj = json_value_get_object(value);

    json_object_set_string(obj, "username", user);
    json_object_set_string(obj, "password", passwd);

    char *info = json_serialize_to_string_pretty(value);

    char *message = compute_post_request((char *)IP, (char *)LOGIN_PATH, (char *)CONTENT_TYPE, &info, 1, NULL, 0, 0);
    send_to_server(sockfd, message);

    char *response = receive_from_server(sockfd);
    char *copy = my_strdup(response);

    char *first_line = strtok(copy, "\n");
    printf("%s\n", first_line);
    free(copy);

    char *start = strstr(response, "connect");

    if (!start) { // unsuccessful login - print response
        char *left = strchr(response, '{');
        char *right = strchr(response, '}');

        right[1] = '\0';

        char *json_string = my_strdup(left);

        JSON_Value *result = json_parse_string(json_string);
        JSON_Object *second_obj = json_value_get_object(result);

        printf("Error: %s\n", json_object_get_string(second_obj, "error"));
        return NULL;
    }
    
    // successful login

    char *tmp = strtok(start, ";");

    char *to_ret = my_strdup(tmp);

    free(message);
    free(response);

    json_free_serialized_string(info);

    return to_ret;
}

/**
 * A function that performs a HTTP GET request in order to obtain the book
 * with the desired ID.
 * Parameters: the socket descriptor, the cookie (to check if the user
 * is logged in), the token (to check if the user accessed the library)
 */
void get_book(int sockfd, char *cookie, char *token)
{
    if (!cookie) {
        printf("User not logged in!\n");
        return;
    }

    if (!token) {
        printf("Cannot get the book - invalid or missing token\n");
        return;
    }

    char id_str[NMAX];

    printf("id=");
    scanf("%s", id_str);

    if (is_number(id_str) == 0) {
        printf("ID is not a number, please try again!\n");
        return;
    }

    int len = strlen(GET_PATH) + strlen(id_str) + 1;
    char *url = malloc((len + 1) * sizeof(char));

    if (!url) {
        fprintf(stderr, "Malloc failed at %s:%d", __FILE__, __LINE__);
        exit(-1);
    }

    memset(url, 0, (len + 1) * sizeof(char));
    strcpy(url, GET_PATH);
    strcat(url, "/");
    strcat(url, id_str);

    char *message = compute_get_request((char *)IP, url, NULL, &token, 1, 1);

    send_to_server(sockfd, message);

    free(message);

    char *response = receive_from_server(sockfd);

    char *start = strchr(response, '{');
    char *stop = strchr(response, '}');

    stop[1] = '\0';

    printf("%s\n", start);

    free(response);
}

/**
 * A function which uses a HTTP GET request in order to obtain all the books
 * on the server.
 * Parameters: the socket descriptor, the cookie (to check if the user
 * is logged in), the token (to check if the user accessed the library)
 */
void get_books(int sockfd, char *cookie, char *token)
{
    if (!cookie) {
        printf("User not logged in!\n");
        return;
    }

    char *message;
    if (!token) {
        printf("Cannot get the list of books - invalid or missing token\n");
        return;
    } else {
        message = compute_get_request((char *)IP, GET_PATH, NULL, &token, 1, 1);
    }

    send_to_server(sockfd, message);

    free(message);

    char *response = receive_from_server(sockfd);

    char *start = strchr(response, '[');
    char *end = strchr(response, ']');
    end[1] = '\0';

    printf("%s\n", start);
}

/**
 * A function that performs a GET request in order to receive a token (to enter the
 * library).
 * Parameters: the socket file descriptor, the cookie (to check if an user is logged in).
 */ 
char *enter_library(int sockfd, char *cookie)
{
    if (!cookie) {
        printf("User not authenticated !\n");
        return NULL;
    }

    char *message = compute_get_request((char *)IP, ENTER_LIBRARY_PATH, NULL, &cookie, 1, 0);

    send_to_server(sockfd, message);
    free(message);

    char *response = receive_from_server(sockfd);

    char *copy = my_strdup(response);
    char *tk = strtok(copy, "\n");
    printf("%s\n", tk);

    free(copy);

    char *start = strchr(response, '{');
    char *stop = strchr(response, '}');

    stop[1] = '\0';
    
    char *json_string = my_strdup(start);

    JSON_Value *result = json_parse_string(json_string);
    JSON_Object *obj = json_value_get_object(result);

    free(response);

    printf("%s\n", json_string);

    return my_strdup(json_object_get_string(obj, "token"));
}

/**
 * A function that performs a POST request in order to add a new book on the server.
 * The informations about the book (title, publisher, page count etc) are read from
 * STDIN.
 * Parameters: the socket descriptor, the cookie (to check if the user is logged in)
 * and the token (to check if the user has logged in).
 */ 
void add_book(int sockfd, char *cookie, char *token)
{
    if (!cookie) {
        printf("User not logged in!\n");
        return;
    }

    JSON_Value *val = json_value_init_object();
    JSON_Object *obj = json_value_get_object(val);

    char buff[NMAX];
    printf("title:");
    fgets(buff, NMAX, stdin);

    if (buff[strlen(buff) - 1] == '\n')
        buff[strlen(buff) - 1] = '\0';
    
    json_object_set_string(obj, "title", buff);

    printf("author:");
    fgets(buff, NMAX, stdin);

    if (buff[strlen(buff) - 1] == '\n')
        buff[strlen(buff) - 1] = '\0';
    
    json_object_set_string(obj, "author", buff);

    printf("genre:");
    fgets(buff, NMAX, stdin);

    if (buff[strlen(buff) - 1] == '\n')
        buff[strlen(buff) - 1] = '\0';
    
    json_object_set_string(obj, "genre", buff);

    printf("publisher:");
    fgets(buff, NMAX, stdin);

    if (buff[strlen(buff) - 1] == '\n')
        buff[strlen(buff) - 1] = '\0';
    
    json_object_set_string(obj, "publisher", buff);

    printf("page_count:");
    fgets(buff, NMAX, stdin);

    if (buff[strlen(buff) - 1] == '\n')
        buff[strlen(buff) - 1] = '\0';
    
    if (is_number(buff) == 0) {
        printf("Invalid number of pages ! Aborting request.\n");
        return;
    }

    json_object_set_number(obj, "page_count", (double)atoi(buff));

    char *info = json_serialize_to_string_pretty(val);

    char *message;
    if (token == NULL)
        message = compute_post_request((char *)IP, (char *)ADD_BOOK_PATH, (char *)CONTENT_TYPE, &info, 1, NULL, 0, 1);
    else
        message = compute_post_request((char *)IP, (char *)ADD_BOOK_PATH, (char *)CONTENT_TYPE, &info, 1, &token, 1, 1);

    send_to_server(sockfd, message);

    char *response = receive_from_server(sockfd);

    char *tk = strtok(response, "\n");
    printf("%s\n", tk);
}

/**
 * A function that performs a DELETE request in order to delete a book from the
 * server.
 * Parameters: the socket file descriptor, the cookie (to check if the user is
 * logged in) and the token (to check if the user accessed the library).
 */ 
void delete_book(int sockfd, char *cookie, char *token)
{
    if (!cookie) {
        printf("User not logged in! \n");
        return;
    }

    if (!token) {
        printf("Invalid token! \n");
        return;
    }

    char id_str[NMAX];

    printf("id=");
    scanf("%s", id_str);

    if (is_number(id_str) == 0) {
        printf("ID is not a number, please try again!\n");
        return;
    }

    int len = strlen(GET_PATH) + strlen(id_str) + 1;
    char *url = malloc((len + 1) * sizeof(char));

    memset(url, 0, (len + 1) * sizeof(char));
    strcat(url, GET_PATH);
    strcat(url, "/");
    strcat(url, id_str);

    char *message = compute_delete_request((char *)IP, url, NULL, &token, 1, 1);

    send_to_server(sockfd, message);

    free(message);

    char *response = receive_from_server(sockfd);

    char *tk = strtok(response, "\n");
    printf("%s\n", tk);

    free(response);
}

/**
 * A function that performs the logout (only if the user is logged in).
 * Parameters: the socket file descriptor and a cookie that shows us if the user is logged in.
 */ 
void logout(int sockfd, char *cookie)
{
    char *message;
    if (!cookie) {
        printf("User already logged out!\n");
        return;
    } else {
        message = compute_get_request((char *)IP, LOGOUT_PATH, NULL, &cookie, 1, 0);
    }

    send_to_server(sockfd, message);

    char *response = receive_from_server(sockfd);

    char *tk = strtok(response, "\n");

    printf("%s\n", tk);

    free(response);
    free(message);
}

/**
 * The exit function that stops the execution of the program. Before exit, the
 * function closes the HTTP connection over TCP.
 */ 
void my_exit(int sockfd)
{
    close_connection(sockfd);
    exit(0);
}