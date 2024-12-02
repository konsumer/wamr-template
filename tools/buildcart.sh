#!/bin/bash

# This will build a null0 cart

LOC_CART="${1}"
LOC_WASM="${2}"
LOC_ASSETS="${3}"
LOC_OUT="$(mktemp -d)"

echo "LOC_CART: ${LOC_CART}"
echo "LOC_WASM: ${LOC_WASM}"
echo "LOC_OUT: ${LOC_OUT}"
echo "LOC_ASSETS: ${LOC_ASSETS}"

cp -R ${LOC_ASSETS}/* "${LOC_OUT}"
cp "${LOC_WASM}" "${LOC_OUT}/main.wasm"
cd "${LOC_OUT}"
zip -r "${LOC_CART}" . -x *.c -x *.h -x .* -x *.ts
rm -r "${LOC_OUT}"
