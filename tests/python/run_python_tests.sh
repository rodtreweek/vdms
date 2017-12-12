rm log.log screen.log dump.log
rm -r test-graph
rm -r native_format
mkdir native_format
mkdir native_format/pngs
mkdir native_format/jpgs
mkdir native_format/descriptors

../../athena -cfg config-tests.json > screen.log &
python main.py -v

sleep 1
pkill athena

dumpg test-graph > dump.log
