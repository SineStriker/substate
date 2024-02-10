#include "entity.h"
#include "entity_p.h"

#include <shared_mutex>

#include <substate/nodehelper.h>

namespace Substate {

    static std::shared_mutex factoryLock;

    static std::unordered_map<std::string, Entity::Factory> factoryManager;

    static inline Entity::Factory getFactory(const std::string &key) {
        std::shared_lock<std::shared_mutex> lock(factoryLock);
        auto it = factoryManager.find(key);
        if (it == factoryManager.end()) {
            throw std::runtime_error("Substate::Entity: Unknown Entity key: " + key);
        }
        return it->second;
    }

    const std::string entity_dyn_key = "_q_substate_entity_type";

    EntityPrivate::EntityPrivate(Node *node) : NodeExtra(node) {
    }

    EntityPrivate::~EntityPrivate() {
        if (m_node->isFree()) {
            setInternalData(nullptr);
            delete m_node;
        }
    }

    void EntityPrivate::init() {
    }

    Entity::~Entity() {
    }

    void Entity::registerFactory(const std::string &key, Entity::Factory fac) {
        std::unique_lock<std::shared_mutex> lock(factoryLock);
        factoryManager[key] = fac;
    }

    void Entity::removeFactory(const std::string &key) {
        std::unique_lock<std::shared_mutex> lock(factoryLock);
        factoryManager.erase(key);
    }

    Entity *Entity::createEntity(Node *node) {
        return getFactory(node->dynamicData(entity_dyn_key).toString())(node);
    }

    Entity *Entity::extractEntity(Node *node) {
        return static_cast<EntityPrivate *>(NodeHelper::getExtra(node))->q_func();
    }

    Node *Entity::internalData() const {
        Q_D(const Entity);
        return d->internalData();
    }

    Entity::Entity(EntityPrivate &d, QObject *parent) : QObject(parent), d_ptr(&d) {
        d.q_ptr = this;
        d.init();
    }

}