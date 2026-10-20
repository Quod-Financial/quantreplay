FROM postgres:16.15

COPY init.sql /docker-entrypoint-initdb.d/init.sql