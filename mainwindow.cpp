#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setFixedSize(1000,1200);
    Program_is_running = false;

    connect(&QBasic,&Program::Print_Val,this,[&](long long val){
        ui->textBrowser->append(QString::number(val));
    });

    connect(&QBasic,&Program::Need_Input,this,[&](){
        ui->cmdLineEdit->setText("? ");
        ui->cmdLineEdit->setFocus();

        QEventLoop loop;
        connect(this,&MainWindow::Proc_stop,this,[&](){
            loop.quit();
        });
        connect(this,&MainWindow::User_input,this,[&](QString &input){
          bool ok;
          input = input.replace("? ","");
          long long val = input.toLongLong(&ok);

          if(!ok)
          {
              ui->textBrowser->append(QString("Invalid input.Please input a (long long) int number."));
              ui->cmdLineEdit->setText("? ");
              ui->cmdLineEdit->setFocus();
          }

          else
          {
            QBasic.Send_Caught_Input(val);
            loop.quit();
          }
        });

        loop.exec();
        disconnect(this,&MainWindow::User_input,0,0);
    });

    connect(ui->btnLoadCode,&QPushButton::clicked,this,[&](){
        QString file = QFileDialog::getOpenFileName();

        ui->textBrowser->append(QString("Load from : ")+file);

        Load_File(file);
    });

    connect(ui->btnRunCode,&QPushButton::clicked,this,[&](){
        if(!Program_is_running)
        {   
            Proc_Run();
        }

        else
        {
            ui->textBrowser->append("Program stop execution.");
            Proc_Kill();
            emit Proc_stop();
//            disconnect(this,&MainWindow::User_input,0,0);
        }
    });

    connect(ui->btnClearCode,&QPushButton::clicked,this,[&](){
        QBasic.Clear_Basic();
        Clear_All();
    });

    ui->cmdLineEdit->setFocus();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::Set_Code(long long line, QString code)
{
    if(line < 0)
    {
        ui->textBrowser->append("Error : line number must not be negative.");
        return;
    }

    try {
        QBasic.Set_Code(line,code);
    } catch (QString &error) {
        ui->textBrowser->append(error);
        return;
    }

    Show_Code();
    Show_Tree();
}

void MainWindow::Delete_Code(long long line)
{
    if(line < 0)
    {
        ui->textBrowser->append("Error : line number must not be negative.");
        return;
    }

    try {
        QBasic.Delete_Code(line);
    } catch (QString &error) {
        ui->textBrowser->append(error);
    }

    Show_Code();
    Show_Tree();
}

void MainWindow::Load_File(const QString &file)
{
    Proc_Kill();
    ui->textBrowser->clear();
    try {
        QBasic.Load_Code_From_File(file);
    } catch (QString &error) {
        ui->textBrowser->append(error);
    }

    Show_Code();
    Show_Tree();
}

void MainWindow::Proc_Run()
{
    Proc_Restart();
    ui->textBrowser->clear();
    ui->textBrowser->append("Program execution.");

    Program_is_running = true;
    QString runtime_wrong;

    try {
        QBasic.Exe_Program();
    } catch (QString &error) {
        runtime_wrong = error;
        QBasic.Kill_Program();
    }

    if(runtime_wrong.isEmpty())
    {
        ui->textBrowser->append("Process exit with 0 error.");
    }

    else
    {
        ui->textBrowser->append("Process exit with error: "+runtime_wrong);
    }

    Show_Tree();
    Proc_Kill();
}

void MainWindow::Proc_Restart()
{
    Program_is_running = false;
    QBasic.Restart();
    ui->cmdLineEdit->clear();
}

void MainWindow::Proc_Kill()
{
    Program_is_running = false;
    QBasic.Kill_Program();
    ui->cmdLineEdit->clear();
}

void MainWindow::on_cmdLineEdit_returnPressed()
{
    try {
        QString cmd = (ui->cmdLineEdit->text()).trimmed();
        ui->cmdLineEdit->setText("");
        ui->textBrowser->moveCursor(QTextCursor::End);
        ui->cmdLineEdit->clear();
        qDebug() << cmd;

        QString first_input = cmd.section(" ",0,0);
        QString code = cmd.section(" ",1).trimmed();

        bool is_Number = false;
        long long line = first_input.toLongLong(&is_Number);
        if(is_Number)
        {
            if(code.isEmpty())
                Delete_Code(line);

            else
            {
                Set_Code(line,code);
            }
        }

        else if(first_input == "RUN")
        {
            Proc_Run();
            return;
        }

        else if(first_input == "CLEAR")
        {
            QBasic.Clear_Basic();
            Clear_All();
        }

        else if(first_input == "LOAD")
        {
            Load_File(code);
        }

        else if(first_input == "HELP")
        {
            Show_Help_List();
        }

        else if(first_input == "QUIT")
        {
            exit(0);
        }

        else if(first_input == "?")
        {
            emit User_input(code);
        }

        else if(first_input == "PRINT")
        {
            long long val = QBasic.eval.Get_Variable_Val_From_Name(code);
            ui->textBrowser->append(QString::number(val));
        }

        else
        {
            ui->textBrowser->append(QString("invalid input of command."));
        }


    } catch (QString &error) {
        ui->textBrowser->append(error);
    }
}

void MainWindow::Clear_All()
{
    Proc_Kill();
    ui->CodeDisplay->clear();
    ui->textBrowser->clear();
    ui->treeDisplay->clear();
//    Show_Code();
}

void MainWindow::Show_Help_List()
{
    ui->textBrowser->append("Help List [Here's what you can input]:");
    ui->textBrowser->append("    RUN                    run your input code.");
    ui->textBrowser->append("    LOAD                   Load code from file.");
    ui->textBrowser->append("    CLEAR                  Clear the currunt QBasic.");
    ui->textBrowser->append("    HELP                   Show this help menu.");
    ui->textBrowser->append("    QUIT                   Quit QBasic.");
    ui->textBrowser->append("    line_number + code     insert a line of code.");
    ui->textBrowser->append("    line_number            delete a line of code.");
}

void MainWindow::Show_Code()
{
    ui->CodeDisplay->clear();

    for(QMap<unsigned long,QString>::iterator it = QBasic.code_print_seq.begin();it != QBasic.code_print_seq.end();++it)
    {
        ui->CodeDisplay->append(QString::number(it.key())+" "+it.value());
    }
}

void MainWindow::Show_Tree()
{
    ui->treeDisplay->clear();

    for(QMap<unsigned long,QStringList>::iterator it = QBasic.tree_print_seq.begin();it != QBasic.tree_print_seq.end();++it)
    {
        QStringList tree = it.value();
        tree[0].push_front(QString::number(it.key())+ " ");
        for(QString &line : tree)
            ui->treeDisplay->append(line);
    }
}
