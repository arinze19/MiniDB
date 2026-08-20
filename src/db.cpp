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
    segment = std::make_unique<Segment>(segment_path); // what does this do? | doesn't seem to have a return type initially

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
    auto index = hash_index.find(key); // is this how we find if a key exists in a hash_index

    if (index == hash_index.end()) // why does it get to end? | is the hash_index.find function an O(n) algorithm?
    {
        return std::nullopt;
    };

    auto record = segment->read(index->second); // what is index->second and index->first

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
    std::vector<Record> records = segment->readAll(); // tutorial uses auto here, why?

    // append to hash index
    for (size_t i = 0; i < records.size(); i++)
    {
        const auto &record = records[i]; // why do we get the reference here?

        if (record.tombstone)
        {
            hash_index.erase(record.key);
            // so since a tombstone comes after the key we want to remove the key from the index
            // could we end up with a state where the the record key might not be in the hash_index? after compaction perhaps?
        }
        else
        {
            size_t offset = 0;
            for (size_t j = 0; j < i; j++)
            {
                offset += 4 + 4 + 1 + records[i].key.size() + records[i].value.size();
            }
            hash_index[record.key] = offset;
        }
    }
}
