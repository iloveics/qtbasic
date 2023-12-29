#include "statement.h"
#include <type_traits>



Statement *generate_statement(QList<Token *> Token_list)
{
    //情况1：Tokenlist为空，说明用户输入为空
    if(Token_list.isEmpty()) return nullptr;

    //情况2：Tokenlist非空，在主界面判断了Command的情况，剩下的合法输入只有Statement
    //检查Tokenlist的第一个元素是不是Keyword
    QList<Token *>::iterator it = Token_list.begin();
    if((*it)->type == Key_Word)
    {
        Key_Word_Token* tmp = static_cast<Key_Word_Token*>((*it));
        switch (tmp->value) {
        case REM:
        {
           if(it +1 == Token_list.end())
               return new Rem_Statement(nullptr);
           else
           {
               String_Token* ret = static_cast<String_Token*>(*(it+1));
               return new Rem_Statement(ret->content);
           }
        }
        case LET:
        {
            if(it +1 == Token_list.end()) throw QString("missing content of statement type : LET.");

            if((*(it +1))->type != Identifier) throw QString("missing identifier of statement type : LET.");

            Identifier_Token* var = static_cast<Identifier_Token*>(*(it +1));

            if(it +2 == Token_list.end() || (*(it +2))->type != Conditional_Operator || ((*(it +2))->type == Conditional_Operator && static_cast<Conditional_Operator_Token*>(*(it +2))->op != EQ))
                throw QString("missing conditional operator '=' in statement : LET");

            Conditional_Operator_Token* op = static_cast<Conditional_Operator_Token*>(*(it +2));

            if(it +3 == Token_list.end()) throw QString("missing expression of statement type : LET.");

            //将Tokenlist前三项删除，后面的项用于构造expression
            for(int i = 0;i < 3;++i)
            {
                Token_list.removeAt(0);
            }

            QStack<Expression*> exp_stack;
            try {
                exp_stack = Parse_Expression(Token_list);
                //复原Tokenlist，防止产生不可预料的后果，如果try引发异常则Tokenlist不用复原
                Token_list.prepend(op);
                Token_list.prepend(var);
                Token_list.prepend(tmp);
            } catch (QString &error) {
                throw error;
            }
            return new Let_Statement(var->name,exp_stack.top());
        }
        case PRINT:
        {
            if(it +1 == Token_list.end()) throw QString("missing content of statement type : Print.");

            Token_list.removeAt(0);

            QStack<Expression*> exp_stack;

            try {
                exp_stack = Parse_Expression(Token_list);
                Token_list.prepend(tmp);
            } catch (QString &error) {
                throw error;
            }
            return new Print_Statement(exp_stack.top());
        }
        case INPUT:
        {
            if(it +1 == Token_list.end()) throw QString("missing content of statement type : Input.");

            if((*(it +1))->type != Identifier) throw QString("missing identifier of statement type : Input.");

            Identifier_Token* var = static_cast<Identifier_Token*>(*(it +1));

            if(it +2 != Token_list.end()) throw QString("invalid input of statement type : Input.Too much input.");

            return new Input_Statement(var->name);
        }
        case GOTO:
        {
            if(it +1 == Token_list.end()) throw QString("missing line number of statement type : Goto.");

            if((*(it +1))->type != Constant) throw QString("invalid input of statement type : Goto.Not a valid line number.");

            Constant_Token* line_number = static_cast<Constant_Token*>(*(it +1));

            if(line_number->val < 0) throw QString("error : line number must not be negative.");

            if(it +2 != Token_list.end()) throw QString("invalid input of statement type : Goto.Too much input.");

            return new Goto_Statement(line_number->val);
        }
        case IF:
        {
            if(it +1 == Token_list.end()) throw QString("missing line number of statement type : If.");

            //首先查找有没有Then
            QList<Token*>::iterator then_finder = Token_list.begin();
            then_finder ++;
            while(then_finder != Token_list.end())
            {
                if((*then_finder)->type == Key_Word && static_cast<Key_Word_Token*>(*then_finder)->value == THEN)
                    break;

                then_finder ++;
            }

            //没有找到Then，引发异常
            if(then_finder == Token_list.end()) throw QString("missing 'Then' in statement : If.");

            //此时then_finder即为then的位置
            //其次查找条件操作符>,<,>=,<=,=,<>是否存在且唯一
            QList<Token*>::iterator cdt_op_finder = Token_list.begin();
            cdt_op_finder ++;
            while(cdt_op_finder != Token_list.end())
            {
                if((*cdt_op_finder)->type == Conditional_Operator)
                    break;

                cdt_op_finder ++;
            }
            //情况1：没有找到
            if(cdt_op_finder == Token_list.end()) throw QString("missing conditional operator in statement : If.");

            //情况2：找到了但是紧跟着if或者then，此时说明缺少一个至少一个exp
            if(cdt_op_finder-1 == it || cdt_op_finder +1 == then_finder) throw QString("missing expression in statement : If.");

            //情况3：找到了但是不止一个
            QList<Token*>::iterator more_finder = cdt_op_finder +1;
            while(more_finder != Token_list.end())
            {
                if((*more_finder)->type == Conditional_Operator)
                    break;

                more_finder ++;
            }

            if(more_finder != Token_list.end()) throw QString("error : more than one conditional operator in statement : If.");

            //情况4：找到了但是在Then后面，可以合并到Then后面不是Constant一起处理
            QList<Token*>::iterator line_number_finder = then_finder +1;

            if(line_number_finder == Token_list.end()) throw QString("missing line number in statement : If");

            if((*line_number_finder)->type != Constant) throw QString("error : line number must be constant.");

            if(line_number_finder +1 != Token_list.end()) throw QString("invalid input of statement type : If.Too much input.");

            QList<Token*> exp1_token;
            for(QList<Token*>::iterator pos = Token_list.begin()+1;pos != cdt_op_finder;++pos)
            {
                exp1_token.append(*pos);
            }

            QList<Token*> exp2_token;
            for(QList<Token*>::iterator pos = cdt_op_finder +1;pos != then_finder;++pos)
            {
                exp2_token.append(*pos);
            }

            Conditional_Operator_Token* op = static_cast<Conditional_Operator_Token*>(*cdt_op_finder);

            Constant_Token* line_number = static_cast<Constant_Token*>(*line_number_finder);

            if(line_number->val < 0) throw QString("error : line number must not be negative.");

            QStack<Expression*> exp1;
            try {
                exp1 = Parse_Expression(exp1_token);
            } catch (QString &error) {
                throw error;
            }

            QStack<Expression*> exp2;
            try {
                exp2 = Parse_Expression(exp2_token);
            } catch (QString &error) {
                throw error;
            }

            return new If_Statement(exp1.top(),exp2.top(),op->op,line_number->val);
        }
        case END:
        {
            if(it +1 != Token_list.end()) throw QString("invalid input of statement End.Too much input.");
            return new End_Statement();
        }
        default:
            throw QString("invalid input of statement.Incorrect Keyword.");
        }
    }

    throw QString("invalid input of statement.");
}

QStringList Rem_Statement::to_String()
{
    return QStringList{"REM "+QString::number(count)}+QStringList{"    "+annotation};
}

QStringList Let_Statement::to_String()
{
    QStringList exp_str;
    try {
        exp_str = exp->to_String();
    } catch (QString &error) {
        throw error;
    }

    for(int i = 0;i < exp_str.size();++i)
    {
        exp_str[i].push_front("    ");
    }

    QStringList var_str = QStringList{"    " +var+" "+QString::number(var_used_time)};

    if(op == EQ) return QStringList{"LET = "+QString::number(count)}+var_str+exp_str;

    else throw QString("Error when generate Let statement.");
}

QStringList Print_Statement::to_String()
{
    QStringList exp_str;
    try {
        exp_str = exp->to_String();
    } catch (QString &error) {
        throw error;
    }

    for(int i = 0;i < exp_str.size();++i)
    {
        exp_str[i].push_front("    ");
    }

    return QStringList{"PRINT "+QString::number(count)}+exp_str;
}

QStringList Input_Statement::to_String()
{
    QStringList var_str = QStringList{"    " +var+" "+QString::number(var_used_time)};
    return QStringList{"INPUT "+QString::number(count)}+var_str;
}

QStringList Goto_Statement::to_String()
{
    return QStringList{"GOTO "+QString::number(count)}+QStringList{"    "+QString::number(line_number)};
}

QStringList If_Statement::to_String()
{
    QStringList exp1_str;
    QStringList exp2_str;
    try {
        exp1_str = exp1->to_String();
    } catch (QString &error) {
        throw error;
    }

    for(int i = 0;i < exp1_str.size();++i)
    {
        exp1_str[i].push_front("    ");
    }

    try {
        exp2_str = exp2->to_String();
    } catch (QString &error) {
        throw error;
    }

    for(int i = 0;i < exp2_str.size();++i)
    {
        exp2_str[i].push_front("    ");
    }

    QStringList op_str;
    if(op == EQ) op_str = QStringList{"    ="};
    else if(op == NE) op_str = QStringList{"    <>"};
    else if(op == LT) op_str = QStringList{"    <"};
    else if(op == LE) op_str = QStringList{"    <="};
    else if(op == GT) op_str = QStringList{"    >"};
    else if(op == GE) op_str = QStringList{"    >="};
    else throw QString("Error when generate if then statement.");

    return QStringList{"IF THEN "+QString::number(if_count)+" "+QString::number(then_count)}+exp1_str+op_str+exp2_str+QStringList{"    " +QString::number(line_number)};
}

QStringList End_Statement::to_String()
{
    return QStringList{"END "+QString::number(count)};
}
