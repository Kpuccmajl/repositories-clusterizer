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
    void drawClustered(std::vector<Cluster> & clusters);
    void showText(std::vector<Cluster> & clusters, QString & txt);

    QList<Repositories> clusters;
    QList<QColor>       colors;

private slots:
    void on_btnAnalyze_clicked();

    void on_btnClusterize_clicked();

private:
    void clearLayout(QLayout *layout);

    Ui::MainWindow * ui;
    QCustomPlot      m_plot;
};

#endif
