FROM rockylinux:8

RUN dnf install -y epel-release && \
    dnf makecache && \
    dnf module reset ruby -y && \
    dnf install -y \
        bison \
        cairo-gobject-devel \
        ccache \
        cmake \
        flex \
        gcc-toolset-12 \
        gdb \
        gdk-pixbuf2-devel \
        git \
        gobject-introspection-devel \
        libxml2-devel \
        make \
        openssl-devel \
        pango-devel \
        perl \
        python3.11-pip \
        @ruby:3.3 \
        ruby-devel

RUN pip3 install --upgrade pip && \
    pip3 install conan==1.59

ENV PATH="/opt/rh/gcc-toolset-12/root/usr/bin:${PATH}"
ENV LD_LIBRARY_PATH="/opt/rh/gcc-toolset-12/root/usr/lib64:/opt/rh/gcc-toolset-12/root/usr/lib:${LD_LIBRARY_PATH}"

RUN gem install \
        asciidoctor \
        asciidoctor-mathematical \
        asciidoctor-pdf \
        rouge

ARG UID=1000
ARG USERNAME=quantreplay-user
RUN useradd -m -u $UID -s /bin/bash $USERNAME

# Persist bash history
RUN SNIPPET="export PROMPT_COMMAND='history -a' && export HISTFILE=/commandhistory/.bash_history" \
    && mkdir /commandhistory \
    && touch /commandhistory/.bash_history \
    && chown -R $USERNAME /commandhistory \
    && echo "$SNIPPET" >> "/home/$USERNAME/.bashrc"

USER $USERNAME
