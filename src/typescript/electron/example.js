const { calculateBanana } = require("./nativeBridge");

const purchases = 10;
const multiplier = 2;

try {
	const banana = calculateBanana(purchases, multiplier);
	console.log({ purchases, multiplier, banana });
} catch (error) {
	console.error("native calculation failed", {
		message: error.message,
		status: error.status,
	});
	process.exitCode = 1;
}
