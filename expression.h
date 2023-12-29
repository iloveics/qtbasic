#ifndef EXPRESSION_H
#define EXPRESSION_H

#include "tokenizer.h"
#include <QString>
#include <QStringList>

enum Exp_Type{
    Constant_EXP,
    Identifier_EXP,
    Compound_EXP,
    Unary_EXP,
};

class Expression
{
public:
    Expression(Exp_Type t):type(t){};
    virtual ~Expression(){};

    Exp_Type type;
    virtual QStringList to_String() = 0;
};

class Constant_Expression : public Expression
{
public:
    long long val;
    Constant_Expression(long long v):Expression(Constant_EXP),val(v){};
    QStringList to_String() override;
    ~Constant_Expression(){};
};

class Identifier_Expression : public Expression
{
public:
    QString identifier_name;
    int identifier_count = 0;
    Identifier_Expression(QString s):Expression(Identifier_EXP),identifier_name(s){};
    QStringList to_String() override;
    ~Identifier_Expression(){};
};

class Compound_Expression : public Expression
{
public:
    Operator_Type op;
    Expression* lexp;
    Expression* rexp;
    Compound_Expression(Operator_Type OP,Expression* l,Expression* r):Expression(Compound_EXP),op(OP),lexp(l),rexp(r){};
    QStringList to_String() override;
    ~Compound_Expression(){delete lexp;delete rexp;}
};

class Unary_Expression : public Expression
{
public:
    Operator_Type op;
    Expression* child_exp;
    Unary_Expression(Operator_Type OP,Expression* c):Expression(Unary_EXP),op(OP),child_exp(c){};
    QStringList to_String() override;
    ~Unary_Expression(){delete child_exp;}
};

#endif // EXPRESSION_H
