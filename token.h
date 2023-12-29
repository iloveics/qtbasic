#ifndef TOKEN_H
#define TOKEN_H

#include <QObject>
#include <QString>

enum Token_Type{
    Key_Word,
    Identifier,
    Constant,
    Operator,
    Conditional_Operator,
    String,
    Command,
};

enum Key_Word_Type{
    REM,
    LET,
    PRINT,
    INPUT,
    GOTO,
    IF,
    THEN,
    END,
};

enum Operator_Type{
    RPA,//右括号
    LPA,//左括号
    ADD,//加
    DEL,//减
    MUL,//乘
    DIV,//除
    MOD,//模
    NEG,//负号
    POW,//幂
};

enum Conditional_Operator_Type{
    EQ,//=
    NE,//<>
    LT,//<=
    GT,//>=
    LE,//<
    GE,//>
};

enum Command_Type{
    RUN,
    LOAD,
    LIST,
    CLEAR,
    HELP,
    QUIT,
};

class Token
{
public:
    Token_Type type;

    Token(Token_Type t):type(t){};

    virtual QString to_String() = 0;

    virtual ~Token(){};
};

// 实现Token子类

class Key_Word_Token : public Token
{
public:
    Key_Word_Type value;
    Key_Word_Token(Key_Word_Type t):Token(Key_Word),value(t){};
    ~Key_Word_Token(){};
    QString to_String() override;
};

class Operator_Token : public Token
{
public:
    Operator_Type op;
    Operator_Token(Operator_Type OP):Token(Operator),op(OP){};
    ~Operator_Token(){};
    QString to_String() override;
};

class Conditional_Operator_Token : public Token
{
public:
    Conditional_Operator_Type op;
    Conditional_Operator_Token(Conditional_Operator_Type OP):Token(Conditional_Operator),op(OP){};
    ~Conditional_Operator_Token(){};
    QString to_String() override;
};

class Identifier_Token : public Token
{
public:
    QString name;
    Identifier_Token(QString n):Token(Identifier),name(n){};
    ~Identifier_Token(){};
    QString to_String() override;
};

class Constant_Token : public Token
{
public:
    long long val;
    Constant_Token(int v):Token(Constant),val(v){};
    ~Constant_Token(){};
    QString to_String() override;
};

class String_Token : public Token
{
public:
    QString content;
    String_Token(QString s):Token(String),content(s){};
    ~String_Token(){};
    QString to_String() override;
};

class Command_Token : public Token
{
public:
    Command_Type cmd;
    Command_Token(Command_Type c):Token(Command),cmd(c){};
    ~Command_Token(){};
    QString to_String() override;
};


#endif // TOKEN_H
