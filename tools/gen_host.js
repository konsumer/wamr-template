// this will stub host, based on your api yaml files

import { readFile } from 'fs/promises'
import YAML from 'yaml'
import { glob } from 'glob'

const types = YAML.parse(await readFile('api/types.yml', 'utf8'))

const out = [`// Host interface exposed to WAMR and Emscripten

#pragma once

`]


// map param/return tyypes to C types
function typeMap(t) {
  if (t ==='void'){
    return 'void'
  }
  if (t == 'u8') {
    return 'uint8_t'
  }
  if (t == 'u16') {
    return 'uint16_t'
  }
  if (t == 'i8') {
    return 'int8_t'
  }
  if (t == 'i16') {
    return 'int16_t'
  }
  if (t == 'i32') {
    return 'int32_t'
  }
  if (t == 'f32') {
    return 'float'
  }
  if (t == 'f64') {
    return 'double'
  }
  if (t == 'u64') {
    return 'uint64_t'
  }
  if (t == 'i64') {
    return 'int64_t'
  }
  // most things are pointers or uint32
  return 'uint32_t'
}

// add Pntr to non-value args
const argName = (name, type) => (type[0].toUpperCase() == type[0] || type === 'string' || type.endsWith('*') || type.endsWith('[]')) ? `${name}Ptr` : name

// generate 1 line for pulling arg into named var
function pullMap(name, type, nextArg) {
  if (type.endsWith('[]')) {
    const t = type.replace('[]', '')
    return `  ${t}* ${name} = (${t}*)copy_from_cart(${name}Ptr, sizeof(${t}) * ${nextArg});`
  }
}

// generate the functionb-body parts (convert in/out)
function generateBody(fname, func) {
  const start = []
  const end = []

  let skipNext = false
  let n = 0
  const args = Object.keys(func.arguments)
  for (const [name, type] of Object.entries(func.arguments)) {
    n++

    if (skipNext) {
      skipNext = false
      continue
    }
    if (type.endsWith('[]')) {
      skipNext = true
      start.push(pullMap(name, type, args[n]))
    } else if (type === 'string') {
      start.push(`  char* ${name} = copy_from_cart_string(${name}Ptr);`)
    }

  }

  return [...start, `  // call ${fname}(${Object.keys(func.arguments).join(', ')}) here`, ...end].join('\n')
}

for (const [tname, type] of Object.entries(types)) {
  out.push(`// ${type.description}`)
  out.push(`typedef struct {`)
  for (const [pname, ptype] of Object.entries(type.properties)) {
    out.push(`  ${typeMap(ptype)} ${pname};`)
  }
  out.push(`} ${tname};\n`)
}

for (const f of await glob('api/*.yml')) {
  if (f === 'api/types.yml') continue
  const api = YAML.parse(await readFile(f, 'utf8'))
  const apiName = f.split('/').pop().split('.')[0]
  out.push(`//// ${apiName.toUpperCase()}\n`)
  for (const [fname, func] of Object.entries(api)) {
    out.push(`// ${func.description}`)
    const as = Object.keys(func.arguments).map(a => `${func.arguments[a]} ${argName(a, func.arguments[a])}`).join(', ')
    out.push(`HOST_FUNCTION(${func.returns}, ${fname}, (${as}), {`)
    out.push(generateBody(fname, func))
    out.push('})\n')
  }
}

console.log(out.join('\n'))
