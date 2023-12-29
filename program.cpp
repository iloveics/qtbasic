#include "program.h"
#include <QDebug>

Program::Program()
{
    code_line.clear();
    cur_line = 0;
    code_print_seq.clear();
    tree_print_seq.clear();
    err_seq.clear();
}

QString Program::Get_Code(QList<Token *> tokenlist)
{
    if(tokenlist.isEmpty()) return "";

    QString ret;
    for(QList<Token*>::iterator it = tokenlist.begin();it != tokenlist.end();++it)
    {
        try {
            ret += (*it)->to_String();
        } catch (QString &error) {
            throw error;
        }

        ret += " ";
    }

    return ret;
}

void Program::Set_Code(unsigned long line, QString &code)
{
    QString set_code;
    QStringList set_tree;
    Statement* statement;
    QList<Token*> tokenlist;
    QString error;

    //生成tokenlist和statement
    try {
        tokenlist = Scan_Token(code);
        statement = generate_statement(tokenlist);
    } catch (QString &err) {
        error = err;
        statement = nullptr;
    }

    //有error优先处理
    if(!error.isEmpty())
    {
        err_seq.insert(line,error); //如果该行初始没有error，会加入；如果已经有，更新为新error；如果没有该行，创建一个
        set_tree = QStringList{error};
        throw error;
    }

    else
    {
        err_seq.insert(line,"");
        set_tree = statement->to_String();
        try {
            set_code = Get_Code(tokenlist);
        } catch (QString &generate_code_error) {
            throw QString("Error when generate code.");
        }
    }

    code_print_seq.insert(line,set_code);
    tree_print_seq.insert(line,set_tree);
    code_line.insert(line,statement);
}

void Program::Delete_Code(unsigned long line)
{
    if(!code_line.contains(line)) return;

    code_line.remove(line);
    code_print_seq.remove(line);
    tree_print_seq.remove(line);
    err_seq.remove(line);
}

void Program::Load_Code_From_File(const QString &filename)
{
    QFile file(filename);

    if(file.isOpen())
        throw QString("File has been opened.");

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        throw QString("Error when open a file.");

    if(file.isOpen())
    {
        if(file.size() !=0)
        {   //Basic置初
            Clear_Basic();

            unsigned long read_line;
            QString read_code;
            QTextStream read_in(&file);
            bool Judge_Number;

            while(!read_in.atEnd())
            {
                read_code = (read_in.readLine()).trimmed();
                if(read_code.isEmpty()) continue;

                QStringList str = read_code.split(" ");
                read_line = str[0].toULong(&Judge_Number);
                if(!Judge_Number) continue;
                QString code = read_code.section(" ",1);

                try {
                    Set_Code(read_line,code);
                } catch (QString &error) {
                    throw error;
                }
            }

            file.close();
        }
    }
}

void Program::Clear_Basic()
{
    code_line.clear();
    cur_line = 0;
    code_print_seq.clear();
    tree_print_seq.clear();
    err_seq.clear();
    eval.Clear();
}

void Program::Exe_Program()
{
    eval.Clear();
    for(QMap<unsigned long,Statement*>::iterator it = code_line.begin();it != code_line.end();++it)
    {
        //每次运行前清空语句执行次数
        it.value()->Count_Clear();
        if(it.value()->type == IF_STMT)
        {
            If_Statement* tmp = static_cast<If_Statement*>(it.value());
            tmp->if_count = 0;
            tmp->then_count = 0;
        }
    }

    for(cur_line = 0;cur_line <code_line.size();++cur_line)
    {
        QMap<unsigned long,Statement*>::iterator it = code_line.begin();
        it += cur_line;
        Statement* currunt_state = it.value();

        switch(currunt_state->type)
        {
        case REM_STMT:
        {
            currunt_state->Add_Count();
            tree_print_seq.insert(it.key(),currunt_state->to_String());
            break;
        }
        case LET_STMT:
        {
            Let_Statement* tmp = static_cast<Let_Statement*>(currunt_state);
            currunt_state->Add_Count();
            for(QMap<QString,int>::iterator time = eval.var_used_time.begin();time != eval.var_used_time.end();++time)
            {
                if(time.key() == tmp->var)
                {
                    time.value()++;
                    tmp->var_used_time = time.value();
                    tree_print_seq.insert(it.key(),currunt_state->to_String());
                    break;
                }
            }

            long long val;
            try {
                val = eval.Calculate_Expression(tmp->exp);
            } catch (QString &error) {
                throw QString::number(it.key()) +" "+error;
            }

            eval.Set_Var(tmp->var,val);
            tree_print_seq.insert(it.key(),currunt_state->to_String());
            break;
        }
        case PRINT_STMT:
        {
            Print_Statement* tmp = static_cast<Print_Statement*>(currunt_state);
            tmp->Add_Count();

            long long val;
            try {
                val = eval.Calculate_Expression(tmp->exp);
            } catch (QString &error) {
                throw QString::number(it.key())+" "+error;
            }

            tree_print_seq.insert(it.key(),currunt_state->to_String());
            Send_Print_Signal(val);
            break;
        }
        case INPUT_STMT:
        {
            Input_Statement* tmp = static_cast<Input_Statement*>(currunt_state);
            tmp->Add_Count();
            for(QMap<QString,int>::iterator time = eval.var_used_time.begin();time != eval.var_used_time.end();++time)
            {
                if(time.key() == tmp->var)
                {
                    time.value()++;
                    tmp->var_used_time = time.value();
                    tree_print_seq.insert(it.key(),currunt_state->to_String());
                    break;
                }
            }

            connect(this,&Program::Caught_Input,this,[&](long long v)
            {
                qDebug() << "catch" ;
                eval.Set_Var(tmp->var,v);
            });

            Send_Need_Input();
            disconnect(this,&Program::Caught_Input,0,0);
            tree_print_seq.insert(it.key(),currunt_state->to_String());
            break;
        }
        case GOTO_STMT:
        {
            Goto_Statement* tmp = static_cast<Goto_Statement*>(currunt_state);
            tmp->Add_Count();
            tree_print_seq.insert(it.key(),currunt_state->to_String());

            unsigned long line = tmp->line_number;
            if(!code_line.contains(line)) throw QString("Error : Goto a line that does not exist.");

            int i = 0;
            for(QMap<unsigned long,Statement*>::iterator pos = code_line.begin();pos != code_line.end();++pos)
            {
                if(pos.key() == line) break;

                ++i;
            }

            cur_line = i -1;
            break;
        }
        case IF_STMT:
        {
            If_Statement* tmp = static_cast<If_Statement*>(currunt_state);
            tmp->if_count ++;

            long long val1;
            try {
                val1 = eval.Calculate_Expression(tmp->exp1);
            } catch (QString &error) {
                throw error;
            }

            long long val2;
            try {
                val2 = eval.Calculate_Expression(tmp->exp2);
            } catch (QString &error) {
                throw error;
            }

            tree_print_seq.insert(it.key(),currunt_state->to_String());

            bool Judge_if = false;
            switch (tmp->op) {
            case EQ:
            {
                if(val1 == val2) Judge_if = true;
                break;
            }
            case NE:
            {
                if(val1 != val2) Judge_if = true;
                break;
            }
            case LT:
            {
                if(val1 < val2) Judge_if = true;
                break;
            }
            case GT:
            {
                if(val1 > val2) Judge_if = true;
                break;
            }
            case LE:
            {
                if(val1 <= val2) Judge_if = true;
                break;
            }
            case GE:
            {
                if(val1 >= val2) Judge_if = true;
                break;
            }
            }

            if(!Judge_if) break;

            tmp->then_count ++;
            tree_print_seq.insert(it.key(),currunt_state->to_String());
            unsigned long line = tmp->line_number;
            if(!code_line.contains(line)) throw QString("Error : Goto a line that does not exist.");

            int i = 0;
            for(QMap<unsigned long,Statement*>::iterator pos = code_line.begin();pos != code_line.end();++pos)
            {
                if(pos.key() == line) break;

                ++i;
            }

            cur_line = i -1;
            break;
        }
        case END_STMT:
        {
            End_Statement* tmp = static_cast<End_Statement*>(currunt_state);
            tmp->Add_Count();
            tree_print_seq.insert(it.key(),currunt_state->to_String());

            cur_line = code_line.size();
            break;
        }
        }
    }
    Update_Tree();

    cur_line = 0;
//    eval.Clear();
}

void Program::Update_Tree()
{
    for(int i = 0;i <code_line.size();++i)
    {
        QMap<unsigned long,Statement*>::iterator it = code_line.begin();
        it += i;
        Statement* currunt_state = it.value();

        switch(currunt_state->type)
        {
        case REM_STMT:
        {
            tree_print_seq.insert(it.key(),currunt_state->to_String());
            break;
        }
        case LET_STMT:
        {
            Let_Statement* tmp = static_cast<Let_Statement*>(currunt_state);
            for(QMap<QString,int>::iterator time = eval.var_used_time.begin();time != eval.var_used_time.end();++time)
            {
                if(time.key() == tmp->var)
                {
                    tmp->var_used_time = time.value();
                    break;
                }
            }

            eval.Update_used_time(tmp->exp);
            tree_print_seq.insert(it.key(),currunt_state->to_String());
            break;
        }
        case PRINT_STMT:
        {
            Print_Statement* tmp = static_cast<Print_Statement*>(currunt_state);
            eval.Update_used_time(tmp->exp);
            tree_print_seq.insert(it.key(),currunt_state->to_String());
            break;
        }
        case INPUT_STMT:
        {
            Input_Statement* tmp = static_cast<Input_Statement*>(currunt_state);
            for(QMap<QString,int>::iterator time = eval.var_used_time.begin();time != eval.var_used_time.end();++time)
            {
                if(time.key() == tmp->var)
                {
                    tmp->var_used_time = time.value();
                    break;
                }
            }

            tree_print_seq.insert(it.key(),currunt_state->to_String());
            break;
        }
        case GOTO_STMT:
        {
            tree_print_seq.insert(it.key(),currunt_state->to_String());
            break;
        }
        case IF_STMT:
        {
            If_Statement* tmp = static_cast<If_Statement*>(currunt_state);
            eval.Update_used_time(tmp->exp1);
            eval.Update_used_time(tmp->exp2);
            tree_print_seq.insert(it.key(),currunt_state->to_String());
            break;
        }
        case END_STMT:
        {
            tree_print_seq.insert(it.key(),currunt_state->to_String());
            break;
        }
        }
    }
}

void Program::Restart()
{
    cur_line = code_line.size();
    eval.Clear();
}

void Program::Kill_Program()
{
    cur_line = code_line.size();
}
