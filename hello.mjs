import { createRequire } from 'node:module';
const require = createRequire(import.meta.url);

const graphLayout = require('./graphLayout');

const assert = (ok, msg) => { if (!ok) throw new Error(msg); };

assert(graphLayout.hello() === 'world', 'hello() should return "world"');

graphLayout.logObject({ "s": "s", "5": 5, 6: 7 });

{
  const input = {
    foo: "bar",
    num: 42,
    nested: { a: "x" },
    arr: [{ x: "y" }]
  };
  console.log("Input:", JSON.stringify(input));
  const t = graphLayout.transformObject(input);
  console.log("Output:", JSON.stringify(t));
  assert(t.bar === 'foo', 'transformObject: expected t.bar === "foo"');
  assert(t[42] === 'num', 'transformObject: expected t[42] === "num"');
  assert(t.nested.x === 'a', 'transformObject: expected t.nested.x === "a"');
  assert(t.arr[0].y === 'x', 'transformObject: expected t.arr[0].y === "x"');
}

assert(graphLayout.graphLayout({}) === true, 'graphLayout({}) should return true');
assert(graphLayout.graphLayout("x") === undefined, 'graphLayout("x") should return undefined');
assert(graphLayout.graphLayout(42) === undefined, 'graphLayout(42) should return undefined');
assert(graphLayout.graphLayout() === undefined, 'graphLayout() should return undefined');

console.log("\nAll tests passed.");