// import helloAddon from './build/Release/hello.node';


import { createRequire } from 'node:module';
const require = createRequire(import.meta.url);

const helloAddon = require('./graphLayout');

console.log(helloAddon.hello()); // 'world'

helloAddon.logObject({"s": "s", "5": 5, 6: 7})

const input = {
  foo: "bar",
  num: 42,
  nested: { a: "x" },
  arr: [{ x: "y" }]
};

console.log("Input:", JSON.stringify(input));
const transformed = helloAddon.transformObject(input);
console.log("Output:", JSON.stringify(transformed));

console.log("\nGraphLayout tests:");
console.log("graphLayout({}):", helloAddon.graphLayout({}));        // true
console.log("graphLayout('x'):", helloAddon.graphLayout("x"));     // undefined
console.log("graphLayout(42):", helloAddon.graphLayout(42));        // undefined
console.log("graphLayout():", helloAddon.graphLayout());           // undefined