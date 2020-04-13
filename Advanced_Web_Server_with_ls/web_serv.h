#ifndef _WEB_SERV_H_
#define _WEB_SERV_H_
# include <stdio.h>
# include <dirent.h>
# include <stdlib.h>//header for dynamic alloacation
# include <string.h>
# include <unistd.h>
# include <sys/stat.h>//header for struct stat
# include <pwd.h>//header for password 
# include <grp.h>//header for group
# include <time.h>///header for time
# include <string.h>
# include <fnmatch.h>//header for fnmatch

typedef struct _Node{
 char    name[255];
        struct stat buf;//for stat information
        struct _Node *next;//for keep next node
        struct _Node *prev;//for keep preve node
}Node;
Node* except_node(Node *a, Node *b);
int is_it_wild_path(char *a, char**b,int c);
char* inspecting_wild(char*a);
void free_node(Node * array);
void print_size(struct stat a,int hflg,FILE* file);
void print_time(struct stat a,FILE* file);
void print_groupID(struct stat a,FILE* file);
void print_userID(struct stat a,FILE* file);
void print_permit(struct stat a, FILE* file);
void print_type(struct stat  a, FILE* file);
Node* reverse_node_swap(Node* p);
Node* sort_list(Node *p, struct dirent *a, struct stat * b, int Sflg);
int web_serv_info_print(char* path,char* home);

#endif
