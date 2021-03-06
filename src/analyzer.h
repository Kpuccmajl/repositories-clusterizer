#ifndef ANALYZER_H
#define ANALYZER_H

#include <QObject>
#include "config.h"

class Analyzer : public QObject
{
    Q_OBJECT
public:
    static bool analyze(Repository &repository);
    static bool sloc(Repository & repository);
    static bool cppcheck(Repository & repository);
    static bool checkSupport(Repository & repository);
    static bool language(Repository & repository);

    static float normalFloat(float f);

private:
    explicit Analyzer(QObject *parent = 0);
    Q_DISABLE_COPY(Analyzer)
};

#endif // ANALYZER_H
