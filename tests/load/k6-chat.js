import http from 'k6/http';
import {check, sleep} from 'k6';

export const options = {
  stages: [
    {duration: '30s', target: 10},
    {duration: '1m', target: 10},
    {duration: '10s', target: 0},
  ],
  thresholds: {
    http_req_duration: ['p(95)<700'],
    http_req_failed: ['rate<0.02'],
  },
};

const BASE_URL = __ENV.BASE_URL || 'http://localhost:8081';

export default function () {
  const payload = JSON.stringify({message: 'hello banana'});
  const res = http.post(`${BASE_URL}/api/chat`, payload, {
    headers: {'Content-Type': 'application/json'},
  });
  check(res, {'status is healthy': (r) => r.status === 200 || r.status === 202});
  sleep(0.1);
}
