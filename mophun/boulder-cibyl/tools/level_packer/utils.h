
#ifndef __LEVEL_CREATOR__UTILS_H__
#define __LEVEL_CREATOR__UTILS_H__

char *create_file_str(const char *dirname, const char *file_name, int file_n);
void strs_free(char **pp, int n);
char **strs_get(char *p_in, int *p_n);
int line_is_acceptable(char *p_in);
char first_nb_char(char *p_in);


ssize_t getline (char **LINEPTR, size_t *N, FILE *STREAM);






#endif /* !__LEVEL_CREATOR__UTILS_H__ */
