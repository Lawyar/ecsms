#include "id.h"

Id::Id(const std::vector<PartId> &id) : _id(id) {}

const std::vector<PartId> &Id::GetId() const { return _id; }

Id Id::GetParentId() const { 
  if (_id.empty())
    return std::vector<PartId>{};

  return Id(std::vector<PartId>(_id.begin(), std::prev(_id.end())));
}

Id Id::GetChildId(PartId child_id) const { return Append(_id, {child_id}); }

bool Id::operator==(const Id &id) const { return _id == id._id; }

bool Id::operator!=(const Id &id) const { return !(_id == id._id); }

bool Id::operator<(const Id &id) const { return _id < id._id; }

std::vector<PartId> Id::Append(const std::vector<PartId> &arr1,
                            const std::vector<PartId> &arr2) {
  
  auto res = arr1;
  res.insert(res.end(), arr2.begin(), arr2.end());
  return res;
}
