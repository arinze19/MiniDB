#include "db.h"
#include "index/btree_index.h"
#include "index/hash_index.h"
#include <filesystem>
#include <iostream>
#include <stdexcept> // provides cpp exception type: std::runtime_error

// TODO: Insepct
size_t MiniDB::encodeLocation(size_t segment_idx, size_t offset) const
{
    return (segment_idx << 32) | (offset & 0xFFFFFFFF);
}

std::pair<size_t, size_t> MiniDB::decodeLocation(size_t encoded) const
{
    size_t segment_idx = encoded >> 32;
    size_t offset = encoded & 0xFFFFFFFF;
    return {segment_idx, offset};
}

// Factory: Creates the right index type
// What is a factory?
std::unique_ptr<Index> MiniDB::createIndex(IndexType type)
{
    switch (type)
    {
    case IndexType::HASH:
        return std::make_unique<HashIndex>();
    case IndexType::BTREE:
        return std::make_unique<BTreeIndex>();
    default:
        std::cout << "Index type is unknown - Defaulting to HashIndex" << std::endl;
        return std::make_unique<HashIndex>();
    }
}

MiniDB::MiniDB(const std::string &dir, IndexType type) : data_dir(dir)
{
    if (!std::filesystem::exists(data_dir))
    {
        std::filesystem::create_directories(data_dir);
    }

    index = createIndex(type); // create index type

    std::string segment_path = data_dir + "/data.seg";
    // create a segment using the segment path
    // is .seg a recognized file format or could it have been end extension? -> for all we know the segment.seg file represents the class from segment.cpp
    // could be made with Segment* segment = new Segment(segment_path) but using this to better manage memory leaks if we forget to delete segment
    segment_manager = std::make_unique<SegmentManager>(segment_path);

    // Build index
    buildIndex();

    std::cout << "[MiniDB] Opened with"
              << (type == IndexType::HASH ? "HashIndex" : "BTreeIndex")
              << " | Keys: " << index->size() << " keys loaded \n"
              << " | Segments: " << segment_manager->segmentCount() << std::endl;
}

void MiniDB::put(const std::string &key, const std::string &value)
{
    Record record = Record{key, value, false}; 

    auto [segment_idx, offset] = segment_manager->write(record);

    index->put(key, encodeLocation(segment_idx, offset)); // setting index keys
}

std::optional<std::string> MiniDB::get(const std::string &key)
{
    // check if key exists in index
    // returns an iterator
    // using auto here so iterator type is automatically inferred as opposed to writing out the whole type of the map
    // auto offset = index->get(key);
    auto encoded = index->get(key);

    // maps must return a value regardless
    // so in the instance where key is not found
    // it returns an iterator pointing to the sentinel just after the end of the map
    // also if an item is not in the index it is most likely tombstoned
    if (!encoded.has_value())
    {
        std::cout << "[MiniDB] key not found: " << key << "\n";
        return std::nullopt;
    };

    // using auto since it returns optional
    // auto record = segment->read(*offset)
    auto [segment_idx, offset] = decodeLocation(*encoded);

    auto record = segment_manager->read(segment_idx, offset);

    // adding !record.has_value() here to
    // prevent invalid state crashes in the system
    if (record->tombstone || !record.has_value())
    {
        return std::nullopt;
    }

    return record->value;
}

bool MiniDB::remove(const std::string &key)
{
    // null case
    if (!index->contains(key))
    {
        std::cout << "[MiniDB]: Key not found" << "\n";
        return false;
    }

    Record tombstone{key, "", true};

    // add to db
    auto [segment_idx, offset] = segment_manager->write(tombstone);

    // update index
    // index working accross multiple segments
    index->remove(key);

    return true;
}

std::vector<std::pair<std::string, std::string>> MiniDB::range(const std::string &start, const std::string &end)
{
    // what is index.get()? does this get you the smart pointer?
    // .get() -> gives access to the raw pointer
    //  dynamic returns a null pointer if the object isn't the requested type?
    // dynamic_cast v static_cast
    //  dynamic -> runs type check at runtime | used for safely navigating polymorphic class hierarchies
    //  static -> runs type check at compile time
    auto *btree = dynamic_cast<BTreeIndex *>(index.get());

    if (!btree)
    {
        throw std::runtime_error(
            "Range queries require BTreeIndex! "
            "Restart with --index=btree");
    }

    auto pairs = btree->range(start, end);

    // reserver space for incoming items
    std::vector<std::pair<std::string, std::string>> result;
    result.reserve(pairs.size());

    for (const auto &[key, encoded] : pairs)
    {
        auto [segment_idx, offset] = decodeLocation(encoded);

        auto record = segment_manager->read(segment_idx, offset);

        if (record.has_value() && !record->tombstone)
        {
            result.emplace_back(key, record->value);
        }
    }

    return result;
}

void MiniDB::buildIndex()
{
    // get all records from the segment? what if there are multiple segments
    auto segments = segment_manager->readAll();
    size_t current_offset = 0;

    for (const auto &[segment_idx, offset, record] : segments)
    {
        if (record.tombstone)
        {
            index->remove(record.key); // what does erase do?
        }
        else
        {
            index->put(record.key, encodeLocation(segment_idx, offset));
        }

        // update offset
        current_offset += 4 + 4 + 1 + record.key.size() + record.value.size();
    }
}

void MiniDB::compact()
{
    segment_manager->compact(*index);
}

size_t MiniDB::indexSize() const
{
    return index->size();
}

size_t MiniDB::getSegmentCount() const
{
    return segment_manager->segmentCount();
}

std::vector<std::string> MiniDB::keys() const
{
    // get the keys in the index
    return index->keys();
}
