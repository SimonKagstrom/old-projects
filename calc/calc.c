/*********************************************************************
 *
 * Copyright (C) 2002,  Blekinge Institute of Technology
 *
 * Filename:      calc.c
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Implementation of C-style operator precedence
 *
 * $Id: calc.c 4749 2005-09-22 06:52:25Z ska $
 *
 ********************************************************************/
#include <stdio.h>    /* printf */
#include <stdlib.h>   /* malloc */
#include <errno.h>    /* errno */
#include <string.h>

#include "stk.h"
#include "precedence.h"

/* --- Token parsing --- */
typedef struct
{
  char *p;                /* The current position in the string */
  int last_token_is_od;   /* If last token was an operand (needed for unary operators etc) */
} token_state_t;

/* Initialise a token state */
static void ts_init(token_state_t *p_state, char *p_str)
{
  p_state->p = p_str;
  p_state->last_token_is_od = -1;
}

/* Create a operand (and update the token state)*/
static void *ts_create_op(token_state_t *p_state, sym_t op)
{
  p_state->last_token_is_od = 0;

  return (void*)op_create(op);
}

/* Get the next token */
static void *ts_next_token(token_state_t *p_state, int *p_is_op)
{
  *p_is_op = 1; /* Assume operators */

  /* Check next character */
  p_state->p++;
  switch(p_state->p[-1])
    {
    case ' ':
      return ts_next_token(p_state, p_is_op);
    case '\'':
      {
	p_state->p+=2;
	p_state->last_token_is_od = 1;
	*p_is_op = 0;
	return (void*)od_create_i((int)p_state->p[-2]);
      }
    /* Ugly, but ANSI C does not allow case '0'..'9': */
    case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8':case '9':
      {
	char *p_end;
	double_t fval;
	int_t ival;
	int base=0;

	*p_is_op = 0; /* Operand, not operator */
	if (p_state->p[0] == 'b')
	  {
	    base = 2;
	    p_state->p+=2;
	  }
	ival = strtol(p_state->p-1, &p_end, base);
	if (errno == ERANGE)
	  ival = strtoul(p_state->p-1, &p_end, base);
	if (p_end[0] == '.')
	  {
	    /* Double value */
	    fval = strtod(p_state->p-1, &p_end);
	    p_state->p = p_end;
	    p_state->last_token_is_od = 1;
	    return (void*)od_create_f(fval);
	  }

	p_state->p = p_end;
	p_state->last_token_is_od = 1;
	return (void*)od_create_i(ival);
      }
    case '+':
      {
	/* Either binary ADD or unary PLUS */
	if (p_state->last_token_is_od == 1)
	  return (void*)ts_create_op(p_state, ADD);
	return (void*)ts_create_op(p_state, PLUS);
      }
    case '-':
      {
	if (p_state->last_token_is_od == 1)
	  return (void*)ts_create_op(p_state, SUB);
	return (void*)ts_create_op(p_state, MINUS);
      };
    case '*':
      return (void*)ts_create_op(p_state, MUL);
    case '/':
      return (void*)ts_create_op(p_state, DIV);
    case '%':
      return (void*)ts_create_op(p_state, MOD);
    case '>':
      {
	if (p_state->p[0] == '=')
	  {
	    p_state->p++;
	    return (void*)ts_create_op(p_state, GE);
	  }
	else if (p_state->p[0] == '>')
	  {
	    p_state->p++;
	    return (void*)ts_create_op(p_state, RSHIFT);
	  }
	return (void*)ts_create_op(p_state, GT);
      }
    case '<':
      {
	if (p_state->p[0] == '=')
	  {
	    p_state->p++;
	    return (void*)ts_create_op(p_state, LE);
	  }
	else if (p_state->p[0] == '<')
	  {
	    p_state->p++;
	    return (void*)ts_create_op(p_state, LSHIFT);
	  }
	return (void*)ts_create_op(p_state, LT);
      }
    case '&':
      {
	if (p_state->p[0] == '&')
	  {
	    p_state->p++;
	    return (void*)ts_create_op(p_state, LAND);
	  }
	return (void*)ts_create_op(p_state, AND);
      }
    case '|':
      {
	if (p_state->p[0] == '|')
	  {
	    p_state->p++;
	    return (void*)ts_create_op(p_state, LOR);
	  }
	return (void*)ts_create_op(p_state, OR);
      }
    case '=':
      {
	if (p_state->p[0] != '=')
	  return NULL;
	p_state->p++;
	return (void*)ts_create_op(p_state, EQ);
      }
    case '!':
      {
	if (p_state->p[0] == '=')
	  {
	    p_state->p++;
	    return (void*)ts_create_op(p_state, NEQ);
	  }
	return (void*)ts_create_op(p_state, LNOT);
      }
    case '^':
      return (void*)ts_create_op(p_state, XOR);
    case '~':
      return (void*)ts_create_op(p_state, CPL);
    case '(':
      return (void*)ts_create_op(p_state, LPAR);
    case ')':
      p_state->last_token_is_od = 1; /* Not exactly true, but ... */
      return (void*)op_create(RPAR);
    case '\n': /* End of the string */
    case '\0':
      return (void*)ts_create_op(p_state, EMPTY);
    default:
      fprintf(stdout, "PARSE ERROR: ts_next_token: %c not handled.\n", p_state->p[-1]);
      return NULL;
    }

  /* Should not be reached */
  return NULL;
}

/* Do the calculation, return an operand with the result */
operand_t *do_calc(char *str)
{
  token_state_t state;
  operand_t *p_res;
  stk_t op_stk; /* operator stack */
  stk_t od_stk; /* operand stack */
  void *p_tok;
  int is_op;

  stk_fill(&op_stk);
  stk_fill(&od_stk);

  stk_push(&op_stk, (void*)op_create(EMPTY));
  ts_init(&state, str);

  /* Parse the string */
  while( (p_tok = ts_next_token(&state, &is_op)) )
    {
      /* Is this an operator? */
      if (is_op)
	{
	  operator_t *p_op = (operator_t*)p_tok;

	  /* While we should keep our hand */
	  parse_operator(&op_stk, &od_stk, p_op);

	  if (p_op->type == EMPTY)
	    break;
	}
      else
	{
	  operand_t *p_od = (operand_t*)p_tok;

	  /* Push the operand on the operand stack */
	  stk_push(&od_stk, p_od);
	}
    }
  p_res = stk_pop(&od_stk);

  /* check for failures - the stack should be empty! */
  if (stk_peek(&od_stk))
    {
      od_delete(p_res);
      return NULL;
    }

  return p_res;
}

#define BIN  1
#define CHAR 2

int main(int argc, char **argv)
{
  const char *fmt = "%d\n";
  operand_t *p_res;
  int type = 0;
  int arg = 1;

  if (argc < 2)
    return 1;

  /* We have some options */
  if (argc >= 2)
    {
      /* Default is to output in decimal format */
      if (strcmp(argv[1], "-h") == 0)
	{
	  /* Output in hex format */
	  fmt = "0x%0.8x\n";
	  arg++;
	}
      else if (strcmp(argv[1], "-b") == 0)
	{
	  /* Output in binary format */
	  fmt = NULL;
	  type = BIN;
	  arg++;
	}
      else if (strcmp(argv[1], "-c") == 0)
	{
	  /* Output in character (as a string) format */
	  fmt = NULL;
	  type = CHAR;
	  arg++;
	}
      else if (strcmp(argv[1], "-o") == 0)
	{
	  /* Output in octal format */
	  fmt = "0%o\n";
	  arg++;
	}

      /* Nothing to compute, skip it */
      if (arg > argc-1)
	return 1;
    }

  /* Operator stack is initially empty */
  if ( !(p_res = do_calc(argv[arg])) )
    {
      fprintf(stderr, "error: no result\n");
      return 1;
    }

  /* Print the result */
  switch(p_res->type)
    {
    case FLOAT:
      printf("%.16f\n", p_res->fval);
      break;
    case INT:
      if (type == BIN)
	{
	  int i;

	  /* Binary special case */
	  printf("0b");
	  for (i=31; i>=0; i--)
	    {
	      if ((i % 4) == 3 && i != 31)
		printf(" ");
	      printf("%d", (p_res->ival & (1<<i)) != 0);
	    }
	  printf("\n");
	}
      else if (type == CHAR)
	{
	  printf("\"%c%c%c%c\"\n", (char)((p_res->ival & 0xff000000) >> 24), (char)((p_res->ival & 0x00ff0000) >> 16),
		 (char)((p_res->ival & 0x0000ff00) >> 8), (char)(p_res->ival & 0x000000ff));
	}
      else
	printf(fmt, p_res->ival);
    }
  od_delete(p_res);

  return 0;
}
