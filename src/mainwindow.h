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

private:
    Ui::MainWindow * ui;
    QCustomPlot      m_plot;
};

#endif
