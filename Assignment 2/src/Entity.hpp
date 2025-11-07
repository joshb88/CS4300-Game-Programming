#pragma once

#include "Components.hpp"
#include <string>
#include <tuple>

class EntityManager;

using ComponentTuple = std::tuple<
    CTransform,
    CShape,
    CCollision,
    CInput,
    CScore,
    CLifespan,
    CWeapon
>;

class Entity
{
    friend class EntityManager;

    ComponentTuple m_components;
    bool m_alive = true;
    const std::string m_tag = "default";
    size_t m_id = 0;

    struct Token{};

    Entity() = delete;

public:
    Entity(size_t id, const std::string& tag, Token)
    : m_id(id)
    , m_tag(tag)
    {}

    bool isAlive() const
    {
        return m_alive;
    }

    void destroy()
    {
        m_alive = false;
    }

    size_t id() const
    {
        return m_id;
    }

    const std::string& tag() const
    {
        return m_tag;
    }

    template <typename T>
    bool has() const
    {
        return get<T>().exists;
    }

    template<typename T, typename... TArgs>
    void add(TArgs&&... args)
    {
        auto& component = get<T>();
        component = T(std::forward<TArgs>(args)...);
        component.exists = true;
    }

    template<typename T>
    T& get()
    {
        return std::get<T>(m_components);
    }

    template<typename T>
    const T& get() const
    {
        return std::get<T>(m_components);
    }

    template<typename T>
    void remove()
    {
        get<T>().exists = false;
    }
};