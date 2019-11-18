
FROM ubuntu
RUN apt update
RUN apt -y install g++ git make libreadline-dev wget snapcraft snapd autoconf automake autopoint autotools-dev libltdl-dev libltdl7 libsigsegv2 libtool m4
RUN mkdir -p /storage/git
ADD build.sh /root
RUN chmod 777 /storage/git

