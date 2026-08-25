#pragma once

#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <thread>
#include <atomic>
#include "segment.h"
#include "../index/index.h"

/**
 * Manage
 * 1. Compaction process and thread
 * 2. segment files
 */
class SegmentManager
{
public:
    static constexpr size_t COMPACTION_THRESHOLD = 3;

    explicit SegmentManager(const std::string &data_dir);

    // TODO: inspect - Stop background thread cleanly
    ~SegmentManager();

    // <segment_index, offset>
    std::pair<size_t, size_t> write(const Record &Record);

    std::optional<Record> read(size_t segment_index, size_t offset);

    // Read all records on active segments for index rebuild
    // <segment_index, offset, record>
    std::vector<std::tuple<size_t, size_t, Record>> readAll();

    // TODO: remove
    // Manually run compaction
    void compact(Index &index);

    size_t segmentCount() const;

private:
    std::string data_dir;

    // Get all segments
    // Can put smart pointers in a vector?? 😳
    std::vector<std::unique_ptr<Segment>> segments;

    // Active segment writing to; default to last to keep readable
    // Return raw pointer
    Segment *getActiveSegment();

    void addSegment();

    void loadAllSegments();

    // Generate segment name from index
    std::string segmentPath(size_t idx) const;

    // Background compaction thread
    std::thread compaction_thread;
    // Thread safe boolean
    // What is atomic?
    std::atomic<bool> should_stop{false}; 
    mutable std::mutex segments_mutex;

    // Background loop
    void compactionLoop(Index *index);
};