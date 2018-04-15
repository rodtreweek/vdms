DB_PATH=tcia_pmgd_

scons -c
scons -j12

for i in 1 10
do
    rm -r $DB_PATH$i
    ./tciadbCreator $DB_PATH$i $i
done
