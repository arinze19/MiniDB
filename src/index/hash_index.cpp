#include "hash_index.h"


void HashIndex::put(const std::string &key, size_t offset)
{
    // get table and add
    table[key] = offset;
}

std::optional<size_t> HashIndex::get(const std::string &key) const
{
    // verify if key in index
    auto it = table.find(key);

    if (it == table.end())
    {
        return std::nullopt;
    }

    return it->second;
}

bool HashIndex::remove(const std::string &key)
{
    // remove item from table
    auto it = table.find(key);

    if (it == table.end())
    {
        return false;
    }

    table.erase(key);

    return true;
}

bool HashIndex::contains(const std::string &key) const
{
    // can also use table.count(key) > 0 | Didn't know aobut table.count
    auto it = table.find(key);

    if (it == table.end())
    {
        return false;
    }

    return true;
}

std::vector<std::string> HashIndex::keys() const
{
    std::vector<std::string> result;
    // Pre-allocate memory: why preallocate memory thought? vectors are dynamically sized
    // Minor optimisation to prevent dynamic resizing
    result.reserve(table.size());

    for (const auto &[key, offset] : table)
    { // why reference? so we don't make completely new copy each time
        result.push_back(key);
    }

    return result;
}

size_t HashIndex::size() const
{
    return table.size();
}