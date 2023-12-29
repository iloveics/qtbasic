#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "program.h"
#include <QEventLoop>
#include <QFileDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    void Clear_All();
    void Show_Help_List();
    void Show_Code();
    void Show_Tree();
    ~MainWindow();

    //辅助函数
    void Set_Code(long long line,QString code);
    void Delete_Code(long long line);
    void Load_File(const QString &file);
    void Proc_Run();
    void Proc_Restart();
    void Proc_Kill();

signals:
    void User_input(QString&);
    void Proc_stop();

private slots:
    void on_cmdLineEdit_returnPressed();

private:
    Ui::MainWindow *ui;

    bool Program_is_running;

    Program QBasic;
};
#endif // MAINWINDOW_H
