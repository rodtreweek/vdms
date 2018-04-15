# VDMS Docker Sample App: The Cancer Image Archive

VDMS server docker image to intel’s docker hub. 
The image is hub.docker.com/intellabs/vdms:hls 

You need to have a compress file with the HLS database.
(contact luis.remis@intel.com to have access to that file) 
Name it data.tar.bz2. 

The build command is: 
tar cf - data.tar.bz2 Dockerfile | docker build -t hub.docker.intel.com/athena-hls/hls:v1.0 --build-arg=http_proxy --build-arg=https_proxy  -
 
To run the docker image as a container, do this:
 
docker run --net=host -d hub.docker.intel.com/athena-hls/hls:v1.0

