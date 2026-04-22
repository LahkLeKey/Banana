import {createServer} from '../server/createServer.js';
import {loadRuntimeConfig} from '../shared/config.js';

const config = loadRuntimeConfig();
const app = createServer(config, {
  banana: true,
  batch: false,
  ripeness: false,
  ml: false,
  notBanana: false,
});

try {
  await app.listen({
    host: config.host,
    port: config.bananaPort,
  });
} catch (error) {
  app.log.error(error);
  process.exit(1);
}
