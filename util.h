#ifndef _UTIL_H_
#define _UTIL_H_

int verify_hex(const char *arg, char *colortype, char *varname);

char* expand_path(char* path);
char* trim(const char* str);

#endif /* _UTIL_H_ */
