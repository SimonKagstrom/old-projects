/*********************************************************************
 *
 * Copyright (C) 2002,  Blekinge Institute of Technology
 *
 * Filename:      precedence.h
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Definitions for operator precedence.
 *
 * $Id: precedence.h,v 1.2 2003/01/16 13:41:16 ska Exp $
 *
 ********************************************************************/
#ifndef PRECEDENCE_H
#define PRECEDENCE_H

typedef enum
{
  /* Unary ops */
  CPL=0, LNOT=1, PLUS=2, MINUS=3,

  /* Arithmetic operations */
  MUL=4, DIV=5, MOD=6,
  ADD=8, SUB=9,

  /* Shifts */
  LSHIFT=12, RSHIFT=13,

  /* Comparisons */
  LT=16, GT=17, LE=18, GE=19,
  EQ=20, NEQ=21,

  /* Bitwise ops */
  AND=24,
  XOR=38,
  OR=32,

  /* Logical comparisons */
  LAND=36,
  LOR=40,

  /* Other */
  LPAR=44,
  RPAR=48,
  EMPTY=52
} sym_t;

typedef enum
{
  INT, FLOAT
} od_t;

typedef long int int_t;
typedef double double_t;

/* Operands */
typedef struct
{
  int_t     ival;
  double_t  fval;
  od_t      type;
} operand_t;

/* Operators */
typedef struct
{
  sym_t type;
  int idx;
} operator_t;

/* -- Prototypes -- */
/**
 * Create a new operator.
 *
 * @param type the operator to create.
 *
 * @return a pointer to the newly created operator.
 */
operator_t *op_create(sym_t type);

/**
 * Delete an operator.
 *
 * @param p_op the operator to delete.
 */
void op_delete(operator_t *p_op);

/**
 * Create a new integer operand.
 *
 * @param val the value of the operand.
 *
 * @return a pointer to the new operand.
 */
operand_t *od_create_i(int_t val);

/**
 * Create a new float operand.
 *
 * @param val the value of the operand.
 *
 * @return a pointer to the new operand.
 */
operand_t *od_create_f(double_t val);

/**
 * Delete an operand.
 *
 * @param p_od a pointer to the operand.
 */
void od_delete(operand_t *p_od);



/**
 * Parse a hand. This should be called for each operator in the
 * token-stream. Operands should be pushed on the operand stack, as
 * they are found.
 *
 * @param p_op_stack the operator stack.
 * @param p_od_stack the operand stack.
 * @param p_hand the operator "in the hand" (i.e. the latest operator).
 */
void parse_operator(stk_t *p_op_stk, stk_t *p_od_stk, operator_t *p_hand);


#endif /* PRECEDENCE_H */
