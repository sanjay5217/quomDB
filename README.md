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

* **Batch Writes (Throughput Optimization):** Persisting every write immediately with `fsync` provides the strongest durability guarantees, but it also incurs significant I/O overhead. To improve throughput, I buffer log records in memory and flush them to disk once the buffer reaches a configurable batch size. This amortizes the cost of `fsync` across multiple writes. The tradeoff is durability: if the database crashes before the buffered entries are flushed, those pending records are lost.

* **Segmentation (Scalablity Optimization):** Allowing the write-ahead log to grow indefinitely would make recovery slower and log management more difficult. Instead, the WAL is divided into fixed-size segments. Once a segment reaches its size limit, a new segment is created for subsequent writes. After the corresponding MemTable has been successfully flushed to an SSTable, obsolete WAL segments can be safely deleted. This keeps recovery times bounded and simplifies storage management.

* **Checksums (Durability Optimization):** A crash or hardware fault can leave log records partially written or corrupted. To detect this, each record stores a CRC32 checksum computed over its contents. During recovery, the checksum is recomputed and compared against the stored value. A mismatch indicates that the record has been corrupted, allowing the database to detect invalid entries rather than replaying potentially damaged data.


