# QUOM

Quom is a distributed key-value store built from scratch to explore the systems that power modern databases like RocksDB. It implements an LSM-tree storage engine, write-ahead logging, Bloom filters, and distributed replication, with an interactive dashboard for visualizing consensus, replication, and failure recovery.

In this database, we can insert via `PUT`, delete via `DELETE`, and retreive values via `GET`. 

![logo](/images/quom-logo.png)

## Phase 1 - Storage Engine

We use a Log Structured Merge Tree so we can optimize our write operations. To do this, I've chosen the following:
*   Write-Ahead Log (WAL) using a Binary Encoding
*   MemTable implemented via a Skip List
*   Leveled Compaction

For our read optimization, we will do the following:
*   Bloom Filters
*   LRU Cache
*   SST Index Blocks

Still in progress.


