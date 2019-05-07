#include "analyzer.h"
#include <QDebug>
#include <math.h>
#include <QProcess>
#include <QDateTime>

#define SLOC_SCRIPT         "../src/scripts/sloc.sh"
#define SLOC_LANG_SCRIPT    "../src/scripts/sloc-lang.sh"
#define CHECKSUPPORT_SCRIPT "../src/scripts/checksupport.sh"
#define CPPCHECK_SCRIPT     "../src/scripts/cppcheck.sh"

bool Analyzer::analyze(Repository &repository)
{
    bool isOk = true;
    if (!sloc(repository)) {
        qCritical() << Q_FUNC_INFO << "can't sloc" << repository.link();
        isOk = false;
    }
    if (!cppcheck(repository)) {
        qCritical() << Q_FUNC_INFO << "can't cppcheck" << repository.link();
        isOk = false;
    }
    if (!checkSupport(repository)) {
        qCritical() << Q_FUNC_INFO << "can't check support" << repository.link();
        isOk = false;
    }
    if (!language(repository)) {
        qCritical() << Q_FUNC_INFO << "can't check language" << repository.link();
        isOk = false;
    }
    return isOk;
}

bool Analyzer::sloc(Repository &repository)
{
    QProcess proc;
    proc.start("/bin/bash", QStringList() << SLOC_SCRIPT << repository.link());
    if (!proc.waitForStarted())
        return false;
    if (!proc.waitForFinished(1000 * 300))
        return false;

    QByteArray sloc = proc.readAll().split('=').last().trimmed().replace(',', "");
    if (sloc == "") {
        return false;
    }
    repository.setSloc(sloc.toUInt());

    int lowgrade = 5000;
    int highgrade = 50000;
    if (repository.sloc() < lowgrade) {
        repository.setSize(0.0);
    } else if (repository.sloc() > highgrade) {
        repository.setSize(1.0);
    } else {
        repository.setSize(normalFloat(static_cast<float>(repository.sloc()) / highgrade));
    }

    return true;
}

bool Analyzer::cppcheck(Repository &repository)
{
    QProcess proc;
    proc.start("/bin/bash", QStringList() << CPPCHECK_SCRIPT << repository.link());
    if (!proc.waitForStarted(1000 * 30))
        return false;
    if (!proc.waitForFinished(1000 * 300))
        return false;

    auto errcount = proc.readAll().split('\n').count();

    int lowgrade = 0;
    int highgrade = 200;
    if (errcount <= lowgrade) {
        repository.setQuality(0.0);
    } else if (errcount > highgrade) {
        repository.setQuality(1.0);
    } else {
        repository.setQuality(normalFloat(static_cast<float>(errcount) / highgrade));
    }

    return true;
}

bool Analyzer::checkSupport(Repository &repository)
{
    QProcess proc;
    proc.start("/bin/bash", QStringList() << CHECKSUPPORT_SCRIPT << repository.link());
    if (!proc.waitForStarted())
        return false;
    if (!proc.waitForFinished(1000 * 300))
        return false;

    auto res = proc.readAll().trimmed();
    auto lastUpdate = res.toUInt();
    if (lastUpdate == 0) {
        return false;
    }
    auto current = QDateTime::currentDateTime().toTime_t();

    float lowgrade = 86400 * 1;
    float highgrade = 86400 * 90;
    if (current - lastUpdate < lowgrade) {
        repository.setSupport(0.0);
    } else if (current - lastUpdate > highgrade) {
        repository.setSupport(1.0);
    } else {
        repository.setSupport(normalFloat(float(current - lastUpdate) / highgrade));
    }

    return true;
}

bool Analyzer::language(Repository &repository)
{
    QProcess proc;
    proc.start("/bin/bash", QStringList() << SLOC_LANG_SCRIPT << repository.link());
    if (!proc.waitForStarted())
        return false;
    if (!proc.waitForFinished(1000 * 300))
        return false;

    auto res = proc.readAll().trimmed().split('\n');

    for (auto & r: res) {
        if (r.split(':').first() == "ansic") {
           repository.setLanguage((int)r.split(' ').last().replace('%', "").replace('(', "").replace(')', "").toDouble() / 100.0);
           break;
        }
    }

    return true;
}

float Analyzer::normalFloat(float f)
{
    return static_cast<float>(rint(f * 100.0)) / 100.0;
}
