/*********************************************************************
 *
 * Copyright (C) 2002,  Blekinge Institute of Technology
 *
 * Filename:      precedence.c
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Implementation of C operator precedence.
 *
 * $Id: precedence.c 7664 2006-04-11 08:32:34Z ska $
 *
 ********************************************************************/
#include <stdio.h>      /* fprintf */
#include <stdlib.h>     /* malloc */

#include "stk.h"
#include "precedence.h"

/* The type of operations */
typedef int (*fn_t)(stk_t *p_op_stk, stk_t *p_od_stk, operator_t *p_hand);

/* The operations */
static int fn_not_impl(stk_t *p_op_stk, stk_t *p_od_stk, operator_t *p_hand);
static int fn_err(stk_t *p_op_stk, stk_t *p_od_stk, operator_t *p_hand);
static int fn_execute(stk_t *p_op_stk, stk_t *p_od_stk, operator_t *p_hand);
static int fn_stack(stk_t *p_op_stk, stk_t *p_od_stk, operator_t *p_hand);
static int fn_done(stk_t *p_op_stk, stk_t *p_od_stk, operator_t *p_hand);
static int fn_pop (stk_t *p_op_stk, stk_t *p_od_stk, operator_t *p_hand);

/* Some helpers */
#define N fn_not_impl
#define E fn_err
#define X fn_execute
#define S fn_stack
#define D fn_done
#define P fn_pop
/* The matrix of operations (i.e. function pointers to these):
 *                       -           >=
 *                       +  %        <=
 *                       !  /  -  << <  !=
 *                       ~  *  +  >> >  == &  ^  |  && || (  )  Empty
 */
fn_t fn_matrix[14*14] = {S, X, X, X, X, X, X, X, X, X, X, S, X, X,    /* +,-,!,~        */
			 S, X, X, X, X, X, X, X, X, X, X, S, X, X,    /* /, *, %        */
			 S, S, X, X, X, X, X, X, X, X, X, S, X, X,    /* -, + (add/sub) */
			 S, S, S, X, X, X, X, X, X, X, X, S, X, X,    /* <<, >>         */
			 S, S, S, S, E, E, X, X, X, X, X, S, X, X,    /* <, >, <=, >=   */
			 S, S, S, S, E, E, X, X, X, X, X, S, X, X,    /* !=, ==         */
			 S, S, S, S, S, S, X, X, X, X, X, S, X, X,    /* &              */
			 S, S, S, S, S, S, S, X, X, X, X, S, X, X,    /* ^              */
			 S, S, S, S, S, S, S, S, X, X, X, S, X, X,    /* |              */
			 S, S, S, S, S, S, S, S, S, X, X, S, X, X,    /* &&             */
			 S, S, S, S, S, S, S, S, S, S, X, S, X, X,    /* ||             */
			 S, S, S, S, S, S, S, S, S, S, S, S, P, E,    /* (              */
			 S, E, E, E, E, E, E, E, E, E, E, E, E, E,    /* )              */
			 S, S, S, S, S, S, S, S, S, S, S, S, S, D};   /* Empty          */
#undef N
#undef E
#undef X
#undef S
#undef D
#undef P

/* Perform an unary calculation */
static int do_unary_calc(operator_t *p_op, stk_t *p_od_stk)
{
  operand_t *p_od_top;

  /* Pop the operand */
  if ( !(p_od_top = (operand_t*)stk_pop(p_od_stk)) )
    return fn_err(NULL, NULL, NULL);

#define DO_UNARY_CALC(op)                                   \
      if (p_od_top->type == FLOAT)                          \
	stk_push(p_od_stk, od_create_f(op p_od_top->fval)); \
      else                                                  \
	stk_push(p_od_stk, od_create_i(op p_od_top->ival)); \
      break;
#define DO_UNARY_CALC_NOFP(op)                              \
      if (p_od_top->type == FLOAT)                          \
	return fn_err(NULL, NULL, NULL);                    \
      stk_push(p_od_stk, od_create_i(op p_od_top->ival));   \
      break;

  switch(p_op->type)
    {
    case PLUS:
      DO_UNARY_CALC(+);
    case MINUS:
      DO_UNARY_CALC(-);
    case CPL:
      DO_UNARY_CALC_NOFP(~);
    case LNOT:
      DO_UNARY_CALC_NOFP(!);
    default:
      break;
    }
#undef DO_UNARY_CALC_FP
#undef DO_UNARY_CALC

  od_delete(p_od_top);
  return 0;
}

/* Perform a binary calculation */
static int do_binary_calc(operator_t *p_op, stk_t *p_od_stk)
{
  operand_t *p_od_next;
  operand_t *p_od_top;

  /* Pop the operands */
  if ( !(p_od_top = (operand_t*)stk_pop(p_od_stk)) )
    return fn_err(NULL, NULL, NULL);

  if ( !(p_od_next = (operand_t*)stk_pop(p_od_stk)) )
    return fn_err(NULL, NULL, NULL);

#define DO_BINARY_CALC(op)                                               \
  if ( p_od_next->type == FLOAT || p_od_top->type == FLOAT)              \
    stk_push(p_od_stk, od_create_f(p_od_next->fval op p_od_top->fval));  \
   else                                                                  \
    stk_push(p_od_stk, od_create_i(p_od_next->ival op p_od_top->ival));  \
  break;

#define DO_BINARY_CALC_NOFP(op)                                          \
  if ( p_od_next->type == FLOAT || p_od_top->type == FLOAT)              \
    fn_err(NULL, NULL, NULL);                                            \
   else                                                                  \
    stk_push(p_od_stk, od_create_i(p_od_next->ival op p_od_top->ival));  \
  break;

  switch (p_op->type)
    {
    case ADD:
      DO_BINARY_CALC(+);
    case SUB:
      DO_BINARY_CALC(-);
    case MUL:
      DO_BINARY_CALC(*);
    case DIV:
      DO_BINARY_CALC(/);
    case MOD:
      DO_BINARY_CALC_NOFP(%);

    case LSHIFT:
      DO_BINARY_CALC_NOFP(<<);
    case RSHIFT:
      DO_BINARY_CALC_NOFP(>>);

    case LE:
      DO_BINARY_CALC(<=);
    case GE:
      DO_BINARY_CALC(>=);
    case LT:
      DO_BINARY_CALC(<);
    case GT:
      DO_BINARY_CALC(>);

    case EQ:
      DO_BINARY_CALC(==);
    case NEQ:
      DO_BINARY_CALC(!=);

    case AND:
      DO_BINARY_CALC_NOFP(&);
    case XOR:
      DO_BINARY_CALC_NOFP(^);
    case OR:
      DO_BINARY_CALC_NOFP(|);

    case LAND:
      DO_BINARY_CALC(&&);
    case LOR:
      DO_BINARY_CALC(||);
    default:
      break;
    }
#undef DO_BINARY_CALC_NOFP
#undef DO_BINARY_CALC
  od_delete(p_od_next);
  od_delete(p_od_top);

  return 0;
}


/* Not implemented */
static int fn_not_impl(stk_t *p_op_stk, stk_t *p_od_stk, operator_t *p_hand)
{
  /* Not implemented! */
  fprintf(stderr, "Error: Not implemented!\n");
  return 1;
}

/* Error! */
static int fn_err(stk_t *p_op_stk, stk_t *p_od_stk, operator_t *p_hand)
{
  /* Parse error */
  fprintf(stderr, "Error in parsing!\n");
  return 1;
}

/* Stack the hand */
static int fn_stack(stk_t *p_op_stk, stk_t *p_od_stk, operator_t *p_hand)
{
  stk_push(p_op_stk, p_hand);
  return 1;
}

/* We are done */
static int fn_done(stk_t *p_op_stk, stk_t *p_od_stk, operator_t *p_hand)
{
  return 1;
}

/* Pop the stack (called when we have a RPAR in the hand and a LPAR on the stack) */
static int fn_pop (stk_t *p_op_stk, stk_t *p_od_stk, operator_t *p_hand)
{
  operator_t *p_op;

  /* Pop the stack, do nothing */
  if (! (p_op = (operator_t*)stk_pop(p_op_stk)) )
    {
      fprintf(stderr, "Error: no operator on stack!\n");
      exit(1);
    }
  op_delete(p_op);

  return 1;
}

/* Pop the stack, execute the operation */
static int fn_execute(stk_t *p_op_stk, stk_t *p_od_stk, operator_t *p_hand)
{
  operator_t *p_op;

  /* Pop the operator */
  if ( !(p_op = (operator_t*)stk_pop(p_op_stk)) )
    return fn_err(p_op_stk, p_od_stk, p_hand);

  switch(p_op->type)
    {
      /* Plus, minus, complement and logical not are unary operators */
    case PLUS:
    case MINUS:
    case CPL:
    case LNOT:
      return do_unary_calc(p_op, p_od_stk);

      /* Other are binary */
    default:
      return do_binary_calc(p_op, p_od_stk);
#undef DO_UNARY_CALC
#undef DO_BINARY_CALC
    }
  return 0;
}

/* Execute an operation on the matrix */
void parse_operator(stk_t *p_op_stk, stk_t *p_od_stk, operator_t *p_hand)
{
  operator_t *p_stk_op;
  int ret;

  do
    {
      /* Do this while we keep our hand */

      if ( !(p_stk_op = (operator_t*)stk_peek(p_op_stk)) )
	{
	  fprintf(stderr, "ERROR: Stack emtpy!\n");
	  exit(1);
	}

      ret = (fn_matrix[p_stk_op->idx*14+p_hand->idx])(p_op_stk, p_od_stk, p_hand);
    }
  while(ret == 0);
}



/* -- Operator functions -- */
/* Create an operator */
operator_t *op_create(sym_t type)
{
  operator_t *p_out;

  if ( !(p_out = malloc(sizeof(operator_t)) ))
    {
      fprintf(stderr, "operator_create: malloc failed\n");
      exit(1);
    }
  p_out->type = type;
  p_out->idx = type / 4;

  return p_out;
}

/* Delete an operator */
void op_delete(operator_t *p_op)
{
  free(p_op);
}


/* -- Operand functions -- */
/* Create an operand */
operand_t *od_create(void)
{
  operand_t *p_out;

  if ( !(p_out = malloc(sizeof(operand_t)) ))
    {
      fprintf(stderr, "od_create: malloc failed\n");
      exit(1);
    }

  return p_out;
}

/* Create a integer operand */
operand_t *od_create_i(int_t val)
{
  operand_t *p_out = od_create();

  p_out->type = INT;
  p_out->ival = val;
  p_out->fval = (double_t)val;

  return p_out;
}

/* Create a float operand */
operand_t *od_create_f(double_t val)
{
  operand_t *p_out = od_create();

  p_out->type = FLOAT;
  p_out->fval = val;
  p_out->ival = (int_t)val;

  return p_out;
}

/* Delete an operand */
void od_delete(operand_t *p_od)
{
  free(p_od);
}
