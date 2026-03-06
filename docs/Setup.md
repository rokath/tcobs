# Cross-Platform Setup (CGO)

This repository uses CGO in multiple packages.  
To run all tests (`go test ./...`), install Go and a working C compiler.

## Requirements

- Go 1.19 or newer
- A C toolchain available in `PATH`

## Quick Verification

Run these commands in the repository root:

```sh
go version
gcc --version
```

Then run tests:

```sh
CGO_ENABLED=1 go test ./...
```

On Windows PowerShell:

```powershell
$env:CGO_ENABLED='1'
go test ./...
```

## Windows (permanent setup)

Recommended compiler: MSYS2 MinGW-w64 (`x86_64`).

1. Install MSYS2 from `https://www.msys2.org/`.
2. Open **MSYS2 UCRT64** shell and install toolchain:

```sh
pacman -S --needed mingw-w64-ucrt-x86_64-toolchain
```

3. Add these directories permanently to the **user** `PATH` (order matters):
- `C:\msys64\ucrt64\bin`
- `C:\msys64\usr\bin`

PowerShell example (writes user PATH):

```powershell
$add = 'C:\msys64\ucrt64\bin;C:\msys64\usr\bin'
$current = [Environment]::GetEnvironmentVariable('Path', 'User')
[Environment]::SetEnvironmentVariable('Path', "$add;$current", 'User')
```

4. Close and reopen terminal.
5. Verify:

```powershell
where gcc
gcc --version
$env:CGO_ENABLED='1'
go test ./...
```

If you use `mingw64` instead of `ucrt64`, use:
- `C:\msys64\mingw64\bin`
- `C:\msys64\usr\bin`

## macOS (permanent setup)

Install Xcode Command Line Tools:

```sh
xcode-select --install
```

Verify:

```sh
clang --version
go test ./...
```

No extra PATH changes are usually needed.

## Linux (permanent setup)

Install build tools with your package manager.

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

Verify:

```sh
gcc --version
CGO_ENABLED=1 go test ./...
```

## Optional: Local Build Cache

If you want a repo-local Go build cache (useful in CI/debugging):

Linux/macOS:

```sh
export GOCACHE="$PWD/.gocache"
CGO_ENABLED=1 go test ./...
```

Windows PowerShell:

```powershell
$env:GOCACHE = "$PWD\.gocache"
$env:CGO_ENABLED='1'
go test ./...
```
