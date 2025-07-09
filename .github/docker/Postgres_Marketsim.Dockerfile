FROM postgres:13.4

COPY init.sql /docker-entrypoint-initdb.d/init.sql