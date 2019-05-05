#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    if (!Config::instance().isOk()) {
        qCritical() << Q_FUNC_INFO << "bad config. terminating..";
        exit(0);
    }

    ui->layPlot->addWidget(&m_plot, 1);
    m_plot.setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    m_plot.xAxis->setLabel("Quantity");
    m_plot.yAxis->setLabel("Quality");

    colors << QColor(Qt::red)
           << QColor(Qt::blue)
           << QColor(Qt::green)
           << QColor(Qt::yellow)
           << QColor(Qt::magenta)
           << QColor(Qt::black)
           << QColor(Qt::gray)
           << QColor(Qt::cyan)
           << QColor(Qt::darkYellow)
           << QColor(Qt::darkCyan)
           << QColor(Qt::darkBlue)
           << QColor(Qt::darkGreen)
           << QColor(Qt::darkMagenta);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::drawPlot()
{
    m_plot.clearGraphs();
    int color = 0;
    int xMin = INT32_MAX;
    int xMax = INT32_MIN;
    int yMin = INT32_MAX;
    int yMax = INT32_MIN;
    for (auto & c: clusters) {
        QVector<double> xv, yv;
        m_plot.addGraph();
        for (auto & r: c) {
            auto x = r.language() + r.size();
            auto y = r.support() + r.quality();
            xv.append(x);
            yv.append(y);
            if (x < xMin) xMin = x;
            if (x > xMax) xMax = x;
            if (y < yMin) yMin = y;
            if (y > yMax) yMax = y;
        }
        QPen pen = QPen(colors.at(color));
        pen.setWidth(10);
        m_plot.graph()->setPen(pen);
        ++color;
        m_plot.graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDot));
        m_plot.graph()->setLineStyle(QCPGraph::LineStyle::lsNone);
        m_plot.graph()->setData(xv, yv);
    }
    m_plot.xAxis->setRange(xMin - 1, xMax + 1);
    m_plot.yAxis->setRange(yMin - 1, yMax + 1);
    m_plot.replot();
}

void MainWindow::on_btnAnalyze_clicked()
{
    ui->text->clear();
    Config::instance().repositories().clear();

    Config::instance().readConfig();
    int repositoryCount = Config::instance().repositories().count();
    int counter = 0;
    ui->progressBar->setMaximum(repositoryCount);
    ui->progressBar->setValue(0);
    for (auto & r: Config::instance().repositories()) {
        Analyzer::analyze(r);
        ui->progressBar->setFormat("Analyzing: " + r.link());
        ui->progressBar->setValue(++counter);
    }
    ui->progressBar->setFormat("");

    //add to table
    ui->table->clear();
    ui->table->clearContents();
    ui->table->setRowCount(Config::instance().repositories().count());
    ui->table->setColumnCount(5);
    ui->table->setHorizontalHeaderLabels(QStringList()
                                         << tr("Репозиторий")
                                         << tr("ЯП")
                                         << tr("Поддержка")
                                         << tr("Размер")
                                         << tr("Качество"));
    ui->table->setColumnWidth(0, 200);
    int row = 0;
    for (auto & r: Config::instance().repositories()) {
        ui->table->setItem(row, 0, new QTableWidgetItem(r.link().split('/').last()));
        ui->table->setItem(row, 1, new QTableWidgetItem(QString::number(r.language())));
        ui->table->setItem(row, 2, new QTableWidgetItem(QString::number(r.support())));
        ui->table->setItem(row, 3, new QTableWidgetItem(QString::number(r.size())));
        ui->table->setItem(row, 4, new QTableWidgetItem(QString::number(r.quality())));
        ++row;
    }
    ui->table->scrollToBottom();

    // do magic
    QString text;
    int reposCount = Config::instance().repositories().count();
    clusters = Clusterizer::clusterize(Config::instance().repositories(), text);
    text += Clusterizer::lingua(clusters, reposCount);
    ui->text->setText(text);

    drawPlot();
}
