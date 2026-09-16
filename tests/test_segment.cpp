#include "test_framework.h"
#include "storage/segment.h"
#include <filesystem>
#include <cstdio>

// Helper - cleanup test files after each test
static void cleanupSegment(const std::string& path) {
    if (std::filesystem::exists(path)) std::filesystem::remove(path);
}

// ─────────────────────────────────────────────

TEST(segment_write_and_read) {
    std::string path = "/tmp/test_seg_wr.seg";
    cleanupSegment(path);

    Segment seg(path);

    Record r{"hello", "world", false};
    size_t offset = seg.write(r);

    auto result = seg.read(offset);

    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(result->key,   "hello");
    ASSERT_EQ(result->value, "world");
    ASSERT_FALSE(result->tombstone);

    cleanupSegment(path);
}

TEST(segment_write_tombstone) {
    std::string path = "/tmp/test_seg_tomb.seg";
    cleanupSegment(path);

    Segment seg(path);

    Record r{"deleted_key", "", true};
    size_t offset = seg.write(r);

    auto result = seg.read(offset);

    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(result->key, "deleted_key");
    ASSERT_TRUE(result->tombstone);

    cleanupSegment(path);
}

TEST(segment_write_multiple_records) {
    std::string path = "/tmp/test_seg_multi.seg";
    cleanupSegment(path);

    Segment seg(path);

    size_t off1 = seg.write({"key1", "val1", false});
    size_t off2 = seg.write({"key2", "val2", false});
    size_t off3 = seg.write({"key3", "val3", false});

    ASSERT_EQ(seg.read(off1)->key, "key1");
    ASSERT_EQ(seg.read(off2)->key, "key2");
    ASSERT_EQ(seg.read(off3)->key, "key3");

    cleanupSegment(path);
}

TEST(segment_read_all) {
    std::string path = "/tmp/test_seg_all.seg";
    cleanupSegment(path);

    Segment seg(path);
    seg.write({"a", "1", false});
    seg.write({"b", "2", false});
    seg.write({"c", "3", true});  // tombstone

    auto records = seg.readAll();

    ASSERT_EQ(records.size(), size_t(3));
    ASSERT_EQ(records[0].key, "a");
    ASSERT_EQ(records[1].key, "b");
    ASSERT_TRUE(records[2].tombstone);

    cleanupSegment(path);
}

TEST(segment_size_tracking) {
    std::string path = "/tmp/test_seg_size.seg";
    cleanupSegment(path);

    Segment seg(path);
    ASSERT_EQ(seg.size(), size_t(0));

    seg.write({"k", "v", false});

    // 4 (key_size) + 4 (val_size) + 1 (tombstone) + 1 (key) + 1 (val)
    ASSERT_EQ(seg.size(), size_t(11));

    cleanupSegment(path);
}

TEST(segment_persists_across_reopen) {
    std::string path = "/tmp/test_seg_persist.seg";
    cleanupSegment(path);

    // Write in first instance
    {
        Segment seg(path);
        seg.write({"persistent", "sega", false});
    }

    // Read in second instance (simulates restart!)
    {
        Segment seg(path);
        auto records = seg.readAll();

        ASSERT_EQ(records.size(), size_t(1));
        ASSERT_EQ(records[0].key,   "persistent");
        ASSERT_EQ(records[0].value, "sega");
    }

    cleanupSegment(path);
}