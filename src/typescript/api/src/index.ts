// Banana Fastify API (spec 008). Owns TypeScript-only routes; ASP.NET (007)
// owns the legacy banana/batch/ripeness/not-banana surface. Per-route
// ownership table:
// .specify/specs/008-typescript-api/contracts/route-ownership.md
import Fastify from 'fastify';

import {registerNotBananaRoutes} from './domains/not-banana/routes.ts';
import {registerCorpusRoutes} from './routes/corpus.ts';
import {registerHealthRoutes} from './routes/health.ts';

const app = Fastify({logger: true});

await registerHealthRoutes(app);
await registerCorpusRoutes(app);
await registerNotBananaRoutes(app);

const port = Number(process.env.PORT ?? 8081);
const host = process.env.HOST ?? '0.0.0.0';

app.listen({port, host}).catch((err) => {
  app.log.error(err);
  process.exit(1);
});
