#pragma once

#include <string>
#include <vector>
#include <optional>

class Index
{
public:
    // = 0 signifies that for every class constructed from this base
    // it must define the following member functions
    virtual void put(const std::string &key, size_t offset) = 0;

    virtual std::optional<size_t> get(const std::string &key) const = 0;

    virtual bool remove(const std::string &key) = 0;

    virtual bool contains(const std::string &key) const = 0;

    virtual std::vector<std::string> keys() const = 0;

    virtual size_t size() const = 0;

    // must be included for base classes
    // omitting would cause memory leak
    // when deleting a derived class, we call this ON the derived class
    virtual ~Index() = default;
};