#ifndef EVALSTATE_H
#define EVALSTATE_H

#include "expression.h"
#include "parser.h"
#include <QMap>
#include <cmath>

class Evalstate
{
public:
    Evalstate();
    long long Calculate_Expression(Expression* exp_stack);
    long long Get_Constant_Val(Expression* exp);
    long long Get_Variable_Val(Expression* exp);
    long long Get_Variable_Val_From_Name(QString name);
    void Set_Var(QString name,long long value);
    void Update_used_time(Expression* exp);
    void Clear(){var_map.clear();var_used_time.clear();}
    ~Evalstate(){Clear();}

    QMap<QString,int> var_used_time;

private:
    QMap<QString,long long> var_map;
};

#endif // EVALSTATE_H
