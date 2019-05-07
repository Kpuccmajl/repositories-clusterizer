#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QTableWidget>
#include <QLabel>

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
    m_plot.xAxis->setScaleRatio(m_plot.yAxis);

    Config::instance().readConfig();

    colors << QColor(Qt::red)
           << QColor(Qt::blue)
           << QColor(Qt::green)
           << QColor(Qt::yellow)
           << QColor(Qt::magenta)
           << QColor(Qt::black)
           << QColor(Qt::gray)
           << QColor(Qt::cyan);

    Config::instance().repositories().clear();
    Config::instance().m_repositories.append(QList<Repository>()
                                             << Repository("p1",  0.3, 0.2, 0.1, 1.2)
                                             << Repository("p2",  0.1, 0.8, 0.0, 0.1)
                                             << Repository("p3",  0.5, 1.3, 0.2, 1.3)
                                             << Repository("p4",  0.6, 0.4, 0.4, 0.4)
                                             << Repository("p5",  1.9, 0.3, 1.5, 0.6)
                                             << Repository("p6",  1.4, 0.2, 0.8, 1.5)
                                             << Repository("p7",  0.3, 1.2, 0.9, 0.4)
                                             << Repository("p8",  1.2, 0.1, 1.9, 0.3)
                                             << Repository("p10", 0.3, 0.3, 2.2, 1.3)
                                             << Repository("p11", 0.7, 1.4, 2.1, 0.2)
                                             << Repository("p12", 0.2, 0.6, 1.3, 0.3)
                                             << Repository("p13", 0.2, 0.5, 0.5, 0.0)
                                             << Repository("p14", 1.3, 1.5, 0.4, 0.9)
                                             << Repository("p15", 0.4, 0.1, 0.2, 1.1)
                                             << Repository("p15", 0.1, 0.2, 1.3, 0.3)
                                             << Repository("p16", 0.8, 0.4, 0.5, 0.3)
                                             << Repository("p17", 1.8, 1.4, 0.6, 0.3)
                                             << Repository("p18", 0.8, 2.3, 0.7, 0.2)
                                             << Repository("p19", 0.2, 0.8, 1.8, 1.7)
                                             << Repository("p20", 0.3, 1.5, 0.0, 0.7)
                                             << Repository("p21", 1.6, 0.5, 0.1, 0.7)
                                             << Repository("p22", 0.7, 0.4, 1.2, 0.6)
                                             << Repository("p23", 0.0, 1.2, 2.4, 0.4)
                                             << Repository("p24", 0.1, 0.1, 0.5, 1.2)
                                             << Repository("p25", 1.2, 0.1, 0.6, 1.1)
                                             );
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::drawClustered(std::vector<Cluster> & clusters)
{
    m_plot.clearGraphs();
    int xMin = INT32_MAX;
    int xMax = INT32_MIN;
    int yMin = INT32_MAX;
    int yMax = INT32_MIN;
    int color = 0;

    for (auto & r: Config::instance().repositories()) {
        QVector<double> xv, yv;
        m_plot.addGraph();
        auto x = r.x();
        auto y = r.y();
        xv.append(x);
        yv.append(y);
        if (x < xMin) xMin = x;
        if (x > xMax) xMax = x;
        if (y < yMin) yMin = y;
        if (y > yMax) yMax = y;
        QPen pen = QPen(Qt::darkGray);
        pen.setWidth(8);
        m_plot.graph()->setPen(pen);
        m_plot.graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle));
        m_plot.graph()->setLineStyle(QCPGraph::LineStyle::lsNone);
        m_plot.graph()->setData(xv, yv);
    }

    for (Cluster & c : clusters) {
        // show centroids
        QVector<double> xv, yv;
        m_plot.addGraph();
        auto x = c.centroid.at(0);
        auto y = c.centroid.at(1);
        xv.append(x);
        yv.append(y);
        QPen cpen = QPen(colors.at(color));
        cpen.setWidth(30);
        m_plot.graph()->setPen(cpen);
        m_plot.graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle));
        m_plot.graph()->setLineStyle(QCPGraph::LineStyle::lsNone);
        m_plot.graph()->setData(xv, yv);

        // show points
        xv.clear();
        yv.clear();
        m_plot.addGraph();
        for (Point & p: c.points) {
            auto x = p.values.at(0);
            auto y = p.values.at(1);
            xv.append(x);
            yv.append(y);
        }
        QPen ppen = QPen(colors.at(color));
        ppen.setStyle(Qt::PenStyle::DashLine);
        ppen.setWidth(2);
        m_plot.graph()->setPen(ppen);
        m_plot.graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle));
        m_plot.graph()->setLineStyle(QCPGraph::LineStyle::lsLine);
        m_plot.graph()->setData(xv, yv);
        ++color;
    }

    double scale = 1.0;
    m_plot.xAxis->setRange(xMin - scale, xMax + scale);
    m_plot.yAxis->setRange(yMin, yMax + scale);
    m_plot.replot();
}

void MainWindow::showText(std::vector<Cluster> & clusters, QString & txt)
{
    QString text = txt;

    std::stringstream ss;
    int n = 1;
    ss << "> centroids:\n";
    for (Cluster & c : clusters) {
        ss << n++ << "\t" << Analyzer::normalFloat(c.centroid.at(0)) << "\t" << Analyzer::normalFloat(c.centroid.at(1)) << "\n";
    }
    ss << "\n\n";
    text += QString::fromStdString(ss.str());

    ui->text->setText(text);
}

void MainWindow::clearLayout(QLayout* layout)
{
    while (QLayoutItem* item = layout->takeAt(0)) {
        if (QWidget* widget = item->widget()) {
            delete widget;
        }
        if (QLayout* childLayout = item->layout()) {
            clearLayout(childLayout);
        }
        delete item;
    }
}

void MainWindow::on_btnAnalyze_clicked()
{
    ui->text->clear();

    clearLayout(ui->layProcess);
    ui->btnAnalyze->setEnabled(false);
    ui->btnClusterize->setEnabled(false);

    //    int counter = 0;
    //    ui->progressBar->setMaximum(Config::instance().repositories().count());
    //    ui->progressBar->setValue(0);
    //    for (auto & r: Config::instance().repositories()) {
    //        ui->progressBar->setFormat("Analyzing: " + r.link());
    //        update();
    //        Analyzer::analyze(r);
    //        ui->progressBar->setValue(++counter);
    //    }
    //    ui->progressBar->setFormat("");

    //add to table
    ui->table->clear();
    ui->table->clearContents();
    ui->table->setRowCount(Config::instance().repositories().count());
    ui->table->setColumnCount(7);
    ui->table->setHorizontalHeaderLabels(QStringList()
                                         << tr("Репозиторий")
                                         << tr("CPP")
                                         << tr("Supp")
                                         << tr("Size")
                                         << tr("Qual")
                                         << tr("Quantity")
                                         << tr("Quality")
                                         );
    ui->table->setColumnWidth(0, 150);
    int row = 0;
    for (auto & r: Config::instance().repositories()) {
        ui->table->setItem(row, 0, new QTableWidgetItem(r.link().split('/').last()));
        ui->table->setItem(row, 1, new QTableWidgetItem(QString::number(r.language())));
        ui->table->setItem(row, 2, new QTableWidgetItem(QString::number(r.support())));
        ui->table->setItem(row, 3, new QTableWidgetItem(QString::number(r.size())));
        ui->table->setItem(row, 4, new QTableWidgetItem(QString::number(r.quality())));
        ui->table->setItem(row, 5, new QTableWidgetItem(QString::number(r.x())));
        ui->table->setItem(row, 6, new QTableWidgetItem(QString::number(r.y())));
        ++row;
    }
    ui->table->scrollToBottom();

    ui->btnAnalyze->setEnabled(true);
    ui->btnClusterize->setEnabled(true);

    on_btnClusterize_clicked();
}

void MainWindow::on_btnClusterize_clicked()
{
    std::vector<Point> points;
    int id = 1;
    for (auto & r: Config::instance().repositories()) {
        std::string str = QString::number(r.x()).toStdString() + " " + QString::number(r.y()).toStdString();
        Point p(id++, str);
        points.push_back(p);
    }

    KMeans k(ui->sbClustersCount->value(), 100);
    k.clusterize(points);
    drawClustered(k.clusters);
    showText(k.clusters, k.info);
}
