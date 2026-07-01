FROM quay.io/keycloak/keycloak:26.0 AS builder

ENV KC_DB=postgres
ENV KC_HEALTH_ENABLED=true
ENV KC_METRICS_ENABLED=true

# Keep one tracked bootstrap realm template in-repo for repeatable setup.
COPY src/typescript/keycloak/realm/banana-realm.template.json /opt/keycloak/data/import/banana-realm.json

RUN /opt/keycloak/bin/kc.sh build

FROM quay.io/keycloak/keycloak:26.0

ENV KC_DB=postgres
ENV KC_HEALTH_ENABLED=true
ENV KC_METRICS_ENABLED=true
ENV KC_HTTP_ENABLED=true
ENV KC_PROXY_HEADERS=xforwarded
ENV KC_HOSTNAME_STRICT=false

COPY --from=builder /opt/keycloak/ /opt/keycloak/

ENTRYPOINT ["/opt/keycloak/bin/kc.sh"]
CMD ["start", "--optimized", "--import-realm", "--http-enabled=true", "--proxy-headers=xforwarded"]
