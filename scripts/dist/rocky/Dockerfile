# syntax=docker/dockerfile:1

FROM rockylinux:9
RUN /usr/sbin/useradd makerpm
RUN yum -y install gcc-c++ git-all make readline-devel rpmdevtools wget
RUN mkdir -p /storage/git
ADD naken_asm.spec.templ /home/makerpm/
ADD build.sh /home/makerpm/
RUN chmod 777 /storage/git

