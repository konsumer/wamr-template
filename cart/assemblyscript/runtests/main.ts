// TODO: inline these from header, since global lib doesn't work right

export function malloc(size: usize, id: u32 = 0): usize {
   const pout = __new(size, id)
  __pin(pout)
  return pout
}

export function free(pointer: usize): void {
  __unpin(pointer)
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
declare function _test_string_out(): ArrayBuffer
function test_string_out(): String {
  return String.UTF8.decode(_test_string_out())
}

@external("null0", "test_bytes_in")
declare function _test_bytes_in(bytes:ArrayBuffer, bytesLen:i32): void
function test_bytes_in(bytes:ArrayBuffer): void {
  _test_bytes_in(bytes, bytes.byteLength)
}

@external("null0", "test_bytes_out")
declare function _test_bytes_out(bytesLenPtr:usize): ArrayBuffer
function test_bytes_out(): ArrayBuffer {
  let bytesLenPtr = malloc(sizeof<i32>())
  const r = _test_bytes_out(bytesLenPtr)
  free(bytesLenPtr)
  return r
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
