import { readFile } from 'fs/promises'
import YAML from 'yaml'
import { glob } from 'glob'

const types = YAML.parse(await readFile('api/types.yml', 'utf8'))

const out = [`// Host interface exposed to WAMR and Emscripten

#pragma once

`]

const typeMap = {
  void: 'void',
  u32: 'unsigned int',
  u8: 'unsigned char',
  TestPoint: 'unsigned int',
  string: 'unsigned int',
  'u8[]': 'unsigned int',
  'u32': 'unsigned int',
  'u32*': 'unsigned int',
}

function generateBody(func) {
  // TODO
  return ''
}

for (const f of await glob('api/*.yml')) {
  if (f === 'api/types.yml') continue
  const api = YAML.parse(await readFile(f, 'utf8'))
  const apiName = f.split('/').pop().split('.')[0]
  out.push(`//// ${apiName.toUpperCase()}\n`)
  for (const [fname, func] of Object.entries(api)) {
    out.push(`// ${func.description}`)
    const as = Object.keys(func.arguments).map(a => `${typeMap[func.arguments[a]]} ${a}`).join(', ')
    out.push(`HOST_FUNCTION(${typeMap[func.returns]}, ${fname}, (${as}), {`)
    out.push(generateBody(func))
    out.push('})\n')
  }
}

console.log(out.join('\n'))
