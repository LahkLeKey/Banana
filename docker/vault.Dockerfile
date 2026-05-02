FROM hashicorp/vault:1.16
ENV VAULT_DEV_ROOT_TOKEN_ID=banana-dev-token
ENV VAULT_DEV_LISTEN_ADDRESS=0.0.0.0:8200
EXPOSE 8200
CMD ["vault", "server", "-dev"]
