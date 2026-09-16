# QUOM (In progress)

Quom is a key-value store built from scratch to explore the systems that power modern databases like RocksDB. It implements an LSM-tree storage engine, write-ahead logging and Bloom filters.

In this database, we can insert via `PUT`, delete via `DELETE`, and retreive values via `GET`. 

![logo](/images/quom-logo.png)

## About

We use a Log Structured Merge Tree so we can optimize our write operations. To do this, I've chosen the following:
*   Write-Ahead Log (WAL) using a Binary Encoding
*   MemTable implemented via a Skip List
*   Leveled Compaction

For our read optimization, we will do the following:
*   Bloom Filters
*   LRU Cache
*   SST Index Blocks

Still in progress.

## Architecture

The following diagram outlines the database structure. It branches out depending on operations. For example, if I were to modify by inserting/deleting a k-v pair, it is considered a `write` operation. Retrieving a value is a `read` operation. 


![architecture](/images/architecture.png)

**put on hold for now**
