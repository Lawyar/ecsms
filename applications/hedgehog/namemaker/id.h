#pragma once

#include <vector>

class Id {
public:
  Id(const std::vector<int> & id);
  virtual ~Id() = default;
  const std::vector<int> &GetId() const;
  Id GetChildId(int child_id) const;
  bool operator==(const Id &id) const;

protected:
  static std::vector<int> Append(const std::vector<int> &arr1,
                                 const std::vector<int> &arr2);

protected:
  std::vector<int> _id;
};
