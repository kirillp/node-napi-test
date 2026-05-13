// import helloAddon from './build/Release/hello.node';


import { createRequire } from 'node:module';
const require = createRequire(import.meta.url);

const helloAddon = require('./graphLayout');

console.log(helloAddon.hello()); // 'world'

helloAddon.logObject({"s": "s", "5": 5, 6: 7})