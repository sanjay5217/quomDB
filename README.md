# QUOM

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


### Write

The write path is the most complex, but is highly optimized through the LSM Tree structured. LSM is a Log-Structured Merge Tree is a 'disk' based data structure, mainly used to optimize writes. Instead of modifying in place memory like B-trees, LSMs simply store them in memory and sequentially update in disk for persistence. This offers a much faster write latency, which is why I went with it. 

**Write-Ahead Logs** 

My database must provide durability while avoiding expensive random disk writes. Since memory-based structures are lost after crashes, we use a Write-Ahead Log (WAL)to record every mutation before modifying the in-memory MemTable. The WAL acts as the durable source of truth until the data is flushed into persistent SST files.

Optimizations:

* **Binary Encoding (Space Optimization):** There were several options for serializing log entries. JSON would have been simpler to inspect and debug, but it introduces unnecessary storage overhead and parsing costs. Since the write-ahead log is on the critical write path, I chose a compact binary format instead. This reduces disk usage, minimizes serialization overhead, and improves write performance.

![binary-encoding](/images/binary-encode.png)

* **Checksums (Durability Optimization):** A crash or hardware fault can leave log records partially written or corrupted. To detect this, each record stores a CRC32 checksum computed over its contents. During recovery, the checksum is recomputed and compared against the stored value. A mismatch indicates that the record has been corrupted, allowing the database to detect invalid entries rather than replaying potentially damaged data.

    Important Note: For Quom, I used CRC32 as a checksum mechanism to detect accidental corruption in WAL records. I treated the CRC32 implementation itself as a black box because it is a well-established algorithm with optimized implementations available. My focus was integrating it into the storage pipeline: generating the checksum during writes, persisting it alongside records, and validating it during recovery.


