const buffer = Buffer.alloc(8214);
const data = new Uint8Array(buffer);
console.log("data.length:", data.length);
const heights = data.slice(20, 4116);
console.log("heights.length after slice(20, 4116):", heights.length);
