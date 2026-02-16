FROM rockylinux/rockylinux:8-minimal AS main

FROM main AS user-creation

RUN microdnf install --nodocs -y shadow-utils && \
    groupadd -g 1000 quantreplay-user && \
    useradd -M -u 1000 -g quantreplay-user quantreplay-user

FROM main

COPY --from=user-creation /etc/passwd /etc/shadow /etc/group /etc/gshadow /etc

# Set environment variables
ENV LOG_DIR=/market-simulator/quod/data/log

RUN mkdir -p /market-simulator/quod/{bin,data/cfg}
COPY --chown=quantreplay-user:quantreplay-user --chmod=754 package/quod/bin /market-simulator/quod/bin

RUN mkdir /template
COPY --chown=quantreplay-user:quantreplay-user template/ /template/

COPY --chown=quantreplay-user:quantreplay-user --chmod=754 script/entrypoint_os.sh /entrypoint_os.sh

USER quantreplay-user

WORKDIR /market-simulator/quod

ENTRYPOINT ["/entrypoint_os.sh"]
