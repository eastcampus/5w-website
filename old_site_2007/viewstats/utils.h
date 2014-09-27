#ifndef _UTILS_H
#define _UTILS_H

#include <stdarg.h>

#define BUFFSIZE 2048

int strcmpi(char *s1, char *s2);
int Log_open(char *filename);
void Log_append(char *format,...);
void Log_vappend(char *format, va_list ap);
void Log_error(char *filename, char *fmt, ...);
void Log_close();
int Log_setto(FILE *f);
char* freadln(FILE *f);
char* sprintfalloc(char *format, ...);
void insertfile(char filename[]);
void stolower(char *s);
long fsize(char *filename);
char* strstrend(char *haystack, char *needle);
int strsuffix(char *in, char *suffix);
long safeatol(char *s);
int safeatoi(char *s);

#endif
