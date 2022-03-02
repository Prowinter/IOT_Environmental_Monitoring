#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->myCustomPlot->addGraph();
    //设置画笔
    ui->myCustomPlot->graph(0)->setPen(QPen(Qt::blue));
    //设置右上角图形标注名称
    ui->myCustomPlot->graph(0)->setName("曲线");
    //设置坐标轴标签名称
    ui->myCustomPlot->xAxis->setLabel("x");
    ui->myCustomPlot->yAxis->setLabel("y");

    //设置坐标轴显示范围,否则我们只能看到默认的范围
    ui->myCustomPlot->xAxis->setRange(0,60);
    ui->myCustomPlot->yAxis->setRange(0,5000);
    ui->myCustomPlot->graph(0)->rescaleAxes(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}
