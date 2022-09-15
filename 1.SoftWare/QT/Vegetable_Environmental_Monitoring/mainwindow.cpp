#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qdebug.h"

QString SoilTemp = "ROUND(AVG(SoilTemp),2) AS 'SoilTemp'";
QString SoilHum = "ROUND(AVG(SoilHum),2) AS 'SoilHum'";
QString AirTemp = "ROUND(AVG(AirTemp),2) AS 'AirTemp'";
QString AirHum = "ROUND(AVG(AirHum),2) AS 'AirHum'";
QString illu = "ROUND(AVG(illu),2) AS 'illu'";
QString worm = "ROUND(AVG(worm),2) AS 'worm'";
QString ph = "ROUND(AVG(ph),2) AS 'ph'";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->myCustomPlot->addGraph();
    //设置画笔
    ui->myCustomPlot->graph(0)->setPen(QPen(Qt::blue));
    ui->myCustomPlot->yAxis->setRange(6.0,8.0);
    
    ui->label->setStyleSheet("color:red;");
    ui->label_3->setStyleSheet("color:red;");
    ui->label_4->setStyleSheet("color:red;");
    ui->label_6->setStyleSheet("color:red;");
    ui->label_9->setStyleSheet("color:red;");
    ui->label_11->setStyleSheet("color:red;");
    ui->label_13->setStyleSheet("color:red;");
    ui->label_15->setStyleSheet("color:red;");

    QTimer *m_pTimer =new QTimer(this);
    connect(m_pTimer,SIGNAL(timeout()),this,SLOT(Update_data()));
    m_pTimer->start(5000);
    //设置坐标轴显示范围,否则我们只能看到默认的范围

    dbconn = QSqlDatabase::addDatabase("QMYSQL");
    dbconn.setHostName(hostName);
    dbconn.setDatabaseName(dbName);
    dbconn.setUserName(userName);
    dbconn.setPassword(password);
    dbconn.setPort(port);
    on_comboBox_currentIndexChanged(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::Update_data()
{
    qDebug() << "Timer";
    QString sql = "SELECT DATE_FORMAT(Data,'%Y-%m-%d %H:%i:%s') AS 'Data', ROUND(SoilTemp,2) AS 'SoilTemp',ROUND(SoilHum,2) AS 'SoilHum',ROUND(AirTemp,2) AS 'AirTemp',ROUND(AirHum,2) AS 'AirHum',ROUND(illu,2) AS 'illu',ROUND(worm,2) AS 'worm',ROUND(ph,2) AS 'ph' FROM `monitor` WHERE DATE_SUB(CURDATE(), INTERVAL 30 DAY) <= date(Data) order by Data DESC limit 1";
    if (dbconn.open()) {
        QSqlQuery query;//用于执行SQL语言
        query.exec(sql);
        while (query.next()) {
            ui->label_16->setText(query.value("Data").toString());
            ui->label_2->setText(query.value("SoilTemp").toString());
            ui->label_5->setText(query.value("SoilHum").toString());
            ui->label_7->setText(query.value("AirTemp").toString());
            ui->label_8->setText(query.value("AirHum").toString());
            ui->label_10->setText(query.value("illu").toString());
            ui->label_12->setText(query.value("worm").toString());
            ui->label_14->setText(query.value("ph").toString());
        }


        dbconn.close();
    } else {
        qDebug() << "Open failed.";
    }
}

void MainWindow::on_comboBox_currentIndexChanged(int index)
{
    QString sql;
    switch (index) {
        case 0:
            sql = SoilTemp;
            ui->myCustomPlot->xAxis->setLabel("时间");
            ui->myCustomPlot->yAxis->setLabel("土壤温度");
            break;
        case 1:
            sql = SoilHum;
            ui->myCustomPlot->xAxis->setLabel("时间");
            ui->myCustomPlot->yAxis->setLabel("土壤湿度");
            break;
        case 2:
            sql = AirTemp;
            ui->myCustomPlot->xAxis->setLabel("时间");
            ui->myCustomPlot->yAxis->setLabel("空气温度");
            break;
        case 3:
            sql = AirHum;
            ui->myCustomPlot->xAxis->setLabel("时间");
            ui->myCustomPlot->yAxis->setLabel("空气湿度");
            break;
        case 4:
            sql = illu;
            ui->myCustomPlot->xAxis->setLabel("时间");
            ui->myCustomPlot->yAxis->setLabel("光照强度(%)");
            break;
        case 5:
            sql = ph;
            ui->myCustomPlot->xAxis->setLabel("时间");
            ui->myCustomPlot->yAxis->setLabel("酸碱度");
            break;
        case 6:
            sql = worm;
            ui->myCustomPlot->xAxis->setLabel("x");
            ui->myCustomPlot->yAxis->setLabel("虫害");
            break;
    }

    if (dbconn.open()) {
        QString Pre_Sql = "SELECT DATE_FORMAT(Data,'%Y-%m-%d') AS 'Data',";
        QString Aft_Sql = "FROM `monitor` WHERE DATE_SUB(CURDATE(), INTERVAL 30 DAY) <= date(Data) GROUP BY DATE_FORMAT(Data,'%Y-%m-%d')";
        qDebug() << "Open successfully.";
        QSqlQuery query;//用于执行SQL语言
        query.exec(Pre_Sql + sql + Aft_Sql);
        QSqlRecord rec = query.record();
        QVector<double> value(query.size());
        QVector<double> time(query.size());
        double Max=0,Min=0;
        int i = 0;
        while (query.next()) {
            if(i == 0){
                Min = query.value(1).toFloat();
                Max = query.value(1).toFloat();
            }
            else
            {
                if(Min>query.value(1).toFloat()) Min = query.value(1).toFloat();
                if(Max<query.value(1).toFloat()) Max = query.value(1).toFloat();
            }
            time[i] = QDateTime::fromString(query.value(0).toString(),"yyyy-MM-dd").toTime_t();
            value[i] = query.value(1).toFloat();
            i++;
            if(i >= query.size()) break;
        }

        ui->myCustomPlot->graph(0)->setData(time,value);
        ui->myCustomPlot->graph(0)->rescaleAxes();

        QSharedPointer<QCPAxisTickerDateTime> dateTicker(new QCPAxisTickerDateTime);//日期做X轴
        dateTicker->setDateTimeFormat("yyyy-MM-dd");//日期格式

        dateTicker->setTickCount(query.size()/4);
        ui->myCustomPlot->xAxis->setTicker(dateTicker);

        ui->myCustomPlot->graph(0)->rescaleKeyAxis();
        ui->myCustomPlot->yAxis->setRange(Min-(Min*0.01),(Max+Max*0.01));//Y轴缩放
        ui->myCustomPlot->replot();
        dbconn.close();
    } else {
        qDebug() << "Open failed.";
    }

}
