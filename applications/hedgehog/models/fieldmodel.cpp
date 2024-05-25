#include "fieldmodel.h"
const QMap<const ConnectNodeWidget *, QVector<const ConnectNodeWidget *>> &
FieldModel::GetConnectionMap() const {
  return _connection_map;
}

void FieldModel::Remove(const ConnectNodeWidget *start) {
  _connection_map.remove(start);
}

void FieldModel::AddConnection(const ConnectNodeWidget *start,
                               const ConnectNodeWidget *end) {
  _connection_map[start].append(end);
}

void FieldModel::RemoveConnection(const ConnectNodeWidget *start,
                                  const ConnectNodeWidget *end) {
  _connection_map[start].erase(std::find(_connection_map[start].begin(),
                                         _connection_map[start].end(), end));
}
