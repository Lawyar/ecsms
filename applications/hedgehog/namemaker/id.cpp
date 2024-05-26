#include "id.h"

Id::Id(const std::vector<int> &id) : _id(id) {}

const std::vector<int> &Id::GetId() const { return _id; }

Id Id::GetChildId(int child_id) const { return Append(_id, {child_id}); }

bool Id::operator==(const Id &id) const { return _id == id._id; }

std::vector<int> Id::Append(const std::vector<int> &arr1,
                            const std::vector<int> &arr2) {
  
  auto res = arr1;
  res.insert(res.end(), arr2.begin(), arr2.end());
  return res;
}
