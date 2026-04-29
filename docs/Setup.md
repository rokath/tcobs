# TCOBS Setup (Windows, macOS, Linux)

This repository contains CGO-based code paths.  
If a C compiler is missing or not visible in `PATH`, tools like `go list`/`gopls` and commands like `go test ./...` can fail.

## 1) Required versions

- Go: `1.19+` (see `go.mod`)
- C compiler toolchain:
  - Windows: `gcc` from MSYS2 (UCRT64 recommended)
  - macOS: `clang` from Xcode Command Line Tools
  - Linux: `gcc` (`build-essential` / equivalent)

## 2) Install compiler toolchain

### Windows

1. Install MSYS2: <https://www.msys2.org/>
2. In **MSYS2 UCRT64** shell:

```sh
pacman -S --needed mingw-w64-ucrt-x86_64-toolchain
```

3. Add to **User PATH** (permanent), in this order:
   - `C:\msys64\ucrt64\bin`
   - `C:\msys64\usr\bin`

PowerShell (copy/paste) to prepend both paths to **User PATH**:

```powershell
$add = 'C:\msys64\ucrt64\bin;C:\msys64\usr\bin'
$cur = [Environment]::GetEnvironmentVariable('Path','User')
if ([string]::IsNullOrWhiteSpace($cur)) {
  [Environment]::SetEnvironmentVariable('Path', $add, 'User')
} elseif ($cur -notlike '*C:\msys64\ucrt64\bin*') {
  [Environment]::SetEnvironmentVariable('Path', "$add;$cur", 'User')
}
```

If you use `mingw64` instead of `ucrt64`, use:
   - `C:\msys64\mingw64\bin`
   - `C:\msys64\usr\bin`

PowerShell (copy/paste) for `mingw64`:

```powershell
$add = 'C:\msys64\mingw64\bin;C:\msys64\usr\bin'
$cur = [Environment]::GetEnvironmentVariable('Path','User')
if ([string]::IsNullOrWhiteSpace($cur)) {
  [Environment]::SetEnvironmentVariable('Path', $add, 'User')
} elseif ($cur -notlike '*C:\msys64\mingw64\bin*') {
  [Environment]::SetEnvironmentVariable('Path', "$add;$cur", 'User')
}
```

4. Close and reopen terminal/IDE.

### macOS

```sh
xcode-select --install
```

### Linux

Debian/Ubuntu:

```sh
sudo apt update
sudo apt install -y build-essential
```

Fedora/RHEL:

```sh
sudo dnf groupinstall -y "Development Tools"
```

Arch:

```sh
sudo pacman -S --needed base-devel
```

## 3) Verify environment

### Windows PowerShell

```powershell
go version
where gcc
gcc --version
$env:CGO_ENABLED='1'
go test ./...
```

### macOS/Linux

```sh
go version
which gcc || which clang
gcc --version || clang --version
CGO_ENABLED=1 go test ./...
```

## 4) Fix for IDE `go list` / gopls CGO errors

Typical error:

`go list failed to return CompiledGoFiles ... failure to perform cgo processing`

This usually means the IDE process does not inherit the same `PATH` as your shell.

- Restart IDE after PATH changes.
- Configure IDE Go environment:
  - `CGO_ENABLED=1`
  - `PATH` includes your compiler runtime dirs (Windows: MSYS2 dirs above).

## 5) Optional local caches (useful in restricted environments)

### Windows PowerShell

```powershell
$env:GOCACHE = "$PWD\.gocache"
$env:GOMODCACHE = "$PWD\.gomodcache"
$env:CGO_ENABLED='1'
go test ./...
```

### macOS/Linux

```sh
export GOCACHE="$PWD/.gocache"
export GOMODCACHE="$PWD/.gomodcache"
CGO_ENABLED=1 go test ./...
```
