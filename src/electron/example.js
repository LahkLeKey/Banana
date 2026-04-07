const { calculatePoints } = require("./nativeBridge");

const points = calculatePoints(10, 2);
console.log({ purchases: 10, multiplier: 2, points });
