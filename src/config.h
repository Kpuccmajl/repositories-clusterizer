#ifndef CONFIG_H
#define CONFIG_H

#include <QObject>
#include <QString>
#include <QList>
#include "repository.h"

class Config : public QObject
{
    Q_OBJECT
public:
    static Config & instance() { static Config c; return c; }

    Repositories & repositories() { return m_repositories; }
    bool isOk() const { return m_isOk; }
    bool readConfig();

private:
    Config();
    Q_DISABLE_COPY(Config)

    Repositories m_repositories;
    bool         m_isOk = false;
};

#endif
