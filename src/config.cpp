#include "config.h"
#include <QXmlStreamReader>
#include <QFile>
#include <QDebug>

#define CONFIG_PATH "../config.xml"

Config::Config()
{
    m_isOk = readConfig();
}

bool Config::readConfig()
{
    QFile file(CONFIG_PATH);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCritical() << Q_FUNC_INFO << "can't read config" << CONFIG_PATH;
        return false;
    }
    QXmlStreamReader xmlReader;
    xmlReader.setDevice(&file);
    QXmlStreamReader::TokenType token;
    repositories().clear();
    while (!xmlReader.hasError() && !xmlReader.isEndDocument()) {
        token = xmlReader.readNext();
        if (token == QXmlStreamReader::StartDocument)
            continue;
        if (token == QXmlStreamReader::StartElement) {
            if (xmlReader.name() == "repository") {
                Repository r;
                for (QXmlStreamAttribute & attr : xmlReader.attributes()) {
                    if (attr.name().toString() == QLatin1String("link"))
                        r.setLink(attr.value().toString().toUtf8());
                }
                m_repositories.append(r);
            }
        }
    }
    file.close();
    return true;
}
