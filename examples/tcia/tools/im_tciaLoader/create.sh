for i in 1
do
	rm -r hls_graph_$i
	mkdir hls_graph_$i
	mkdir hls_graph_$i/images/
	mkdir hls_graph_$i/images/pngs
	cp -r ../tciaLoader/tcia_pmgd_$i hls_graph_$i/hls-graph
	vdms -cfg config-vdms-$i.json > screen.log 2> log.log &
	python loadImages.py $i
	pkill vdms

	rm comp_hls_graph_$i.tgz
	bsdtar cvfz comp_hls_graph_$i.tgz hls_graph_$i
	rm -r hls_graph_$i
done
