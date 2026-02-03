# jepsen.postgres

Jepsen tests for the NeurDB database system.

## Usage

To check NeurDB `SERIALIZABLE`, start a postgres process on localhost, with a user (and
database) named `neurdb`, try:

```
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
```

Currently, only support single node tests.


## License

Copyright © 2020, 2024 Jepsen, LLC

This program and the accompanying materials are made available under the
terms of the Eclipse Public License 2.0 which is available at
http://www.eclipse.org/legal/epl-2.0.

This Source Code may also be made available under the following Secondary
Licenses when the conditions for such availability set forth in the Eclipse
Public License, v. 2.0 are satisfied: GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or (at your
option) any later version, with the GNU Classpath Exception which is available
at https://www.gnu.org/software/classpath/license.html.
