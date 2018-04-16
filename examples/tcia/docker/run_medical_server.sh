#!/bin/bash
export PATH=$PATH:/vdms
export LD_LIBRARY_PATH=/pmgd/lib:$(find /usr/local/lib/ / -type f -name "*.so" | xargs dirname | sort | uniq | tr "\n" ":")
echo "Building database... "
tar -xzSf /data.tgz
echo "Database built!"
mv hls-config-1.json config-vdms.json
../../vdms 2> log.log
