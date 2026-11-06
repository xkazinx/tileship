# The server for Linux (the deployed one), built here in a Docker container of the
# server's own system (Ubuntu 26.04: its glibc), from src\server\CMakeLists.txt.
# The image: kazilib's compiler image (kazilib-linux-build:26.04: g++, cmake, ninja)
# plus libmysqlclient-dev and pkg-config, made once as tileship-linux-build:26.04.
# Output src\build\linux\tileship_server (stripped; libstdc++ inside; the box needs
# libmysqlclient24, which deploy\ubuntu\install.sh installs).
#   powershell -ExecutionPolicy Bypass -File build_linux.ps1
$ErrorActionPreference = 'Stop'
$root = $PSScriptRoot
$base = 'kazilib-linux-build:26.04'
$image = 'tileship-linux-build:26.04'

if (-not (docker images -q $image)) {
	if (-not (docker images -q $base)) {
		"FROM ubuntu:26.04`nRUN apt-get update -qq && DEBIAN_FRONTEND=noninteractive apt-get install -y -qq g++ cmake ninja-build >/dev/null && rm -rf /var/lib/apt/lists/*`n" |
			docker build -q -t $base - | Out-Null
		if ($LASTEXITCODE) { throw "docker build of $base failed" }
	}
	"FROM $base`nRUN apt-get update -qq && DEBIAN_FRONTEND=noninteractive apt-get install -y -qq libmysqlclient-dev pkg-config >/dev/null && rm -rf /var/lib/apt/lists/*`n" |
		docker build -q -t $image - | Out-Null
	if ($LASTEXITCODE) { throw "docker build of $image failed" }
}

$jobs = [Math]::Min(8, [Math]::Max(1, [Environment]::ProcessorCount - 1))
$src = ($root -replace '\\', '/')
docker run --rm -v "${src}:/ts" $image bash -c "set -e; cmake -S /ts/src/server -B /tmp/b -G Ninja -DCMAKE_BUILD_TYPE=Release >/dev/null; cmake --build /tmp/b -j $jobs; strip /tmp/b/tileship_server; mkdir -p /ts/src/build/linux; cp /tmp/b/tileship_server /ts/src/build/linux/"
if ($LASTEXITCODE) { throw 'Linux server build failed' }
Write-Host "Linux server: $(Join-Path $root 'src\build\linux\tileship_server')"
