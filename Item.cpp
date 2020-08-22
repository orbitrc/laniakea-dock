#include "Item.h"

#include <QUuid>

Item::Item(QObject *parent)
    : QObject(parent)
{
    this->setId(QUuid::createUuid().toString(QUuid::StringFormat::WithoutBraces));
}

QString Item::id() const
{
    return this->m_id;
}

void Item::setId(const QString &id)
{
    this->m_id = id;
}

bool Item::pinned() const
{
    return this->m_pinned;
}

void Item::setPinned(bool val)
{
    this->m_pinned = val;
}

Item::ItemType Item::type() const
{
    return this->m_type;
}

void Item::setType(ItemType type)
{
    this->m_type = type;
}

std::optional<QString> Item::cls() const
{
    return this->m_class;
}

void Item::setCls(const QString &cls)
{
    this->m_class = cls;
}

void Item::clearCls()
{
    this->m_class.reset();
}

QList<int> Item::windows() const
{
    return this->m_wIds;
}

void Item::appendWindow(int wId)
{
    this->m_wIds.append(wId);
}
