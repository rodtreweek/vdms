cd ~/vcs/vdms
./vdms -cfg hls-config-1.json > remove.log &
cd ../apps/hls/pyClient/
python poc.py
sudo pkill vdms

cd ~/vcs/vdms
./vdms -cfg hls-config-10.json > remove.log &
cd ../apps/hls/pyClient/
sleep 1
python poc.py
sudo pkill vdms
sleep 2

cd ~/vcs/vdms
./vdms -cfg hls-config-100.json > remove.log &
cd ../apps/hls/pyClient/
sleep 1
python poc.py
sudo pkill vdms
