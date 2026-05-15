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

assert(graphLayout.graphLayout({}) === undefined, 'graphLayout({}) should return undefined');

{
  const input = {
    id: "root",
    children: [{
      height: 100,
      id: "child1",
      width: 200,
      layoutOptions: {
        "org.eclipse.elk.nodeLabels.placement": "inside",
        "org.eclipse.elk.padding": "[5,5,5,5]",
        "org.eclipse.elk.portConstraints": "FIXED_SIDE"
      },
      ports: [{
        height: 10,
        id: "port1",
        width: 10,
        layoutOptions: {
          "org.eclipse.elk.port.borderOffset": "2",
          "org.eclipse.elk.port.side": "EAST"
        }
      }]
    }],
    edges: [{
      id: "edge1",
      sources: ["child1"],
      targets: ["child2"]
    }],
    layoutOptions: {
      "PyPTO.filePath": "/path",
      "PyPTO.renderMode": "svg",
      "PyPTO.showTensor": "1",
      "elk.direction": "DOWN",
      "org.eclipse.elk.algorithm": "layered",
      "org.eclipse.elk.layered.nodePlacement.strategy": "SIMPLE",
      "org.eclipse.elk.layered.spacing.nodeNodeBetweenLayers": "20"
    }
  };
  console.log("ElkLayout data:", JSON.stringify(input));
  const t = graphLayout.graphLayout(input);
  assert(t === true, 'graphLayout(full input) should return true');
}
assert(graphLayout.graphLayout("x") === undefined, 'graphLayout("x") should return undefined');
assert(graphLayout.graphLayout(42) === undefined, 'graphLayout(42) should return undefined');
assert(graphLayout.graphLayout() === undefined, 'graphLayout() should return undefined');

console.log("\nAll tests passed.");