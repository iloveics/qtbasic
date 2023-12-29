#include "expression.h"


QStringList Constant_Expression::to_String()
{
    return QStringList{QString::number(val)};
}

QStringList Identifier_Expression::to_String()
{
    return QStringList{identifier_name +" "+ QString::number(identifier_count)};
}

QStringList Compound_Expression::to_String()
{
    QStringList lchild = lexp->to_String();
    QStringList rchild = rexp->to_String();
    for(int i = 0;i < lchild.size();++i)
    {
        lchild[i].push_front("    ");
    }

    for(int i = 0;i < rchild.size();++i)
    {
        rchild[i].push_front("    ");
    }

    if(op == ADD) return QStringList{"+"}+lchild+rchild;
    if(op == DEL) return QStringList{"-"}+lchild+rchild;
    if(op == MUL) return QStringList{"*"}+lchild+rchild;
    if(op == DIV) return QStringList{"/"}+lchild+rchild;
    if(op == MOD) return QStringList{"MOD"}+lchild+rchild;
    if(op == POW) return QStringList{"**"}+lchild+rchild;
    else throw QString("Error when generate compound expression.");
}

QStringList Unary_Expression::to_String()
{
    QStringList child = child_exp->to_String();
    for(int i = 0;i < child.size();++i)
    {
        child[i].push_front("    ");
    }

    if(op == NEG) return QStringList{"-"}+child;

    else throw QString("Error when generate unary expression.");
}
