#ifndef PROGRAM_H
#define PROGRAM_H

#include "evalstate.h"
#include "statement.h"
#include "parser.h"
#include "token.h"
#include "tokenizer.h"
#include "expression.h"
#include <QMap>
#include <QFile>
#include <QTextStream>
#include <QEventLoop>
#include <qobject.h>

class Program : public QObject
{
    Q_OBJECT

public:
    Program();

    QString Get_Code(QList<Token*> tokenlist);
    void Set_Code(unsigned long line,QString &code);
    void Delete_Code(unsigned long line);

    //和mainwindow不同，以下函数相当于内核处理
    void Load_Code_From_File(const QString &filename);
    void Clear_Basic();
    void Exe_Program();
    void Update_Tree();
    void Restart();
    void Kill_Program();

    void Send_Print_Signal(long long v){emit Print_Val(v);}
    void Send_Need_Input(){emit Need_Input();}
    void Send_Caught_Input(long long v){emit Caught_Input(v);}

    //下面是存储成员
    QMap<unsigned long,Statement*> code_line; //用于按行号存储statement
    Evalstate eval; //用于计算表达式的值并且存储
    int cur_line; //记录当前执行到的位置

    //以下三个结构用于打印
    QMap<unsigned long,QString> code_print_seq;
    QMap<unsigned long,QStringList> tree_print_seq;
    QMap<unsigned long,QString> err_seq;

signals:
    void Print_Val(long long);
    void Need_Input();
    void Caught_Input(long long);

private:

};

#endif // PROGRAM_H
