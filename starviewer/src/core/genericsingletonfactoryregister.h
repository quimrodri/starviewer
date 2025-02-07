#ifndef UDGGENERICSINGLETONFACTORYREGISTER_H
#define UDGGENERICSINGLETONFACTORYREGISTER_H

#include <QObject>

namespace udg {

/**
    Classe que ens facilita el registrar una classe en una GenericFactory que sigui singleton. Aquest template ens facilita la feina
    de registrar una classe en un GenericFactory. L'única condició és que el GenericFactory sigui Singleton. En comptes de crear
    una macroe s'ha escollit fer-ho amb un template ja que així ens estalviem els problemes de fer servir macros.
    La manera de registrar una classe és declarar una variable del tipus GenericSingletonFactoryRegister.

    Tot i això, aquesta classe s'hauria d'heredar i només fer servir les heredades d'aquesta. Com a exemple es té ExtensionFactoryRegister.
    Així doncs, aquesta classe queda només reservada per quan es vulgui implementar un nou factory i no per utilitzar-la directament en el
    registre de factory's.
  */
template <class AncestorType, class BaseClass, typename ClassIdentifier, class SingletonRegistry, class ParentType = QObject>
class GenericSingletonFactoryRegister {
public:
    /// Mètode constructor que ens serveix per registrar una classe amb un id concret en un Factory.
    /// @param id Identificador de la classe que es vol registrar
    /// @return
    GenericSingletonFactoryRegister(const ClassIdentifier &id)
    {
        SingletonRegistry::instance()->registerCreateFunction(id, createInstance);
    }

    /// Mètode auxiliar i que no s'hauria d'utilitzar directament
    static AncestorType* createInstance(ParentType *parent)
    {
        return dynamic_cast<AncestorType*>(new BaseClass(parent));
    }
};

}

#endif
