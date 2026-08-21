#pragma once

#include <string>
#include <vector>
#include <optional>

// Base abstract class for index
// how do we solidify this as an abstract class?
class Index
{
public:
    // = 0 signifies that for every class constructed from this base
    // it must define the following member functions
    virtual void put(const std::string &key, size_t offset) = 0;

    virtual std::optional<size_t> get(const std::string &key) const = 0; // const meaning we will not modify the value?

    virtual bool remove(const std::string &key) = 0;

    virtual bool contains(const std::string &key) const = 0; // promises not to modify the Index members, side effects

    virtual std::vector<std::string> keys() const = 0; // get all keys

    virtual size_t size() const = 0;

    // must be included for base classes
    // omitting would cause memory leak
    // when deleting a derived class, we call this ON the derived class
    virtual ~Index() = default;
};