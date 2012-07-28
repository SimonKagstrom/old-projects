/*********************************************************************
 *
 * Copyright (C) 2002,  Blekinge Institute of Technology
 *
 * Filename:      stk.h
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Definition of a generic stack
 *
 * $Id: stk.h,v 1.1.1.1 2002/10/22 13:19:15 ska Exp $
 *
 ********************************************************************/
#ifndef STK_H
#define STK_H

typedef struct s_elem
{
  void *p;          /* Pointer to stack elem (i.e. either operator or operand) */

  /* The stack cares about this */
  struct s_elem *p_next;   /* Next entry on the stack */
} elem_t;

typedef struct
{
  elem_t *p_top;
} stk_t;


/* --- Operations --- */
/**
 * Create a new stack.
 *
 * @param p_stk a pointer to the stack-object.
 */
int stk_fill(stk_t *p_stk);

void  stk_push(stk_t *p_stk, void *p_elem);
void *stk_peek(stk_t *p_stk);
void *stk_pop(stk_t *p_stk);

#endif /* STK_H */
