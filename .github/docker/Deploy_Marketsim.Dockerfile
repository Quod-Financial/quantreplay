FROM rockylinux:8

RUN dnf --refresh makecache
RUN dnf install -y gcc-toolset-12

RUN groupadd -g 1000 quantreplay-user && \
    useradd -m -u 1000 -g quantreplay-user quantreplay-user

# Set environment variables
ENV LOG_DIR=/market-simulator/quod/data/log
ENV LD_LIBRARY_PATH="/opt/rh/gcc-toolset-12/root/usr/lib64:/opt/rh/gcc-toolset-12/root/usr/lib:${LD_LIBRARY_PATH}"

RUN mkdir -p /market-simulator/quod/data/cfg
RUN mkdir /template
COPY template/ /template/

COPY package/quod /market-simulator/quod

RUN chmod +x /market-simulator/quod/bin/marketsimulator

# Remove specific files from the cfg folder
RUN rm -f /market-simulator/quod/cfg/market_simulator.xml \
    && rm -f /market-simulator/quod/cfg/configSim.txt

COPY script/entrypoint_os.sh /entrypoint_os.sh
RUN chmod +x /entrypoint_os.sh

RUN chown -R quantreplay-user:quantreplay-user /market-simulator /entrypoint_os.sh

USER quantreplay-user

WORKDIR /market-simulator/quod

ENTRYPOINT ["/entrypoint_os.sh"]
