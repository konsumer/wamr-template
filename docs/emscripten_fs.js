// Simple wrapper to share fs between easywasi and emscripten
// -sFORCE_FILESYSTEM -sEXTRA_EXPORTED_RUNTIME_METHODS=FS

export default class EmscriptenFS {
  constructor(FS) {
    // you can set this later, too
    this.FS = FS
  }

  // TODO: these should be implemented?

  appendFileSync (path, data, options = {}) {
    throw new Error('appendFileSync not implemented. This is a dummy fs.')
  }

  fsyncSync (fd) {
    throw new Error('fsyncSync not implemented. This is a dummy fs.')
  }

  setFlagsSync (path, flags) {
    throw new Error('setFlagsSync not implemented. This is a dummy fs.')
  }



  linkSync (existingPath, newPath) {
    return this.FS.symlink(existingPath, newPath)
  }

  mkdirSync (path, options = {}) {
    const mkdir = options.recursive ? this.FS.mkdirTree : this.FS.mkdir
    return mkdir(path)
  }

  readdirSync (path, options = {}) {
    return this.FS.readdir(path)
  }

  readFileSync (path, options = {}) {
    return this.FS.readFile(path, options)
  }

  readlinkSync (path, options = {}) {
    return this.FS.readlink(path)
  }

  renameSync (oldPath, newPath) {
    return this.FS.rename(oldPath, newPath)
  }

  rmdirSync (path, options = {}) {
    return this.FS.rmdir(path)
  }

  statSync (path, options = {}) {
    return this.FS.stat(path)
  }

  symlinkSync (target, path, type = 'file') {
    return this.FS.symlink(existingPath, newPath)
  }

  truncateSync (path, len = 0) {
    return this.FS.truncate(path, len)
  }

  unlinkSync (path) {
    return this.FS.unlink(path)
  }

  utimesSync (path, atime, mtime) {
    return this.FS.utime(path, atime, mtime)
  }

  writeFileSync (path, data, options = {}) {
    return this.FS.writeFile(path, data, options)
  }
}
