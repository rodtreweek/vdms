sync; sudo sysctl -w vm.drop_caches=3
rm log.log
rm -rf hls_graph_1
tar xzf tools/im_tciaLoader/comp_hls_graph_1.tgz
# vdms -cfg hls-config-1.json 2> log.log
