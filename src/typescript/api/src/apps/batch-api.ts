import {createServer} from '../server/createServer.js';
import {loadRuntimeConfig} from '../shared/config.js';

const config = loadRuntimeConfig();
const app = createServer(config, {
  banana: false,
  batch: true,
  ripeness: false,
  ml: false,
});

try {
  await app.listen({
    host: config.host,
    port: config.batchPort,
  });
} catch (error) {
  app.log.error(error);
  process.exit(1);
}
