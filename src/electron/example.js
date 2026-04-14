const { calculateBanana } = require("./nativeBridge");

const banana = calculateBanana(10, 2);
console.log({ purchases: 10, multiplier: 2, banana });
