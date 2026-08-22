#pragma once

#include "index.h"
#include <map> // This is a Red-Black Tree (Self balancing BST)

class BTreeIndex : public Index
{
public:
    void put(const std::string &key, const size_t offset) override;

    std::optional<size_t> get(const std::string &key) const override;

    bool remove(const std::string &key) override;

    bool contains(const std::string &key) const override;

    std::vector<std::string> keys() const override;

    size_t size() const override;

    // Range Query: Available only on BTreeIndex
    // returns all key offset pairs between start and end inclusive
    // restricting to a static two pair tuple -> std::pair
    // std::tuple is available if you need more fields
    std::vector<std::pair<std::string, size_t>> range(
        const std::string &start,
        const std::string &end) const;

private:
    std::map<std::string, size_t> tree;
};
