#ifndef REPOSITORY_H
#define REPOSITORY_H

#include <QString>
#include <QStringList>

enum class ELanguage : u_int8_t { ANSIC, CPP }; const float languageWeight  = 1.0;
enum class EQuality : u_int8_t { BAD, GOOD };   const float qualityWeight   = 1.0;
enum class ESupport : u_int8_t { HIGH, LOW };   const float supportWeight   = 1.0;
enum class ESize : u_int8_t { SMALL, BIG };     const float sizeWeight      = 1.0;

struct Repository
{
public:
    Repository() {}

    Repository(QString li, float si, float qu, float la, float su)
        : m_link(li)
        , m_size(si)
        , m_quality(qu)
        , m_language(la)
        , m_support(su) {}

    QString name() const { return m_link.split('/').last(); }

    float x() const { return size() + language(); }
    float y() const { return quality() + support(); }

    float size() const { return m_size * sizeWeight; }
    float quality() const { return m_quality * qualityWeight; }
    float language() const { return m_language * languageWeight; }
    float support() const { return m_support * supportWeight; }
    int sloc() const { return m_sloc; }
    QString link() const { return m_link; }

    void setSloc(const uint &sloc);
    void setLink(const QString &link);
    void setSize(const float &size);
    void setQuality(const float &quality);
    void setLanguage(const float &language);
    void setSupport(const float &support);

private:
    QString m_link     = "";
    int     m_sloc     = 0;

    float   m_size     = 0.0;
    float   m_quality  = 0.0;
    float   m_language = 0.0;
    float   m_support  = 0.0;
};
using Repositories = QList<Repository>;

#endif // REPOSITORY_H
