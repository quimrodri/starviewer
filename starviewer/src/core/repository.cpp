#ifndef UDGREPOSITORY_CPP
#define UDGREPOSITORY_CPP

#include "repository.h"

namespace udg {

template< typename ItemType >
Repository<ItemType>::~Repository()
{
    this->cleanUp();
}

template<typename ItemType>
Identifier Repository<ItemType>::addItem(ItemType* item)
{
    Identifier id(m_nextIdentifierNumber++);
    m_itemList[id] = item;
    emit changed();
    return id;
}

template<typename ItemType>
ItemType* Repository<ItemType>::getItem(const Identifier &id) const
{
    return m_itemList.value(id);
}

template<typename ItemType>
QList<ItemType*> Repository<ItemType>::getItems() const
{
    return m_itemList.values();
}

template<typename ItemType>
void Repository<ItemType>::removeItem(const Identifier &id)
{
    m_itemList.remove(id);
    emit changed();
}

template<typename ItemType>
int Repository<ItemType>::getNumberOfItems() const
{
   return m_itemList.size();
}

template<typename ItemType>
QList<Identifier> Repository<ItemType>::getIdentifiers() const
{
    return m_itemList.keys();
}

template<typename ItemType>
void Repository<ItemType>::cleanUp()
{
    foreach (ItemType *item, m_itemList)
    {
        delete item;
    }

    m_itemList.clear();
    emit changed();
}

};  // End namespace udg

#endif
