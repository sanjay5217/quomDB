# QUOM

Quom is a distributed key-value store built from scratch to explore the systems that power modern databases like RocksDB, Cassandra, and TiKV. It implements an LSM-tree storage engine, write-ahead logging, Bloom filters, and distributed replication, with an interactive dashboard for visualizing consensus, replication, and failure recovery.

![logo](/images/quom-logo.png)

## Phase 1 - Storage Engine

This phase specifically builds the storage engine of a single node database. This incldues the following
*   Write Ahead Log (WAL)
*   MemTable 
*   Sorted String Table (SST)
*   Log Structured Merge Tree (LSM)
*   Bloom Filter

