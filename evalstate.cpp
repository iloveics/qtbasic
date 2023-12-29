#include "evalstate.h"

Evalstate::Evalstate()
{

}

long long Evalstate::Calculate_Expression(Expression * exp)
{
    switch (exp->type) {
    case Constant_EXP:
    {
        return Get_Constant_Val(exp);
    }
    case Identifier_EXP:
    {
        long long val;
        try {
            val = Get_Variable_Val(exp);
        } catch (QString &error) {
            throw error;
        }
        return val;
    }
    case Unary_EXP:
    {
        Unary_Expression* tmp = static_cast<Unary_Expression*>(exp);
        if(tmp->op != NEG)
        {
            throw QString ("Error : using undefined operator.");
        }
        long long ret;
        try {
            ret = Calculate_Expression(tmp->child_exp);
        } catch (QString &error) {
            throw error;
        }

        return 0 - ret;
    }
    case Compound_EXP:
    {
        Compound_Expression* tmp = static_cast<Compound_Expression*>(exp);
        switch (tmp->op) {
        case ADD:
        {
            long long ret;
            try {
                ret = Calculate_Expression(tmp->lexp) + Calculate_Expression(tmp->rexp);
            } catch (QString &error) {
                throw error;
            }

            return ret;
        }
        case DEL:
        {
            long long ret;
            try {
                ret = Calculate_Expression(tmp->lexp) - Calculate_Expression(tmp->rexp);
            } catch (QString &error) {
                throw error;
            }

            return ret;
        }
        case MUL:
        {
            long long ret;
            try {
                ret = Calculate_Expression(tmp->lexp) * Calculate_Expression(tmp->rexp);
            } catch (QString &error) {
                throw error;
            }

            return ret;
        }
        case DIV:
        {
            long long rval;
            try {
                rval = Calculate_Expression(tmp->rexp);
            } catch (QString &error) {
                throw error;
            }

            if(rval == 0) throw QString("Error : Divide Zero.");

            long long ret;
            try {
                ret = Calculate_Expression(tmp->lexp) / Calculate_Expression(tmp->rexp);
            } catch (QString &error) {
                throw error;
            }

            return ret;
        }
        case MOD:
        {
            long long rval;
            try {
                rval = Calculate_Expression(tmp->rexp);
            } catch (QString &error) {
                throw error;
            }

            if(rval == 0) throw QString("Error : MOD Zero.");

            long long lval;
            try {
                lval = Calculate_Expression(tmp->lexp);
            } catch (QString &error) {
                throw error;
            }

            if(rval < 0)
            {
                long long abs_rval = -rval;

                if(lval >= abs_rval && lval % abs_rval == 0) return 0;

                long long mod = lval / abs_rval;
                mod ++;
                return lval + (mod * rval);
            }

            else return lval % rval;
        }
        case POW:
        {
            long long rval;
            try {
                rval = Calculate_Expression(tmp->rexp);
            } catch (QString &error) {
                throw error;
            }

            long long lval;
            try {
                lval = Calculate_Expression(tmp->lexp);
            } catch (QString &error) {
                throw error;
            }

            if(rval == 0 && lval == 0) throw QString("Error : Zero Pow Zero.");

            return pow(lval,rval);
        }
        default:
            throw QString("invalid input of expression.");
        }
    }
    }
}

long long Evalstate::Get_Constant_Val(Expression *exp)
{
    Constant_Expression* constant = static_cast<Constant_Expression*>(exp);
    return constant->val;
}

long long Evalstate::Get_Variable_Val(Expression *exp)
{
    Identifier_Expression* var = static_cast<Identifier_Expression*>(exp);
    if(!var_map.contains(var->identifier_name))
        throw QString("Error : variable " + var->identifier_name + " used without being initialized.");

    for(QMap<QString,int>::iterator it = var_used_time.begin();it != var_used_time.end();++it)
    {
        if(it.key() == var->identifier_name)
        {
            it.value()++;
            var->identifier_count = it.value();
        }
    }

    return var_map[var->identifier_name];
}

long long Evalstate::Get_Variable_Val_From_Name(QString name)
{
    if(!var_map.contains(name))
        throw QString("Error : Variable not exist.");

    return var_map[name];
}

void Evalstate::Set_Var(QString name, long long value)
{
    var_map.insert(name,value);
    if(!var_used_time.contains(name))
    {
        var_used_time.insert(name,0);
    }
}

void Evalstate::Update_used_time(Expression *exp)
{
    switch (exp->type) {
    case Constant_EXP:break;
    case Identifier_EXP:
    {
        Identifier_Expression* var = static_cast<Identifier_Expression*>(exp);
        for(QMap<QString,int>::iterator it = var_used_time.begin();it != var_used_time.end();++it)
        {
            if(it.key() == var->identifier_name)
            {
                var->identifier_count = it.value();
                break;
            }
        }
        break;
    }
    case Unary_EXP:
    {
        Unary_Expression* e = static_cast<Unary_Expression*>(exp);
        Update_used_time(e->child_exp);
        break;
    }
    case Compound_EXP:
    {
        Compound_Expression* e = static_cast<Compound_Expression*>(exp);
        Update_used_time(e->lexp);
        Update_used_time(e->rexp);
        break;
    }
    }

    return;
}
