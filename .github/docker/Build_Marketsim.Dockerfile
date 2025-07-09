FROM rockylinux:8

RUN dnf --refresh makecache

RUN dnf install -y \
    gcc-toolset-12 \
    git \
    make \
    openssl-devel \
    perl \
    python3-pip

RUN pip3 install --upgrade pip

RUN pip3 install cmake==3.21 conan==1.59 gcovr

# Hardcoding env variables to allow a container to use gcc 12,
# located in /opt/rh/gcc-toolset-12, without additional scripting
ENV PATH="/opt/rh/gcc-toolset-12/root/usr/bin:${PATH}"
ENV LD_LIBRARY_PATH="/opt/rh/gcc-toolset-12/root/usr/lib64:/opt/rh/gcc-toolset-12/root/usr/lib:${LD_LIBRARY_PATH}"