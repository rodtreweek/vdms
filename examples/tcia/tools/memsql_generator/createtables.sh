scons -c
scons -j12

for i in "../tciaLoader/tcia_pmgd_1" "../tciaLoader/tcia_pmgd_10"
         #"../tciaLoader/tcia_pmgd_100" "../tciaLoader/tcia_pmgd_1000"
do
    ./tcia_memsql_gen $i
done