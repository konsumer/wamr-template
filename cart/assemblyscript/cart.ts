@external("null0", "abort")
declare function _host_abort(msg: ArrayBuffer, file: ArrayBuffer, line: i32, column: i32): void
export function _cart_abort(msg: string, file: string, line: i32, column: i32): void {
  _host_abort(String.UTF8.encode(msg, true),  String.UTF8.encode(file, true), line, column);
}

@external("null0", "trace")
declare function _host_trace(msg: ArrayBuffer): void
export function _cart_trace(msg: string): void {
  _host_trace(String.UTF8.encode(msg, true));
}
