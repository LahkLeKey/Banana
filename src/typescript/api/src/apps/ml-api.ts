import {createServer} from '../server/createServer.js';
import {loadRuntimeConfig} from '../shared/config.js';

const config = loadRuntimeConfig();
const app = createServer(config, {
  banana: false,
  batch: false,
  ripeness: false,
  ml: true,
});

try {
  await app.listen({
    host: config.host,
    port: config.mlPort,
  });
} catch (error) {
  app.log.error(error);
  process.exit(1);
}
