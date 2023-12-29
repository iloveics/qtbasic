#include "parser.h"

QStack<Expression*> Parse_Expression(QList<Token *> &Token_list)
{
    QStack<Operator_Type> op_stack;
    QStack<Expression*> exp_stack;
    for(QList<Token *>::iterator it = Token_list.begin();it != Token_list.end();++it)
    {
        //读取到变量和常量都直接压栈
        if((*it)->type == Identifier)
        {
            exp_stack.push(new Identifier_Expression(static_cast<Identifier_Token*>(*(it))->name));
        }

        else if((*it)->type == Constant)
        {
            exp_stack.push(new Constant_Expression(static_cast<Constant_Token*>(*(it))->val));
        }

        //读取到操作符需要判断当前是否需要合成新的表达式
        else if((*it)->type == Operator)
        {
            Operator_Token* tmp = static_cast<Operator_Token*>(*(it));

            //每次判断栈顶元素是不是比当前要压栈的元素优先级高，因此传入合成函数的操作符是当前同等级操作符的低一级
            if(tmp->op == ADD || tmp->op == DEL){
                try {
                    Syn_Exp(op_stack,exp_stack,LPA);
                } catch (QString &error) {
                    throw  error;
                }}
            if(tmp->op == MUL || tmp->op == DIV || tmp->op == MOD){
                try {
                    Syn_Exp(op_stack,exp_stack,DEL);
                } catch (QString &error) {
                    throw  error;
                }}
            //右括号需要一直判断到左括号为止
            if(tmp->op == RPA)
            {
                try {
                    Syn_Exp(op_stack,exp_stack,LPA);
                } catch (QString &error) {
                    throw  error;
                }
                //如果存在左括号，则当前栈顶元素为左括号，否则为空或者在合成函数中出现操作数不足，两者都会报错
                if(op_stack.isEmpty()){throw QString("missing left parenthese.");}
                op_stack.pop();
            }
            else
            {
                op_stack.push(tmp->op);
            }
        }

        //读到不合法的表达式输入
        else
        {
            throw QString("invalid input of expression.");
        }
    }
    //如果最终输入没有进行合成，比如输入的操作符优先级是升序的，则至少需要一次最低级别的合成
    try {
        Syn_Exp(op_stack,exp_stack,LPA);
    } catch (QString &error) {
        throw  error;
    }
    //如果输入了多余的左括号
    if(!op_stack.isEmpty()) throw QString("missing right parenthese.");
    //如果合成后没有表达式，说明输入为空
    if(exp_stack.isEmpty()) throw QString("input empty expression");
    //如果表达式中没有操作符，如LET A = 9 9 9;
    if(exp_stack.size() >= 2) throw QString("invalid expression.");

    return exp_stack;
}

void Syn_Exp(QStack<Operator_Type> &op_stack, QStack<Expression *> &exp_stack, Operator_Type t)
{
    while(!op_stack.isEmpty() && op_stack.top() > t)
    {
        if(op_stack.top() == NEG)
        {
            if(exp_stack.isEmpty())
            {
                throw QString("missing operand of Operator : Negative.");
            }

            else
            {
                Unary_Expression* syn = new Unary_Expression(op_stack.pop(),exp_stack.pop());
                exp_stack.push(syn);
            }
        }

        else
        {
            if(exp_stack.size() < 2)
            {
                throw QString("missing operand of Operator : " + QString::number(op_stack.top()) + ".");
            }

            else
            {
                Expression* r_exp = exp_stack.pop();
                Expression* l_exp = exp_stack.pop();
                exp_stack.push(new Compound_Expression(op_stack.pop(),l_exp,r_exp));
            }
        }
    }
}
