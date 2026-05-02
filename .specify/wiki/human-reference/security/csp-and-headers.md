<!-- breadcrumb: Security > CSP and Security Headers -->

# CSP and Security Headers

> [Home](../Home.md) › [Security](README.md) › CSP and Security Headers

Related pages: [Threat Model](threat-model.md)

> **Status:** Stub — expand with header inventory and CSP directive explanation.

## Applied Headers

Both the ASP.NET API and the Fastify API apply a common set of security response headers:

| Header | Value | Purpose |
|--------|-------|---------|
| `X-Content-Type-Options` | `nosniff` | Prevent MIME-type sniffing |
| `X-Frame-Options` | `DENY` | Clickjacking protection |
| `Content-Security-Policy` | See below | XSS mitigation |
| `Referrer-Policy` | `no-referrer` | Prevent referrer leakage |
| `Permissions-Policy` | `camera=(), microphone=(), geolocation=()` | Disable browser APIs |

## CSP Directives (ASP.NET)

```
default-src 'self';
img-src 'self' data:;
script-src 'self';
style-src 'self' 'unsafe-inline';
connect-src 'self';
font-src 'self'
```

## Fastify (helmet)

Fastify uses `@fastify/helmet` with `crossOriginEmbedderPolicy: false` to allow
embedded content needed by the Electron runtime. CSP directives match ASP.NET above.

## Updating Headers

Changes to headers should:
1. Update both API layers in the same PR.
2. Add/update an E2E test asserting the header is present.
3. Update this page if the directive set changes.
