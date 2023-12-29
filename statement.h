#ifndef STATEMENT_H
#define STATEMENT_H

#include <QList>
#include "expression.h"
#include "parser.h"
#include "tokenizer.h"

enum Statement_Type{
    REM_STMT,
    LET_STMT,
    PRINT_STMT,
    INPUT_STMT,
    GOTO_STMT,
    IF_STMT,
    END_STMT,
};

class Statement
{
public:
    Statement_Type type;
    Statement(Statement_Type t):type(t){};
    unsigned int count = 0; //记录语句执行次数
    virtual ~Statement(){};
    virtual QStringList to_String() = 0;
    void Count_Clear(){count = 0;};
    void Add_Count(){count ++;};
};

class Rem_Statement : public Statement
{
public:
    QString annotation;
    Rem_Statement(QString a):Statement(REM_STMT),annotation(a){};
    ~Rem_Statement(){};
    QStringList to_String() override;
};

class Let_Statement : public Statement
{
public:
    QString var;
    int var_used_time = 0;
    Conditional_Operator_Type op;
    Expression* exp;
    Let_Statement(QString name,Expression* e):Statement(LET_STMT),var(name),op(EQ),exp(e){};
    ~Let_Statement(){delete exp;}
    QStringList to_String() override;
};

class Print_Statement : public Statement
{
public:
    Expression* exp;
    Print_Statement(Expression* e):Statement(PRINT_STMT),exp(e){};
    ~Print_Statement(){delete exp;}
    QStringList to_String() override;
};

class Input_Statement : public Statement
{
public:
    QString var;
    int var_used_time = 0;
    Input_Statement(QString v):Statement(INPUT_STMT),var(v){};
    ~Input_Statement(){};
    QStringList to_String() override;
};

class Goto_Statement : public Statement
{
public:
    unsigned long line_number;
    Goto_Statement(unsigned long l):Statement(GOTO_STMT),line_number(l){};
    ~Goto_Statement(){};
    QStringList to_String() override;
};

class If_Statement : public Statement
{
public:
    Expression* exp1;
    Expression* exp2;
    Conditional_Operator_Type op;
    unsigned long line_number;
    int if_count = 0;
    int then_count = 0;
    If_Statement(Expression* e1,Expression* e2,Conditional_Operator_Type OP,unsigned long l):Statement(IF_STMT),exp1(e1),exp2(e2),op(OP),line_number(l){};
    ~If_Statement(){delete exp1;delete exp2;}
    QStringList to_String() override;
};

class End_Statement : public Statement
{
public:
    End_Statement():Statement(END_STMT){};
    ~End_Statement(){};
    QStringList to_String() override;
};

Statement* generate_statement(QList<Token*> Token_list);

#endif // STATEMENT_H
