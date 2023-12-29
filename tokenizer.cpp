#include "tokenizer.h"

QList<Token*> Scan_Token(QString user_input)
{
    QList<Token*> Token_list;
    QString::iterator it = user_input.begin();
    while(it != user_input.end())  //如果input为空，tokenlist为空
    {
        if((*it) == ' ') //跳过输入开头的空格
        {
            it ++;
            continue;
        }

        //情况1：识别到REM后的注释
        if(!Token_list.isEmpty() && ((*(Token_list.end()-1)))->type == Key_Word)
        {
            Key_Word_Token* tmp = static_cast<Key_Word_Token*>((*(Token_list.end()-1)));
            if(tmp->value == REM)
            {
                QString input = nullptr;
                while(it != user_input.end())
                {
                   input.append(*it);
                   ++it;
                }
                Token_list.append(new String_Token(input));
                return Token_list;
            }
        }

        //情况2：识别到数字
        if(it->isDigit())
        {
            long long val = it->digitValue();
            while(it+1 != user_input.end() && (it+1)->isDigit())
            {
                ++it;
                val = val * 10 + it->digitValue();
            }
            Token_list.append(new Constant_Token(val));
            ++it;
        }

        //情况3：识别到字母或者下划线
        else if(it->isLetter() || (*it) == '_')
        {
            QString input = nullptr;
            input.append(*it);
            while(it+1 != user_input.end() && ((it+1)->isLetterOrNumber() || *(it+1) == '_'))
            {
                ++it;
                input.append(*it);
            }
            //情况1：输入是关键字
            if(input == "REM") {Token_list.append(new Key_Word_Token(REM));++it;}

            else if(input == "LET"){Token_list.append(new Key_Word_Token(LET));++it;}

            else if(input == "PRINT"){Token_list.append(new Key_Word_Token(PRINT));++it;}

            else if(input == "INPUT"){Token_list.append(new Key_Word_Token(INPUT));++it;}

            else if(input == "GOTO"){Token_list.append(new Key_Word_Token(GOTO));++it;}

            else if(input == "IF"){Token_list.append(new Key_Word_Token(IF));++it;}

            else if(input == "THEN"){Token_list.append(new Key_Word_Token(THEN));++it;}

            else if(input == "END"){Token_list.append(new Key_Word_Token(END));++it;}

            //情况2：输入是操作符
            else if(input == "MOD"){Token_list.append(new Operator_Token(MOD));++it;}

            //情况3：输入是命令符
            else if(input == "RUN"){Token_list.append(new Command_Token(RUN));++it;}

            else if(input == "LOAD"){Token_list.append(new Command_Token((LOAD)));++it;}

            else if(input == "LIST"){Token_list.append(new Command_Token(LIST));++it;}

            else if(input == "CLEAR"){Token_list.append(new Command_Token(CLEAR));++it;}

            else if(input == "HELP"){Token_list.append(new Command_Token(HELP));++it;}

            else if(input == "QUIT"){Token_list.append(new Command_Token(QUIT));++it;}

            //情况4：输入变量
            else{Token_list.append(new Identifier_Token(input));++it;}
        }

        //情况4：识别到操作符
        else if(*it == '+'){Token_list.append(new Operator_Token(ADD));++it;}

        else if(*it == '-')
        {
            if(!Token_list.isEmpty() && ((*(Token_list.end()-1))->type == Constant || (*(Token_list.end()-1))->type == Identifier))
            {
                Token_list.append(new Operator_Token(DEL));
                ++it;
            }

            else if(!Token_list.isEmpty() && (*(Token_list.end()-1))->type == Operator)
            {
                Operator_Token* tmp = static_cast<Operator_Token*>(*(Token_list.end()-1));
                if(tmp->op == RPA)
                {
                    Token_list.append(new Operator_Token(DEL));
                    ++it;
                }
                else
                {
                    Token_list.append(new Operator_Token(NEG));
                    ++it;
                }
            }

            else
            {
                Token_list.append(new Operator_Token(NEG));
                ++it;
            }

        }

        else if(*it == '*')
        {
            if(it+1 != user_input.end() && *(++it) == '*')
            {
                Token_list.append(new Operator_Token(POW));
                ++it;
            }

            else
            {
                Token_list.append(new Operator_Token(MUL));
            }
        }

        else if(*it == '/'){Token_list.append(new Operator_Token(DIV));++it;}

        else if(*it == '('){Token_list.append(new Operator_Token(LPA));++it;}

        else if(*it == ')'){Token_list.append(new Operator_Token(RPA));++it;}

        else if(*it == '<')
        {
            if(it+1 != user_input.end() && *(++it) == '=')
            {
                Token_list.append(new Conditional_Operator_Token(LE));
                ++it;
            }

            else if(*it == '>')
            {
                Token_list.append(new Conditional_Operator_Token(NE));
                ++it;
            }

            else
            {
                Token_list.append(new Conditional_Operator_Token(LT));
            }

        }

        else if(*it == '>')
        {
            if(it+1 != user_input.end() && *(++it) == '*')
            {
                Token_list.append(new Conditional_Operator_Token(GE));
                ++it;
            }

            else
            {
                Token_list.append(new Conditional_Operator_Token(GT));
            }

        }

        else if(*it == '='){Token_list.append(new Conditional_Operator_Token(EQ));++it;}

        //情况5：输入不合法
        else
        {
            Token_list.clear();
            throw QString("invalid input.");
        }
    }
    return Token_list;
}

