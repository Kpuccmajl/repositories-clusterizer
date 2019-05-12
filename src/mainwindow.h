#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "analyzer.h"
#include "clusterizer.h"
#include "qcustomplot.h"
#include "kmeans.cpp"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    void drawPoints();
    void drawClustered();
    void showText(QString & txt);
    void setColorForClusters();
    void lingua();
    double clustersWeight(std::vector<Cluster> & clusters);
    double range(float x1, float x2, float y1, float y2);

    QList<QColor>        colors;
    bool                 wasClicked = false;
    std::vector<Cluster> clusters;

private slots:
    void on_btnAnalyze_clicked();
    void on_btnClusterize_clicked();
    void on_table_itemClicked(QTableWidgetItem *item);

    void on_rbLines_clicked();
    void on_rbDots_clicked();
    void on_rbCentroids_clicked();
    void on_rbCircles_clicked();

    void on_btnReport_clicked();

private:
    Ui::MainWindow * ui;
    QCustomPlot      m_plot;
};

#endif
