let memory = {}; 

// 定义日志函数
function logWrapper(ptr, len) {
  const bytes = new Uint8Array(memory.buffer, ptr, len);
  const msg = new TextDecoder('utf-8').decode(bytes);
  console.log(msg);
}

function errorWrapper(ptr, len) {
  const bytes = new Uint8Array(memory.buffer, ptr, len);
  const msg = new TextDecoder('utf-8').decode(bytes);
  console.error(msg);
}

const imports = {
  env: {
    // // 初始化内存，假设需要256页（每页64KB）
    // memory: memory,  
    // // 初始化表，假设初始大小为1，可以根据需要调整
    // table: new WXWebAssembly.Table({ initial: 256, element: 'anyfunc' }),
    log_wrapper: logWrapper,
    error_wrapper: errorWrapper,
  },
};

const instance = WXWebAssembly.instantiate("wasm/gameplay.wasm", imports).then(
  (result) => {
    console.log("初始化成功");
    const wasmInstance = result.instance;
    const wasmExports = wasmInstance.exports;
    console.log("Wasm exports:", wasmInstance.exports);
    memory = wasmExports.memory;

    const {q_add, start} = wasmExports;

    let sum = q_add(11, 22);
    console.log("q_add result:", sum);

    start();
  },
  (err) => {
    console.log("初始化失败", err);
  }
);