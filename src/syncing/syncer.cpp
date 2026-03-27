#include "syncer.h"
#include "abstractnotesprovider.h"
#include "db/db.h"

Syncer::Syncer(QObject *parent)
    : QObject(parent)
    , m_provider(nullptr) // online service handler
{
}

void Syncer::setProvider(AbstractNotesProvider *provider)
{
    this->m_provider = std::move(provider);
    this->m_provider->setParent(this);
    this->m_provider->disconnect();
    this->setConections();
}
