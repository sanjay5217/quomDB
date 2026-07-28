#include <gtest/gtest.h>

#include "storage.hpp"

// Tests the Storage Engine API

TEST(StorageEngineTest, GetOnMissingKeyReturnsNullopt) {
    StorageEngine engine;
    EXPECT_EQ(engine.get("missing"), std::nullopt);
}

TEST(StorageEngineTest, PutThenGetReturnsValue) {
    StorageEngine engine;
    engine.put("key", "value");
    EXPECT_EQ(engine.get("key"), "value");
}

TEST(StorageEngineTest, PutOverwritesExistingValue) {
    StorageEngine engine;
    engine.put("key", "value");
    engine.put("key", "new-value");
    EXPECT_EQ(engine.get("key"), "new-value");
}

TEST(StorageEngineTest, DelRemovesExistingKey) {
    StorageEngine engine;
    engine.put("key", "value");
    EXPECT_TRUE(engine.del("key"));
    EXPECT_EQ(engine.get("key"), std::nullopt);
}

TEST(StorageEngineTest, DelOnMissingKeyReturnsFalse) {
    StorageEngine engine;
    EXPECT_FALSE(engine.del("missing"));
}
