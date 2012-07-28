/*********************************************************************
 *
 * Copyright (C) 2002-2003,  Blekinge Institute of Technology
 *
 * Filename:      stk.c
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Implementation of a generic stack.
 *
 * $Id: stk.c 7665 2006-04-11 08:32:42Z ska $
 *
 ********************************************************************/
#include <string.h> /* memset */
#include <stdlib.h> /* malloc */
#include <stdio.h>  /* fprintf */

#include "stk.h"

int stk_fill(stk_t *p_stk)
{
  memset(p_stk, 0, sizeof(stk_t));

  return 0;
}

void stk_push(stk_t *p_stk, void *p_entry)
{
  elem_t *p_elem = malloc(sizeof(elem_t));

  if (!p_elem)
    {
      fprintf(stderr, "stk_push: Error in creating element\n");
      exit(1);
    }
  p_elem->p = p_entry;
  p_elem->p_next = p_stk->p_top;

  p_stk->p_top = p_elem;
}

void *stk_peek(stk_t *p_stk)
{
  return p_stk->p_top ? p_stk->p_top->p : NULL;
}

void *stk_pop(stk_t *p_stk)
{
  elem_t *p_elem = p_stk->p_top;

  /* Something on the stack? */
  if (p_elem)
    {
      void *p_ret = p_elem->p;

      /* Pop the stack, return the memory */
      p_stk->p_top = p_stk->p_top->p_next;
      free (p_elem);

      return p_ret;
    }

  /* Stack empty */
  return NULL;
}
