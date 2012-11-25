/*********************************************************************
 *
 * Copyright (C) 2004,  Blekinge Institute of Technology
 *
 * Filename:      utils.c
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Utilities for the level creator
 *
 * $Id: utils.c 474 2004-08-20 19:33:10Z ska $
 *
 ********************************************************************/
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <assert.h>
#include <stdint.h>

#define __USE_BSD
#include <string.h>
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>

#include <file_handling.h>
#include <tiles.h>

char first_nb_char(char *p_in)
{
  int i;

  for (i=0; i<strlen(p_in); i++)
    {
      if (p_in[i] != ' ' || p_in[i] != '\t')
	return p_in[i];
    }

  /* Nothing on the line */
  return '\0';
}

/* Do we want to accept a line? */
int line_is_acceptable(char *p_in)
{
  int nb = first_nb_char(p_in);

  if (nb == '\n' ||
      nb == '\r' ||
      nb == '\0' ||
      nb == '#')
    return 0;

  return 1;
}

#define TOKENS " \t,\n"

/* Create a vector of strings from a string (tokenize it) */
char **strs_get(char *p_in, int *p_n)
{
  char *p_dup = strdup(p_in);
  char **pp;
  char *p_tok;
  int i=0;

  assert(p_dup);
  *p_n = 0;

  p_tok = strtok(p_dup, TOKENS);
  while (p_tok)
    {
      p_tok = strtok(NULL, TOKENS);
      (*p_n)++;
    }
  if (!(*p_n))
    return NULL;
  pp = malloc(sizeof(char**)*(*p_n));

  free(p_dup);
  p_dup = strdup(p_in);
  assert(p_dup);

  p_tok = strtok(p_dup, " ,\n");
  while (p_tok)
    {
      pp[i++] = strdup(p_tok);
      p_tok = strtok(NULL, " ,\n");
    }

  return pp;
}

/* Free a vector of strings */
void strs_free(char **pp, int n)
{
  int i;

  for (i=0; i<n; i++)
    free(pp[i]);

  free(pp);
}

char *create_file_str(const char *dirname, const char *file_name, int file_n)
{
  int len = strlen(dirname) + strlen(file_name) + 20;
  char *p_out = malloc(len);
  struct stat buf;

  memset(p_out, 0, len);
  sprintf(p_out, "%s/%s.%d", dirname, file_name, file_n);

  if (lstat(p_out, &buf) < 0)
    {
      if (errno == ENOENT)
	{
	  free(p_out);
	  return NULL;
	}
    }

  return p_out;
}
