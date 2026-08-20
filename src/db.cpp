#include "db.h"
#include <filesystem>
#include <iostream>

MiniDB::MiniDB(const std::string &dir) : data_dir(dir)
{
    if (!std::filesystem::exists(data_dir))
    {
        std::filesystem::create_directories(data_dir);
    }

    std::string segment_path = data_dir + "/data.seg";
    // create a segment using the segment path
    // is .seg a recognized file format or could it have been end extension? -> for all we know the segment.seg file represents the class from segment.cpp
    segment = std::make_unique<Segment>(segment_path);

    // Build index
    buildHashIndex();

    std::cout << "Database loaded at " << data_dir << "\n";
    std::cout << "Index loaded with " << hash_index.size() << "\n";
}

void MiniDB::put(const std::string &key, const std::string &value)
{
    Record record = Record{key, value, false}; // is this how we define objects

    size_t offset = segment->write(record);

    hash_index[key] = offset; // setting hash keys
}

std::optional<std::string> MiniDB::get(const std::string &key)
{
    // check if key exists in index
    // returns an iterator
    // using auto here so iterator type is automatically inferred as opposed to writing out the whole type of the map
    auto index = hash_index.find(key);

    // maps must return a value regardless
    // so in the instance where key is not found
    // it returns an iterator pointing to the sentinel just after the end of the map
    // also if an item is not in the index it is most likely tombstoned
    if (index == hash_index.end())
    {
        return std::nullopt;
    };

    // index->second references the value of the key selected above
    // using auto since it returns optional
    auto record = segment->read(index->second);

    // we could probably do with a !record.has_value()
    // here but that's severely underestimating the accuracy of our
    // database
    if (record->tombstone)
    {
        return std::nullopt;
    }

    return record->value;
}

bool MiniDB::remove(const std::string &key)
{
    // null case
    if (hash_index.find(key) == hash_index.end())
    {
        std::cout << "[MiniDB]: Key not found" << "\n";
        return false;
    }

    Record tombstone{key, "", true};

    // add to db
    size_t offset = segment->write(tombstone);

    // update index
    hash_index[key] = offset;

    return true;
}

void MiniDB::buildHashIndex()
{
    // get all records from the segment? what if there are multiple segments
    auto records = segment->readAll();
    size_t offset = 0;

    for (const auto &record : records)
    {
        if (record.tombstone)
        {
            hash_index.erase(record.key); // what does erase do?
        }
        else
        {
            hash_index[record.key] = offset;
        }

        // update offset
        offset += 4 + 4 + 1 + record.key.size() + record.value.size();
    }
}
