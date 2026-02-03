# NRAM Table Access Method (NeurDB)

**NRAM** is a custom **PostgreSQL Table Access Method (TAM)** developed for **NeurDB**.
It provides a **KV-style storage backend built on RocksDB**, while preserving PostgreSQL’s native transactional semantics.
NRAM is designed to integrate tightly with **NeurCC**, NeurDB’s learned concurrency control framework.

---

## Installation

Build and install the extension:

```bash
make clean
make
sudo make install
```

Run unit tests:

```bash
make unit-test
```

Run both unit and integration tests:

```bash
make test
```

---

## Using NRAM Tables

To create a table backed by NRAM, specify `USING nram` at table creation time:

```sql
CREATE TABLE y (
    a INT,
    b TEXT,
    PRIMARY KEY (a)
) USING nram;
```

Tables created with `USING nram` are managed by the NRAM Table Access Method.
All accesses to such tables are handled by NRAM and are subject to NeurCC-based concurrency control.

---

## NeurCC

**NeurCC** follows the **CCaaLF** design, which models concurrency control as a **learnable function** that maps database state to control actions.

When NeurCC is enabled:

* Concurrency control decisions are made **per operation**, rather than per transaction.
* The control logic is implemented as a configurable **agent function**.
* Users can optimize or replace the agent function via external scripts or policy files.

### Loading Built-in Policies

For example, to switch the agent function to a classic **2PL** policy:

```sql
SELECT nram_load_policy('2pl');
```

### Loading Custom Policies

Users can also define custom policies in a file and load them at runtime:

```sql
SELECT nram_load_policy('FILE/PATH');
```

---

## Benchmarking

### YCSB Benchmark

To run YCSB-based benchmarks:

```bash
python benchmark/ycsb.py
```

### Training / Optimization

To benchmark and optimize the NeurCC training process:

```bash
python optimizer/train.py
```

Refer to the provided script `optimizer/train.py` for examples of policy optimization workflows.


## Reference

- CCaaLF: https://arxiv.org/abs/2503.10036.