import {createServer} from '../server/createServer.js';
import {loadRuntimeConfig} from '../shared/config.js';

const config = loadRuntimeConfig();
const app = createServer(config, {
  banana: true,
  batch: true,
  ripeness: true,
  ml: true,
});

try {
  await app.listen({
    host: config.host,
    port: config.gatewayPort,
  });
} catch (error) {
  app.log.error(error);
  process.exit(1);
}
