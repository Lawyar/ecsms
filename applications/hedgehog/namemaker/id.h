#pragma once

#include <vector>

using PartId = int;

class Id;

using BlockId = Id;
using NodeId = Id;

class Id {
public:
  Id(const std::vector<PartId> &id);
  virtual ~Id() = default;
  const std::vector<PartId> &GetId() const;
  Id GetParentId() const;
  Id GetChildId(PartId child_id) const;
  bool operator==(const Id &id) const;
  bool operator!=(const Id &id) const;
  bool operator<(const Id &id) const;

protected:
  static std::vector<PartId> Append(const std::vector<PartId> &arr1,
                                    const std::vector<PartId> &arr2);

protected:
  std::vector<PartId> _id;
};
