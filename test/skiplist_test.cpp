#include <gtest/gtest.h>

#include "skiplist.hpp"

TEST(SkipList, SearchOnEmptyListReturnsNullopt) {
    SkipList<std::string, std::string> list(16);
    EXPECT_EQ(list.search("missing"), std::nullopt);
}

TEST(SkipList, InsertThenSearchReturnsValue) {
    SkipList<std::string, std::string> list(16);
    list.insert("key", "value");
    EXPECT_EQ(list.search("key"), "value");
}

TEST(SkipList, InsertOverwritesExistingValue) {
    SkipList<std::string, std::string> list(16);
    list.insert("key", "value");
    list.insert("key", "new-value");
    EXPECT_EQ(list.search("key"), "new-value");
}

TEST(SkipList, SearchMissingKeyReturnsNullopt) {
    SkipList<std::string, std::string> list(16);
    list.insert("a", "1");
    list.insert("b", "2");
    EXPECT_EQ(list.search("missing"), std::nullopt);
}

TEST(SkipList, EraseRemovesExistingKey) {
    SkipList<std::string, std::string> list(16);
    list.insert("key", "value");
    EXPECT_TRUE(list.erase("key"));
    EXPECT_EQ(list.search("key"), std::nullopt);
}

TEST(SkipList, EraseOnMissingKeyReturnsFalse) {
    SkipList<std::string, std::string> list(16);
    EXPECT_FALSE(list.erase("missing"));
}

TEST(SkipList, MaintainsManyKeysInSortedInsertOrder) {
    SkipList<int, std::string> list(16);

    for (int i = 0; i < 200; i++) {
        list.insert(i, std::to_string(i));
    }

    for (int i = 0; i < 200; i++) {
        EXPECT_EQ(list.search(i), std::to_string(i));
    }
}
