#include "mainwindow.h"

#include <QApplication>
#include "evalstate.h"
#include "tokenizer.h"
#include "parser.h"
#include <QDebug>
#include "token.h"
#include "statement.h"
#include "program.h"
#include "QFileDialog"
#include "QFile"

void Print_EXP(Expression* e)
{
    if(e->type == Identifier_EXP)
    {
        Identifier_Expression* tmp = static_cast<Identifier_Expression*>(e);
        qDebug()<<tmp->identifier_name;
        return;
    }

    if(e->type == Constant_EXP)
    {
        Constant_Expression* tmp = static_cast<Constant_Expression*>(e);
        qDebug()<<tmp->val;
        return;
    }

    if(e->type == Unary_EXP)
    {
        Unary_Expression* tmp = static_cast<Unary_Expression*>(e);
        qDebug()<<tmp->op;
        Print_EXP(tmp->child_exp);
        return;
    }

    Compound_Expression* tmp = static_cast<Compound_Expression*>(e);
    qDebug()<<tmp->op;
    Print_EXP(tmp->lexp);
    Print_EXP(tmp->rexp);

    return;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();


//    QString s = "100 REM Program to print the Fibonacci sequence.";
//    bool is_Number = false;
//    QString first_input = s.section(" ",0,0);
//    long long line = first_input.toLongLong(&is_Number);
//    qDebug() << is_Number;
//    qDebug() << s.section(" ",1);
//    QString ss = s.section(" ",1);
//    Program p;
//    p.Set_Code(line,ss);
//    qDebug() << p.code_line[line]->to_String();
//    qDebug() << p.tree_print_seq[line];


//    QString s = "100 IF A > MAX THEN 140";
//        bool is_Number = false;
//        QString first_input = s.section(" ",0,0);
//        long long line = first_input.toLongLong(&is_Number);
//        qDebug() << is_Number;
//        qDebug() << s.section(" ",1);
//        QString ss = s.section(" ",1);
//        QList<Token*> tokenlist = Scan_Token(ss);
//        Statement* state;
//        try {
//            state = generate_statement(tokenlist);
//        } catch (QString &error) {
//            qDebug() << error;
//        }
//        qDebug() << state->to_String();

//    QString s = "100 GOTO 120";
//    QString s1 = "120 END";
//            bool is_Number = false;
//            QString first_input = s.section(" ",0,0);
//            long long line = first_input.toLongLong(&is_Number);
//            qDebug() << is_Number;
//            qDebug() << s.section(" ",1);
//            QString ss = s.section(" ",1);
//            QString input2 = s1.section(" ",0,0);
//            QString ss2 = s1.section(" ",1);
//            long long line2 = input2.toLongLong();
//            Program p;
//            p.Set_Code(line,ss);
//            p.Set_Code(line2,ss2);
//            p.Exe_Program();

//    QString file = QFileDialog::getOpenFileName();

//    Program p;

//    p.Load_Code_From_File(file);

//    p.Exe_Program();
    return a.exec();
}
