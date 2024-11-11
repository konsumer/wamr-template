import { WasiPreview1 } from 'easywasi'

export default async function setupWasm(url, options = {}) {
  const { args = [], env = {}, runStart = true, fs } = options
  const wasi_snapshot_preview1 = new WasiPreview1({
    fs,
    args,
    env
  })
  const {
    instance: { exports }
  } = await WebAssembly.instantiateStreaming(fetch(url), {
    wasi_snapshot_preview1
  })
  let exitCode = 0
  if (runStart) {
    exitCode = wasi_snapshot_preview1.start(exports)
  }
  return { exports, wasi_snapshot_preview1, exitCode }
}
