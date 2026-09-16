#include "test_framework.h"
#include "storage/memtable.h"

TEST(memtable_put_and_get) {
    Memtable mt;
    mt.put("name", "Arinze");

    auto result = mt.get("name");
    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(*result, "Arinze");
}

TEST(memtable_get_missing_key) {
    Memtable mt;
    auto result = mt.get("nonexistent");
    ASSERT_FALSE(result.has_value());
}

TEST(memtable_overwrite_key) {
    Memtable mt;
    mt.put("key", "value1");
    mt.put("key", "value2");

    auto result = mt.get("key");
    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(*result, "value2");  // Latest value wins!
}

TEST(memtable_remove_returns_null) {
    Memtable mt;
    mt.put("key", "value");
    mt.remove("key");

    // get() returns nullopt for tombstoned keys
    auto result = mt.get("key");
    ASSERT_FALSE(result.has_value());

    // But contains() still returns true (tombstone IS there!)
    ASSERT_TRUE(mt.contains("key"));
}

TEST(memtable_remove_nonexistent_key) {
    Memtable mt;
    // Should not throw - just writes a tombstone
    mt.remove("ghost_key");
    ASSERT_TRUE(mt.contains("ghost_key"));
}

TEST(memtable_size_tracking) {
    Memtable mt;
    ASSERT_EQ(mt.sizeBytes(), size_t(0));

    mt.put("key", "value");
    ASSERT(mt.sizeBytes() > 0);

    size_t size_before = mt.sizeBytes();
    mt.put("key", "longer_value_than_before");
    ASSERT(mt.sizeBytes() > size_before);
}

TEST(memtable_flush_returns_sorted) {
    Memtable mt;
    mt.put("zebra", "z");
    mt.put("apple", "a");
    mt.put("mango", "m");

    auto records = mt.flush();

    ASSERT_EQ(records.size(), size_t(3));

    // std::map keeps keys sorted - flush must be sorted!
    ASSERT_EQ(records[0].key, "apple");
    ASSERT_EQ(records[1].key, "mango");
    ASSERT_EQ(records[2].key, "zebra");
}

TEST(memtable_flush_includes_tombstones) {
    Memtable mt;
    mt.put("alive", "yes");
    mt.remove("dead");

    auto records = mt.flush();
    ASSERT_EQ(records.size(), size_t(2));

    // Find the tombstone
    bool found_tombstone = false;
    for (const auto& r : records) {
        if (r.key == "dead" && r.tombstone) found_tombstone = true;
    }
    ASSERT_TRUE(found_tombstone);
}

TEST(memtable_clear_resets_size) {
    Memtable mt;
    mt.put("a", "1");
    mt.put("b", "2");

    ASSERT_FALSE(mt.isEmpty());
    ASSERT(mt.sizeBytes() > 0);

    mt.clear();

    ASSERT_TRUE(mt.isEmpty());
    ASSERT_EQ(mt.sizeBytes(), size_t(0));
}

TEST(memtable_full_triggers_at_limit) {
    // Create tiny memtable (200 bytes max)
    Memtable mt(200);
    ASSERT_FALSE(mt.isFull());

    // Write enough to exceed limit
    for (int i = 0; i < 5; i++) {
        mt.put("key" + std::to_string(i), "value" + std::to_string(i));
    }

    ASSERT_TRUE(mt.isFull());
}