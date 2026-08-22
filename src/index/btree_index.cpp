#include "btree_index.h"

void BTreeIndex::put(const std::string &key, size_t offset)
{
    tree[key] = offset; // what does the tree balance by? key or offset?
}

std::optional<size_t> BTreeIndex::get(const std::string &key) const
{
    auto it = tree.find(key);

    if (it == tree.end())
    {
        return std::nullopt;
    }

    return it->second;
}

bool BTreeIndex::remove(const std::string &key)
{
    auto it = tree.find(key);

    if (it == tree.end())
    {
        return false;
    }

    tree.erase(it->first);

    return true;
}

bool BTreeIndex::contains(const std::string &key) const
{
    return tree.count(key) > 0;
}

std::vector<std::string> BTreeIndex::keys() const
{
    std::vector<std::string> result;
    result.reserve(tree.size());

    for (const auto &[key, offset] : tree)
    {
        result.push_back(key);
    }

    return result;
}

size_t BTreeIndex::size() const
{
    return tree.size();
}

std::vector<std::pair<std::string, size_t>> BTreeIndex::range(const std::string &start, const std::string &end) const
{
    std::vector<std::pair<std::string, size_t>> result;

    // Find the first key >= start
    // if key not found it gives the next meant to be after
    // upper and lower bound are not functions available to unsorted map since it keeps no ordering of its elements
    auto it = tree.lower_bound(start);

    // Find the last key > end
    // find the key strictly greater than the end key
    // essentially this is the same iterator returned when we try to use tree.find(key)
    // on a key that does not exist in the tree, returning tree.end() which represents one position after the final element
    auto stop = tree.upper_bound(end);

    while (it != stop)
    {
        // this could also be
        // result.push_back({ it->first, it->second });
        result.emplace_back(it->first, it->second);
        // temporarily preferred here because we do not need to return anything
        // i++ will generally return the old copy of it
        ++it;
    }

    return result;
}