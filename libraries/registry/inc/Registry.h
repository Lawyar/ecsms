#pragma once

#include <unordered_map>
#include <memory>
#include <functional>

template<typename Base, typename Key, typename... Args>
class Registry
{
public:
    using FactoryFunc = std::function<std::unique_ptr<Base>(Args...)>;

    template<typename Derived>
    void registerClass(const Key &key);

    void registerFactory(const Key &key, const FactoryFunc factory);

    std::unique_ptr<Base> construct(const Key &key, Args... args) const;

private:
    template<typename Derived>
    static std::unique_ptr<Base> constructDerived(Args... args);

    std::unordered_map<Key, FactoryFunc> m_map;
};

template <typename Base, typename Key, typename... Args>
template <typename Derived>
void Registry<Base, Key, Args...>::registerClass(const Key &key) {
  m_map[key] = &constructDerived<Derived>;
}

template <typename Base, typename Key, typename... Args>
void Registry<Base, Key, Args...>::registerFactory(const Key &key,
                                                   const FactoryFunc factory) {
  m_map[key] = factory;
}

template <typename Base, typename Key, typename... Args>
std::unique_ptr<Base> Registry<Base, Key, Args...>::construct(const Key &key,
                                                              Args... args) const {
  return m_map.at(key)(std::forward<Args>(args)...);
}

template <typename Base, typename Key, typename... Args>
template <typename Derived>
std::unique_ptr<Base>
Registry<Base, Key, Args...>::constructDerived(Args... args) {
  return std::make_unique<Derived>(std::forward<Args>(args)...);
}
