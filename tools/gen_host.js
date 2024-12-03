// this will stub host, based on your api yaml files
import { readFile } from 'fs/promises'
import YAML from 'yaml'
import { glob } from 'glob'

const types = YAML.parse(await readFile('api/types.yml', 'utf8'))

const out = [`// Host interface exposed to WAMR and Emscripten
#pragma once

`]

// map param/return types to C types
function typeMap(t) {
  if (t === 'void') return 'void'
  if (t === 'string' || t.endsWith('[]') || t[0].toUpperCase() === t[0]) return 'u32'
  return t  // use the type directly from yaml (u8, u32, etc)
}

function generateBody(fname, func) {
  const lines = []
  const params = []

  // Handle input parameters
  for (const [name, type] of Object.entries(func.arguments)) {
    if (type === 'string') {
      lines.push(`  char* ${name} = copy_from_cart_string(${name}Ptr);`)
      params.push(name)
    } else if (type.endsWith('[]')) {
      lines.push(`  u8* ${name} = copy_from_cart(${name}Ptr, ${name}Len);`)
      // Only add length param once
      params.push(name, `${name}Len`)
    } else if (type[0].toUpperCase() === type[0]) {
      lines.push(`  ${type}* ${name} = copy_from_cart(${name}Ptr, sizeof(${type}));`)
      params.push(name)
    } else if (type.endsWith('*')) {
      params.push(name + 'Ptr')
    } else {
      params.push(name)
    }
  }

  // Filter out duplicate length parameters from params array
  const uniqueParams = [...new Set(params)]

  // Handle return value setup
  if (func.returns === 'string') {
    lines.push(`  // implement ${fname}(${uniqueParams.join(', ')}) here`)
    lines.push(`  char* s = "";`)
    lines.push(`  return copy_to_cart_string(s);`)
  } else if (func.returns.endsWith('[]')) {
    lines.push(`  u32 outLen = 0;`)
    lines.push(`  // implement ${func.returns.replace('[]', '')}* returnVal=${fname}(&outLen) here`)
    lines.push(`  ${func.returns.replace('[]', '')}* returnVal = {};`)
    lines.push(`  copy_to_cart_with_pointer(outLenPtr, &outLen, sizeof(outLen));`)
    lines.push(`  return copy_to_cart(returnVal, outLen);`)
  } else if (func.returns[0].toUpperCase() === func.returns[0]) {
    lines.push(`  // implement ${fname}(${params.join(', ')}) here`)
    lines.push(`  ${func.returns} result = {};`)
    lines.push(`  return copy_to_cart(&result, sizeof(result));`)
  } else if (func.returns === 'void') {
    lines.push(`  // implement ${fname}(${uniqueParams.join(', ')}) here`)
  } else {
    lines.push(`  // implement ${fname}(${uniqueParams.join(', ')}) here`)
    lines.push(`  return 0;`)
  }

  return lines.join('\n')
}

// Output struct definitions
for (const [tname, type] of Object.entries(types)) {
  out.push(`// ${type.description}`)
  out.push(`typedef struct {`)
  for (const [pname, ptype] of Object.entries(type.properties)) {
    out.push(`  ${typeMap(ptype)} ${pname};`)
  }
  out.push(`} ${tname};\n`)
}

// Output function definitions
for (const f of await glob('api/*.yml')) {
  if (f === 'api/types.yml') continue
  const api = YAML.parse(await readFile(f, 'utf8'))
  const apiName = f.split('/').pop().split('.')[0]
  out.push(`// ${apiName.toUpperCase()} API\n`)

  for (const [fname, func] of Object.entries(api)) {
    out.push(`// ${func.description}`)

    // Convert arguments to include Ptr for pointer types
    const args = Object.entries(func.arguments).map(([name, type]) => {
      if (type === 'string') {
        return `${typeMap(type)} ${name}Ptr`
      }
      if (type.endsWith('[]')) {
        // Don't add length parameter here if it's already in the arguments
        const hasLengthParam = Object.entries(func.arguments).some(([n, t]) => n === `${name}Len`)
        const parts = [`${typeMap(type)} ${name}Ptr`]
        if (!hasLengthParam) parts.push(`${typeMap('u32')} ${name}Len`)
        return parts.join(', ')
      }
      if (type[0].toUpperCase() === type[0]) {
        return `${typeMap(type)} ${name}Ptr`
      }
      return `${typeMap(type)} ${name}${type.endsWith('*') ? 'Ptr' : ''}`
    }).join(', ')

    const returnType = typeMap(func.returns)
    out.push(`HOST_FUNCTION(${returnType}, ${fname}, (${args}), {`)
    out.push(generateBody(fname, func))
    out.push('})\n')
  }
}

console.log(out.join('\n'))
