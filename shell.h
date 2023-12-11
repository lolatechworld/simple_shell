#ifndef _SHELL_H_
#define _SHELL_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <limits.h>
#include <fcntl.h>
#include <errno.h>

/* for read/write buffers */
#define READ_BUF_SIZE 1024
#define WRITE_BUF_SIZE 1024
#define BUF_FLUSH -1

/* for command chaining */
#define CMD_NORM0
#define CMD_OR1
#define CMD_AND2
#define CMD_CHAIN3

/* for convert_number() */
#define CONVERT_LOWERCASE1
#define CONVERT_UNSIGNED2

/* 1 if using system getline() */
#define USE_GETLINE 0
#define USE_STRTOK 0

#define HIST_FILE".simple_shell_history"
#define HIST_MAX4096

extern char **environ;


/**
* struct liststr - singly linked list
* @num: the number field
* @str: a string
* @next: points to the next node
*/
typedef struct liststr
{
int num;
char *str;
struct liststr *next;
} list_t;

/**
*struct passinfo - contains pseudo-arguements to pass into a function,
*allowing uniform prototype for function pointer struct
*@arg: a string generated from getline containing arguements
*@argv: an array of strings generated from arg
*@path: a string path for the current command
*@argc: the argument count
*@line_count: the error count
*@err_num: the error code for exit()s
*@linecount_flag: if on count this line of input
*@fname: the program filename
*@env: linked list local copy of environ
*@environ: custom modified copy of environ from LL env
*@history: the history node
*@alias: the alias node
*@env_changed: on if environ was changed
*@status: the return status of the last exec'd command
*@cmd_buf: address of pointer to cmd_buf, on if chaining
*@cmd_buf_type: CMD_type ||, &&, ;
*@readfd: the fd from which to read line input
*@histcount: the history line number count
*/
typedef struct passinfo
{
char *arg;
char **argv;
char *path;
int argc;
unsigned int line_count;
int err_num;
int linecount_flag;
char *fname;
list_t *env;
list_t *history;
list_t *alias;
char **environ;
int env_changed;
int status;

char **cmd_buf; /* pointer to cmd ; chain buffer, for memory management */
int cmd_buf_type; /* CMD_type ||, &&, ; */
int readfd;
int histcount;
} info_t;

#define INFO_INIT {NULL, NULL, NULL, 0, 0, 0, 0, NULL, NULL, NULL, \
NULL, NULL, 0, 0, NULL, 0, 0, 0}

/**
*struct builtin - contains a builtin string and related function
*@type: the builtin command flag
*@func: the function
*/
typedef struct builtin
{
char *type;
int (*func)(info_t *);
} builtin_table;

/* definition_atoi.c */
int check_interactive(info_t *shell_info);
int is_delimiter(char c, char *delimiter);
int _isalpha(int c);
int _atoi(char *s);

/* definition_builtin.c */
int custom_exit(info_t *shell_info);
int custom_cd(info_t *shell_info);
int custom_help(info_t *shell_info);

/* definition_builtin1.c */
int custom_history(info_t *shell_info);
int remove_alias(info_t *shell_info, char *str);
int define_alias(info_t *shell_info, char *str);
int custom_alias(info_t *shell_info);

/* definition_environ.c */
char *get_environment(info_t *shell_info, const char *name);
int custom_environment(info_t *shell_info);
int set_environment(info_t *shell_info);
Int remove_environment(info_t *shell_info);
int populate_environment_list(info_t *shell_info);

/* definition_errors.c */
void print_error_string(char *str);
int print_error_char(char c);
int print_to_fd(char c, int fd);
int print_string_to_fd(char *str, int fd);

/* definition_errors1.c */
int string_to_integer(char *s);
void print_error_message(info_t *shell_info, char *error_str);
int print_decimal(int input, int fd);
char *convert_to_string(long int num, int base, int flags);
void erase_comments(char *buf);

/* definition_exits.c */
char *custom_strncpy(char *dest, char *src, int n);
char *custom_strncat(char *dest, char *src, int n);
char *custom_strchr(char *s, char c);

/* definition_getline.c */
ssize_t get_user_input(info_t *shell_info);
int _getline(info_t *shell_info, char **ptr, size_t *length);
void sigintHandler(__attribute__((unused))int sig_num);

/* definition_getenv.c */
char **get_environment(info_t *shell_info);
int remove_environment_variable(info_t *shell_info, char *variable);
int set_environment_variable(info_t *shell_info, char *variable, char *value);

/* definition_getinfo.c */
void initialize_info(info_t *shell_info);
void populate_info(info_t *shell_info, char **argument_vector);
void release_info(info_t *shell_info, int all);

/* definition_history.c */
char *get_history_filepath(info_t *shell_info);
int save_history(info_t *shell_info);
int load_history(info_t *shell_info);
int add_history_entry(info_t *shell_info, char *buffer, int line_count);
int update_history_count(info_t *shell_info);

/* definition_lists.c */
list_t *add_list_node(list_t **head, const char *data, int index);
list_t *add_list_node_end(list_t **head, const char *data, int index);
size_t print_list_data(const list_t *head);
int delete_node_at_list_index(list_t **head, unsigned int index);
void free_list_nodes(list_t **head_ptr);

/* definition_lists1.c */
size_t get_list_size(const list_t *current_node);
char **list_to_str_array(list_t *head_node);
size_t display_list(const list_t *current_node);
list_t *find_matching_node(list_t *node, char *prefix, char next_char);
ssize_t get_node_position(list_t *head_node, list_t *node);

/* definition_memory.c */
int free_and_nullify(void **pointer_address);

/* definition_parser.c */
int is_executable_command(info_t *shell_info, char *file_path);
char *duplicate_characters(char *path_string, int start, int stop);
char *find_command_path(info_t *shell_info, char *path_string, char *command);

/*definition - vars.c */
int is_command_chain(info_t *shell_info, char *buffer, size_t *position);
void check_cmd_chain(info_t *shell_info, char *buffer,
size_t *position, size_t start, size_t length);
int replace_cmd_alias(info_t *shell_info);
int replace_cmd_vars(info_t *shell_info);
replace_string_content(char **old_content, int  char *new_content);

/*definition - tokenizer.c*/
char **split_string(char *str, char *delimiters);
char **split_string_v2(char *str, char delimiter);

/* definition - string1.c*/
char *_strcpy(char *destination, char *source);
char *_strdup(const char *str);
void _puts(char *str);
int _putchar(char c);

/* definition - string.c*/
int _strlen(char *str);
int _strcmp(char *s1, char *s2);
char *starts_with(const char *haystack, const char *needle);
char *_strcat(char *destination, char *source);

/* definition - shell_loop.c*/
int shell_loop(info_t *shell_info, char **arguments);
int find_builtin(info_t *shell_info);
void find_command(info_t *shell_info);
void fork_command(info_t *shell_info);

/* definition - realloc.c */
char *_memset(char *destination, char byte, unsigned int size);
void ffree(char **string_array);
void *_realloc(void *pointer, unsigned int old_size, unsigned int new_size);

#endif