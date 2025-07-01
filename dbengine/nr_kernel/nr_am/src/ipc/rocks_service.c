#include "rocks_service.h"

#include "threadpool.h"


RocksEngine *GlobalRocksEngine = NULL;

void start_rocks_service(int num_threads) {
    GlobalRocksEngine = rocksengine_open();     // initialize RocksDB engine
    ThreadPool pool;
    threadpool_init(&pool, num_threads);        // Initialize thread pool
    KVChannel *channel = KVChannelInit(
        "rocks_service_channel",
        true
    );                                          // initialize IPC channel

    while (true) {
        KVMsg *msg = (KVMsg *)palloc0(sizeof(KVMsg));
        bool ok = KVChannelPopMsg(channel, msg, true);
        if (!ok) {
            continue;
        }
        threadpool_add_task(&pool, process_request, msg);
    }
    rocksengine_destroy(&GlobalRocksEngine->engine);
    threadpool_destroy(&pool);
    KVChannelDestroy(channel);
}

void *process_request(void *arg) {
    KVMsg *msg = (KVMsg *)arg;
    KVMsg *resp = NULL;

    char chan_name[64];
    snprintf(chan_name, sizeof(chan_name), "kv_resp_%u", msg->header.respChannel);
    KVChannel *resp_chan = KVChannelInit(chan_name, false);

    switch (msg->header.op) {
        case kv_open:
            break;
        case kv_close:
            break;
        case kv_get:
            resp = handle_kv_get(msg);
            break;
        case kv_put:
            resp = handle_kv_put(msg);
            break;
        case kv_delete:
            break;
        default:
            break;
    }
    if (resp != NULL) {
        KVChannelPushMsg(resp_chan, resp, true);
        if (resp->entity)
            pfree(resp->entity);
        pfree(resp);
    }
    if (msg->entity)
        pfree(msg->entity);
    pfree(msg);
    return NULL;
}

KVMsg *handle_kv_get(KVMsg *msg) {
    Size key_len = msg->header.entitySize;
    NRAMKey key = tkey_deserialize((char *)msg->entity, key_len);

    // execution
    NRAMValue value = rocksengine_get(&GlobalRocksEngine->engine, key);

    // prepare response
    KVMsg *resp = palloc0(sizeof(KVMsg));
    *resp = NewStatusMsg(kv_status_ok, msg->header.respChannel);
    resp->header.op = kv_get;

    Size val_len;
    resp->entity = tvalue_serialize(value, &val_len);
    resp->header.entitySize = val_len;
    resp->header.relId = key->tableOid;

    pfree(key);
    pfree(value);

    return resp;
}

KVMsg *handle_kv_put(KVMsg *msg) {
    Size total_len, key_len, value_len;
    NRAMKey key;
    NRAMValue value;

    total_len = msg->header.entitySize;
    char *buf = (char *) msg->entity;
    memcpy(&key_len, buf, sizeof(Size));
    buf += sizeof(Size);
    key = tkey_deserialize(buf, key_len);
    buf += key_len;
    value_len = total_len - key_len - sizeof(Size);
    value = tvalue_deserialize(buf, value_len);

    // execution
    rocksengine_put(&GlobalRocksEngine->engine, key, value);

    // prepare response
    KVMsg *resp = palloc0(sizeof(KVMsg));
    *resp = NewStatusMsg(kv_status_ok, msg->header.respChannel);
    resp->header.op = kv_put;
    resp->header.relId = key->tableOid;

    pfree(key);
    pfree(value);
    return resp;
}
