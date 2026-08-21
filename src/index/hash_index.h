#pragma once

#include "index.h"
#include <unordered_map>

// we can have private Index too where the properties inherited from Index would be private to this class? Inspect
// can abstract class have private member variables?
class HashIndex : public Index
{
public:
    void put(const std::string &key, size_t offset) override; // override the virtual function from the base class

    std::optional<size_t> get(const std::string &key) const override;

    bool remove(const std::string &key) override;

    bool contains(const std::string &key) const override;

    std::vector<std::string> keys() const override;

    size_t size() const override;

private:
    std::unordered_map<std::string, size_t> table;
};