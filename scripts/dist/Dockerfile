
FROM centos:7
RUN /usr/sbin/useradd makerpm
RUN yum -y install gcc-c++ git-all make readline-devel rpmdevtools wget
RUN mkdir -p /storage/git
RUN cd /storage/git && git clone https://github.com/mikeakohn/naken_asm.git
RUN cd /storage/git && git clone https://github.com/mikeakohn/java_grinder.git

