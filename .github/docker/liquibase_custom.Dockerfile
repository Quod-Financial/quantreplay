FROM liquibase/liquibase

COPY db /liquibase/project/

ENV SIMULATOR_DATABASE_HOST="market-simulator-database"
ENV SIMULATOR_DATABASE_PORT="5432"
ENV SIMULATOR_DATABASE_DBNAME="simdb"
ENV SIMULATOR_DATABASE_APP_USER="sim"
ENV SIMULATOR_DATABASE_APP_PASSWORD="sim"
ENV SIMULATOR_LIQUIBASE_CONTEXT="market-simulator,schema,demo"
ENV CHANGELOG_FILE="root-changelog.yml"

ENTRYPOINT sh -c "liquibase \
    --url=jdbc:postgresql://${SIMULATOR_DATABASE_HOST}:${SIMULATOR_DATABASE_PORT}/${SIMULATOR_DATABASE_DBNAME} \
    --username=${SIMULATOR_DATABASE_APP_USER} \
    --password=${SIMULATOR_DATABASE_APP_PASSWORD} \
    --classpath=/liquibase/project \
    --changelog-file=${CHANGELOG_FILE} \
    --contexts=${SIMULATOR_LIQUIBASE_CONTEXT} \
    update"


