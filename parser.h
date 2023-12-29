#ifndef PARSER_H
#define PARSER_H

#include "tokenizer.h"
#include "expression.h"
#include <QStack>


QStack<Expression*> Parse_Expression(QList<Token*> &Token_list);
void Syn_Exp(QStack<Operator_Type> &op_stack,QStack<Expression*> &exp_stack,Operator_Type t);

#endif // PARSER_H
