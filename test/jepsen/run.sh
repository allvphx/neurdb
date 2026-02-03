psql
lein run test \
  -w append \
  --existing-postgres \
  --nodes localhost \
  --nemesis none \
  --concurrency 4 \
  -r 50 \
  -v 0.16.3 \
  --time-limit 30 \
  --isolation serializable \
  --postgres-user neurdb \
  --max-writes-per-key 4 \
  |& tee ./test.log
