#ifndef ROCKS_SERVICE_H
#define ROCKS_SERVICE_H

#include "msg.h"
#include "nram_storage/rocksengine.h"

extern RocksEngine *GlobalRocksEngine;


void start_rocks_service(int num_threads);

void *process_request(void *arg);

KVMsg *handle_kv_get(KVMsg *msg);

KVMsg *handle_kv_put(KVMsg *msg);

#endif //ROCKS_SERVICE_H
