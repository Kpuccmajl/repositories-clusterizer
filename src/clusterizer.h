#ifndef CLUSTERIZER_H
#define CLUSTERIZER_H

#include <QObject>
#include "config.h"

using Floats = QList<float>;

struct NamedList
{
    QString name;
    Floats   list;
};

class Clusterizer : public QObject
{
    Q_OBJECT
public:
    static QList<Repositories> clusterizeMaxSimilarity(Repositories & repositories, QString &text);
    static QString lingua(QList<Repositories> & clusters, int reposCount);

private:
    explicit Clusterizer(QObject *parent = 0);
    Q_DISABLE_COPY(Clusterizer)

};

#endif // CLUSTERIZER_H
