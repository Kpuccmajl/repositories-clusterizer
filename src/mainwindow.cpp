#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QTableWidget>
#include <QLabel>
#include <strstream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    if (!Config::instance().isOk()) {
        qCritical() << Q_FUNC_INFO << "bad config. terminating..";
        exit(0);
    }

    ui->layPlot->insertWidget(0, &m_plot, 1);
    m_plot.setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    m_plot.xAxis->setLabel("Quantity");
    m_plot.yAxis->setLabel("Quality");
    m_plot.xAxis->setScaleRatio(m_plot.yAxis);
    m_plot.legend->setVisible(true);

    Config::instance().readConfig();

    colors << QColor(QColor::fromRgb(0,0,255,150))
           << QColor(QColor::fromRgb(255,0,0,150))
           << QColor(QColor::fromRgb(0,255,0,150))
           << QColor(QColor::fromRgb(0,125,125,150))
           << QColor(QColor::fromRgb(125,125,0,150))
           << QColor(QColor::fromRgb(125,0,125,150))
           << QColor(QColor::fromRgb(125,255,125,150))
           << QColor(QColor::fromRgb(255,0,255,150));

    Config::instance().repositories().clear();
    Config::instance().m_repositories.append(QList<Repository>()
                                             << Repository("project1",  0.3, 0.2, 0.1, 1.2)
                                             << Repository("project2",  0.1, 0.8, 0.0, 0.1)
                                             << Repository("project3",  0.5, 1.3, 0.2, 1.3)
                                             << Repository("project4",  0.6, 0.4, 0.4, 0.4)
                                             << Repository("project5",  1.9, 0.3, 1.5, 0.6)
                                             << Repository("project6",  1.4, 0.2, 0.8, 1.5)
                                             << Repository("project7",  0.3, 1.2, 0.9, 0.4)
                                             << Repository("project8",  1.2, 0.1, 1.9, 0.3)
                                             << Repository("project10", 0.3, 0.3, 2.2, 1.3)
                                             << Repository("project11", 0.7, 1.4, 2.1, 0.2)
                                             << Repository("project12", 0.2, 0.6, 1.3, 0.3)
                                             << Repository("project13", 0.2, 0.5, 0.5, 0.0)
                                             << Repository("project14", 1.3, 1.5, 0.4, 0.9)
                                             << Repository("project15", 0.4, 0.1, 0.2, 1.1)
                                             << Repository("project15", 0.1, 0.2, 1.3, 0.3)
                                             << Repository("project16", 0.8, 0.4, 0.5, 0.3)
                                             << Repository("project17", 1.8, 1.4, 0.6, 0.3)
                                             << Repository("project18", 0.8, 2.3, 0.7, 0.2)
                                             << Repository("project19", 0.2, 0.8, 1.8, 1.7)
                                             << Repository("project20", 0.3, 1.5, 0.0, 0.7)
                                             << Repository("project21", 1.6, 0.5, 0.1, 0.7)
                                             << Repository("project22", 0.7, 0.4, 1.2, 0.6)
                                             << Repository("project23", 0.0, 1.2, 2.4, 0.4)
                                             << Repository("project24", 0.1, 0.1, 0.5, 1.2)
                                             << Repository("project25", 1.2, 0.1, 0.6, 1.1)
                                             );
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::drawClustered()
{
    m_plot.clearGraphs();
    wasClicked = false;
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
        QPen pen = QPen(Qt::lightGray);
        pen.setWidth(8);
        m_plot.graph()->setPen(pen);
        m_plot.graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle));
        m_plot.graph()->setLineStyle(QCPGraph::LineStyle::lsNone);
        m_plot.graph()->setData(xv, yv);
        m_plot.graph()->removeFromLegend();
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
        cpen.setWidth(15);
        m_plot.graph()->setPen(cpen);
        m_plot.graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle));
        m_plot.graph()->setLineStyle(QCPGraph::LineStyle::lsNone);
        m_plot.graph()->setData(xv, yv);
        m_plot.graph()->setName("cluster " + QString::number(c.clusterId) + " centroid");

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

        if (ui->rbLines->isChecked()) {
            m_plot.graph()->setLineStyle(QCPGraph::LineStyle::lsLine);
        } else if (ui->rbDots->isChecked() || ui->rbCentroids->isChecked() || ui->rbCircles->isChecked()) {
            m_plot.graph()->setLineStyle(QCPGraph::LineStyle::lsNone);
        }

        m_plot.graph()->setData(xv, yv);
        m_plot.graph()->setName("cluster " + QString::number(c.clusterId) + " points");

        if (ui->rbCentroids->isChecked()) {
            for (auto & p: c.points) {
                m_plot.addGraph();
                m_plot.graph()->removeFromLegend();
                xv.clear();
                yv.clear();
                xv.append(p.values.at(0));
                xv.append(c.centroid.at(0));
                yv.append(p.values.at(1));
                yv.append(c.centroid.at(1));
                QPen pppen = QPen(colors.at(color));
                pppen.setStyle(Qt::PenStyle::DashLine);
                pppen.setWidth(2);
                m_plot.graph()->setPen(pppen);
                m_plot.graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle));
                m_plot.graph()->setData(xv, yv);
            }
        }

        if (ui->rbCircles->isChecked()) {
            m_plot.addGraph();
            m_plot.graph()->setName("cluster " + QString::number(c.clusterId) + " area");
            xv.clear();
            yv.clear();
            xv.append(c.centroid.at(0));
            yv.append(c.centroid.at(1));
            QPen pppen = QPen(colors.at(color));
            pppen.setStyle(Qt::PenStyle::SolidLine);
            pppen.setWidth(150);
            m_plot.graph()->setPen(pppen);
            m_plot.graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle));
            m_plot.graph()->setData(xv, yv);
        }

        ++color;
    }

    double scale = 1.0;
    m_plot.xAxis->setRange(xMin - scale, xMax + scale);
    m_plot.yAxis->setRange(yMin, yMax + scale);
    m_plot.replot();
}

void MainWindow::showText(QString & txt)
{
    QString text = txt;

    std::stringstream ss;
    int n = 1;
    ss << "\n> centroids:\n";
    for (Cluster & c : clusters) {
        ss << n++ << " (" << Analyzer::normalFloat(c.centroid.at(0)) << ", " << Analyzer::normalFloat(c.centroid.at(1)) << ")\n";
    }
    text += QString::fromStdString(ss.str());

    ui->text->setText(text);
}

void MainWindow::setColorForClusters()
{
    int clus = 0;
    for (Cluster & c: clusters) {
        for (Point & p: c.points) {
            auto item = ui->table->item(p.pointId - 1, 0);
            if (item) {
                item->setBackgroundColor(colors.at(clus));
                float distToCluster = std::sqrt(fabs(std::pow(c.centroid.at(0) - p.values.at(0), 2)
                                                     + std::pow(c.centroid.at(1) - p.values.at(1), 2)));
                float distSum = 0.0;
                for (Cluster & cc: clusters) {
                    distSum += std::sqrt(fabs(std::pow(cc.centroid.at(0) - p.values.at(0), 2)
                                              + std::pow(cc.centroid.at(1) - p.values.at(1), 2)));
                }
                float perc = Analyzer::normalFloat(100.0 - (distToCluster / distSum) * 100.0);
                item->setText(QString::number(perc) + "%");
            }
        }
        ++clus;
    }
}

void MainWindow::lingua()
{
    std::stringstream ss;
    ss << "> linguistic analyze " << endl;
    auto all = Config::instance().repositories().count();

    // biggest
    int biggest = 0;
    int dots = 0;
    for (auto & c : clusters) {
        if (dots < c.points.size()) {
            biggest = c.clusterId;
            dots = c.points.size();
        }
    }
    ss << "biggest cluster is  " << biggest << " (" << QString::number(float(dots) / float(all) * 100.0, 'f', 2).toStdString() << "% of all)" << endl;

    // smalles
    int smallest = 0;
    dots = INT32_MAX;
    for (auto & c : clusters) {
        if (dots > c.points.size()) {
            smallest = c.clusterId;
            dots = c.points.size();
        }
    }
    ss << "smallest cluster is " << smallest << " (" << QString::number(float(dots) / float(all) * 100.0, 'f', 2).toStdString() << "% of all)" << endl;

    for (auto & c : clusters) {
        double sumX = 0.0;
        double sumY = 0.0;
        int numberOfPoints = c.points.size();
        for (auto & p : c.points) {
            sumX += p.values.at(0);
            sumY += p.values.at(1);
        }
        double aveX = sumX / numberOfPoints;
        double avey = sumY / numberOfPoints;

        ss << endl << "projects in cluster " << c.clusterId << " are: ";
        if (aveX < 1.0) {
            ss << "very small";
        } else if (aveX < 2.0) {
            ss << "small";
        } else if (aveX < 3.0) {
            ss << "average";
        } else if (aveX < 4.0) {
            ss << "big";
        } else {
            ss << "very big";
        }

        ss << " size with ";

        if (avey < 0.5) {
            ss << "very bad";
        } else if (avey < 1.0) {
            ss << "bad";
        } else if (avey < 1.5) {
            ss << "average";
        } else if (avey < 2.0) {
            ss << "good";
        } else {
            ss << "very good";
        }

        ss << " quality";

    }

    ui->text->append(QString::fromStdString(ss.str()));
}

double MainWindow::clustersWeight(std::vector<Cluster> &clusters)
{
    double weight = 0.0;
    double sum = 0.0;
    // points to centroids
    for (auto & c: clusters) {
        weight = 0.0;
        for (auto & p: c.points) {
            weight += range(c.centroid.at(0), p.values.at(0), c.centroid.at(1), p.values.at(1));
        }
        sum += weight;
    }
    // between centroids
    double max = 0.0, min = INT32_MAX;
    for (int i = 1; i < clusters.size(); i++) {
        auto res = range(clusters.at(0).centroid.at(0), clusters.at(i).centroid.at(0),
                         clusters.at(0).centroid.at(1), clusters.at(i).centroid.at(1));
        if (max < res) max = res;
        if (min > res) min = res;
    }
    sum += (max + min) / clusters.size();
    return sum;
}

double MainWindow::range(float x1, float x2, float y1, float y2)
{
    return std::sqrt(fabs(std::pow(x2 - x1, 2) + std::pow(y2 - y1, 2)));
}

void MainWindow::on_btnAnalyze_clicked()
{
    ui->text->clear();
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
    ui->table->setColumnCount(8);
    ui->table->setHorizontalHeaderLabels(QStringList()
                                         << tr("Cluster")
                                         << tr("Repository")
                                         << tr("C%")
                                         << tr("Support")
                                         << tr("Size")
                                         << tr("Quality")
                                         << tr("mQuantity")
                                         << tr("mQuality")
                                         );
    ui->table->setColumnWidth(1, 100);
    int row = 0;
    for (auto & r: Config::instance().repositories()) {
        ui->table->setItem(row, 0, new QTableWidgetItem());
        ui->table->setItem(row, 1, new QTableWidgetItem(r.link().split('/').last()));
        ui->table->setItem(row, 2, new QTableWidgetItem(QString::number(r.language())));
        ui->table->setItem(row, 3, new QTableWidgetItem(QString::number(r.support())));
        ui->table->setItem(row, 4, new QTableWidgetItem(QString::number(r.size())));
        ui->table->setItem(row, 5, new QTableWidgetItem(QString::number(r.quality())));
        ui->table->setItem(row, 6, new QTableWidgetItem(QString::number(r.x())));
        ui->table->setItem(row, 7, new QTableWidgetItem(QString::number(r.y())));
        ++row;
    }
    ui->table->scrollToBottom();
    ui->btnAnalyze->setEnabled(true);
    ui->btnClusterize->setEnabled(true);
}

void MainWindow::on_btnClusterize_clicked()
{
    ui->btnClusterize->setEnabled(false);
    update();
    std::vector<Point> points;
    int id = 1;
    for (auto & r: Config::instance().repositories()) {
        std::string str = QString::number(r.x()).toStdString() + " " + QString::number(r.y()).toStdString();
        Point p(id++, str);
        points.push_back(p);
    }

    double bestWeight = INT32_MAX;
    QString info;
    srand(time(NULL));
    double ave = 0.0;
    KMeans k(ui->sbClustersCount->value(), 100);
    int iters = 1000;
    for (int i = 0; i < iters; i++) {
        k.clusterize(points);
        auto w = clustersWeight(k.clusters);
        ave += w;
        if (bestWeight > w) {
            bestWeight = w;
            info = k.info;
            clusters = k.clusters;
        }
    }
    ave = ave / iters;
    info += "> iterations = " + QString::number(iters) + "\n";
    info += "> best clusterisation MINMAX index = " + QString::number(bestWeight) + " (ave = " + QString::number(ave, 'f', 2) + ")";
    drawClustered();
    showText(info);
    setColorForClusters();
    lingua();
    ui->btnClusterize->setEnabled(true);
}

void MainWindow::on_table_itemClicked(QTableWidgetItem *item)
{
    if (item) {
        auto x = ui->table->item(item->row(), 6)->text().toFloat();
        auto y = ui->table->item(item->row(), 7)->text().toFloat();
        QVector<double> xv, yv;
        xv.append(x);
        yv.append(y);
        QPen pen = QPen(Qt::black);
        pen.setWidth(25);
        if (wasClicked)
            m_plot.removeGraph(m_plot.graph());
        m_plot.addGraph();
        m_plot.graph()->setPen(pen);
        m_plot.graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle));
        m_plot.graph()->setData(xv, yv);
        m_plot.graph()->removeFromLegend();
        m_plot.replot();
        wasClicked = true;
    }
}

void MainWindow::on_rbLines_clicked()
{
    drawClustered();
}

void MainWindow::on_rbDots_clicked()
{
    drawClustered();
}

void MainWindow::on_rbCentroids_clicked()
{
    drawClustered();
}

void MainWindow::on_rbCircles_clicked()
{
    drawClustered();
}

void MainWindow::on_btnReport_clicked()
{

}
