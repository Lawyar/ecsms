#pragma once

#include <QMap>
#include <QSet>

template <class Key, class Value>
QSet<Key> SubstractionKeys(const QMap<Key, Value> &map1,
                           const QMap<Key, Value> &map2) {
  QSet<Key> set1 = map1.keys().toSet();
  QSet<Key> set2 = map2.keys().toSet();
  return set1 - set2;
}
