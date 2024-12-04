// TODO: inline these from header, since global lib doesn't work right

export function malloc(size: usize, id: u32 = 0): usize {
   const pout = __new(size, id)
  __pin(pout)
  return pout
}

export function free(pointer: usize): void {
  __unpin(pointer)
}

// simple util to properly copy a string from host
// it's already been lowered into memory, but this makes it work better

// utility function for getting bytes from host
export function get_host_bytes(lenPtr: usize, bytesPtr: usize): ArrayBuffer {
  const len = i32.load(lenPtr)
  const view = new ArrayBuffer(len)
  memory.copy(changetype<usize>(view), bytesPtr, len)
  return view
}

// utility function for getting string from host
function get_host_string(ptr: usize): string {
  if (!ptr) return ""

  let len = 0
  while (i32.load8_u(ptr + len) !== 0 && len < (1024*1024)) len++

  if (len < 1) return ""

  const view = new ArrayBuffer(len)
  memory.copy(changetype<usize>(view), ptr, len)
  return String.UTF8.decode(view, true)
}

class TestPoint {
  constructor(x:i32 = 0, y:i32 = 0) {
    this.x = x
    this.y = y
  }
  x: i32;
  y: i32;
}

@external("null0", "test_string_in")
declare function _test_string_in(s:ArrayBuffer): void
function test_string_in(s: string): void {
  _test_string_in(String.UTF8.encode(s, true))
}

@external("null0", "test_string_out")
declare function _test_string_out(): usize
function test_string_out(): string {
  const ptr = _test_string_out()
  return get_host_string(ptr)
}

@external("null0", "test_bytes_out")
declare function _test_bytes_out(bytesLenPtr: usize): usize
function test_bytes_out(): ArrayBuffer {
  // Create buffer for length
  const lenBuf = new ArrayBuffer(4)
  const lenPtr = changetype<usize>(lenBuf)

  // Get bytes pointer from host (host will write length to lenPtr)
  const bytesPtr = _test_bytes_out(lenPtr)

  // Read the length that the host wrote
  const bytesLen = i32.load(lenPtr)

  // Create buffer of proper size and copy data
  const view = new ArrayBuffer(bytesLen)
  memory.copy(changetype<usize>(view), bytesPtr, bytesLen)

  return view
}

@external("null0", "test_bytes_in")
declare function _test_bytes_in(bytes:ArrayBuffer, bytesLen:i32): void
function test_bytes_in(bytes:ArrayBuffer): void {
  _test_bytes_in(bytes, bytes.byteLength)
}

@external("null0", "test_struct_in")
declare function test_struct_in(point:TestPoint): void

@external("null0", "test_struct_out")
declare function test_struct_out(): TestPoint


export function load():void {
  test_string_in("cart(as): hi")
  trace("cart (as): test_string_out - " + test_string_out())

  const a = new Uint8Array(4)
  a[0]=2
  a[1]=4
  a[2]=6
  a[3]=8
  test_bytes_in(a.buffer)

  const b = Uint8Array.wrap(test_bytes_out())
  trace("cart(as): test_bytes_out - " + b[0].toString() + "," + b[1].toString() + "," +b[2].toString() + "," +b[3].toString())

  const point = new TestPoint(123, 456)
  test_struct_in(point)

  const r = test_struct_out()
  trace("cart(as): test_struct_out: " + r.x.toString() + "x" + r.y.toString())
}
