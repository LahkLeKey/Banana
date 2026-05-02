# Architecture

> [Home](../Home.md) › Architecture

Design decisions, system diagrams, and domain model documentation.

## Pages In This Section

| Page | Description |
|------|-------------|
| [Overview](overview.md) | System architecture diagrams and HTTP route flows |
| [Native Wrapper ABI](native-wrapper-abi.md) | Why we use a wrapper and how it works |
| [Database Pipeline Stage](database-pipeline-stage.md) | How the `DatabaseAccessStep` works |
| [Domain Model](domain-model.md) | Native core domain areas and aggregate boundaries |

## Core Principle

> `controller → service → pipeline → native interop`

Native behavior lives in C. The wrapper ABI is the only contract managed code touches.
