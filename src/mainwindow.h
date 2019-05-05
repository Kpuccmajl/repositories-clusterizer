#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "analyzer.h"
#include "clusterizer.h"
#include "qcustomplot.h"

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
    void drawPlot();

    QList<Repositories> clusters;
    QList<QColor>       colors;

private slots:
    void on_btnAnalyze_clicked();

private:
    Ui::MainWindow * ui;
    QCustomPlot      m_plot;
};

#endif
