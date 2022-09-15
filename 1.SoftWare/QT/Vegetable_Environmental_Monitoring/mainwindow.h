#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qcustomplot.h"

#include <QSql>
#include <QSqlQueryModel>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QDateTime>
#include <QSharedPointer>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QSqlDatabase dbconn;
    int port = 3306;                        // 端口
    QString hostName = "175.178.128.27";    // 主机名
    QString dbName = "monitor";             // 数据库名称
    QString userName = "monitor";           // 用户名
    QString password = "monitor";           // 密码


private slots:
    void on_comboBox_currentIndexChanged(int index);
    void Update_data(void);

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
