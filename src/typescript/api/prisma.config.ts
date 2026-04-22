import {defineConfig} from 'prisma/config';

const defaultDatabaseUrl =
    'postgresql://cinterop:cinterop@localhost:5432/cinterop';

export default defineConfig({
  schema: 'prisma/schema.prisma',
  migrations: {
    path: 'prisma/migrations',
  },
  datasource: {
    url: process.env.DATABASE_URL ?? defaultDatabaseUrl,
  },
});
