import http from 'k6/http';
import {check, sleep} from 'k6';

export const options = {
  stages: [
    {duration: '30s', target: 20},
    {duration: '1m', target: 20},
    {duration: '10s', target: 0},
  ],
  thresholds: {
    http_req_duration: ['p(95)<500'],
    http_req_failed: ['rate<0.01'],
  },
};

const BASE_URL = __ENV.BASE_URL || 'http://localhost:8080';

export default function () {
  const res = http.post(`${BASE_URL}/api/banana`, JSON.stringify({text: 'banana test input'}), {
    headers: {'Content-Type': 'application/json'},
  });
  check(res, {'status 200': (r) => r.status === 200});
  sleep(0.1);
}
