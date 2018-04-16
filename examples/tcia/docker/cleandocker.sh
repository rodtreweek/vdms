docker rm -f $(docker ps -a -q)
docker rmi -f $(docker images -a -q)

# If things still bad, things get real:
sudo service docker stop
sudo rm -rf /var/lib/docker/aufs/*
sudo rm -f /var/lib/docker/linkgraph.db
sudo service docker start
