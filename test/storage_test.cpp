#include <gtest/gtest.h>

#include "storage.hpp"

TEST(StorageEngine, GetOnMissingKeyReturnsNullopt) {
    StorageEngine engine;
    EXPECT_EQ(engine.get("missing"), std::nullopt);
}

TEST(StorageEngine, PutThenGetReturnsValue) {
    StorageEngine engine;
    engine.put("key", "value");
    EXPECT_EQ(engine.get("key"), "value");
}

TEST(StorageEngine, PutOverwritesExistingValue) {
    StorageEngine engine;
    engine.put("key", "value");
    engine.put("key", "new-value");
    EXPECT_EQ(engine.get("key"), "new-value");
}

TEST(StorageEngine, DelRemovesExistingKey) {
    StorageEngine engine;
    engine.put("key", "value");
    EXPECT_TRUE(engine.del("key"));
    EXPECT_EQ(engine.get("key"), std::nullopt);
}

TEST(StorageEngine, DelOnMissingKeyReturnsFalse) {
    StorageEngine engine;
    EXPECT_FALSE(engine.del("missing"));
}
