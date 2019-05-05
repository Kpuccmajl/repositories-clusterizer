#include "repository.h"

void Repository::setSloc(const uint &sloc)
{
    m_sloc = sloc;
}

void Repository::setLink(const QString &link)
{
    m_link = link;
}

void Repository::setSize(const float &size)
{
    m_size = size;
}

void Repository::setQuality(const float &quality)
{
    m_quality = quality;
}

void Repository::setLanguage(const float &language)
{
    m_language = language;
}

void Repository::setSupport(const float &support)
{
    m_support = support;
}
