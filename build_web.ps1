# The client for a web browser (WebAssembly): src\client\CMakeLists.txt with
# Emscripten, against kazilib's SFML web build (D:\code\tools\kazilib-web, made by
# src\dep\kazilib\web\build_deps.ps1 when it is missing). Output
# src\build\web\client-build\index.html / .js / .wasm, copied to deploy\server\web\
# (the Windows server's gateway serves it: http://127.0.0.1:7020/).
#   powershell -ExecutionPolicy Bypass -File build_web.ps1 [-Deps] [-Debug]
# -Deps   rebuild SFML for the web first (after a kazilib web patch change)
# -Debug  function names in stack traces and Emscripten's memory checks
param([switch]$Deps, [switch]$Debug)
$ErrorActionPreference = 'Stop'
$root = $PSScriptRoot
$web = Join-Path $root 'src\build\web'
$emsdk = 'D:\code\tools\emsdk'
$depsOut = 'D:\code\tools\kazilib-web'
$sfmlDir = Join-Path $depsOut 'sfml-web\lib\cmake\SFML'

if ($Deps -or -not (Test-Path (Join-Path $sfmlDir 'SFMLConfig.cmake'))) {
	& (Join-Path $root 'src\dep\kazilib\web\build_deps.ps1') -Out $depsOut -Emsdk $emsdk
	if ($LASTEXITCODE) { throw 'web dependencies failed' }
}

$env:EMSDK_QUIET = 1
. (Join-Path $emsdk 'emsdk_env.ps1') | Out-Null
$ninja = (Get-Command ninja -ErrorAction SilentlyContinue)
$ninja = if ($ninja) { $ninja.Source } else {
	$vs = 'C:\Program Files\Microsoft Visual Studio\18\Insiders\Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja\ninja.exe'
	if (Test-Path $vs) { $vs } else {
		Get-ChildItem 'C:\Program Files\Microsoft Visual Studio' -Recurse -Filter ninja.exe -ErrorAction SilentlyContinue | Select-Object -First 1 -ExpandProperty FullName
	}
}
if (-not $ninja) { throw 'ninja.exe not found (PATH or Visual Studio)' }

$build = Join-Path $web 'client-build'
$linkFlags = if ($Debug) { '--profiling-funcs -sASSERTIONS=2 -sSAFE_HEAP=1' } else { '' }
emcmake cmake -S (Join-Path $root 'src\client') -B $build -G Ninja "-DCMAKE_MAKE_PROGRAM=$ninja" -DCMAKE_BUILD_TYPE=Release `
	"-DSFML_DIR=$($sfmlDir -replace '\\', '/')" "-DCMAKE_EXE_LINKER_FLAGS=$linkFlags" | Out-Null
if ($LASTEXITCODE) { throw 'configure failed' }
$jobs = [Math]::Min(8, [Math]::Max(1, [Environment]::ProcessorCount - 1))
cmake --build $build -j $jobs
if ($LASTEXITCODE) { throw 'web client build failed' }

$serverWeb = Join-Path $root 'deploy\server\web'
New-Item -ItemType Directory -Force $serverWeb | Out-Null
Copy-Item (Join-Path $build 'index.*') $serverWeb -Force
Write-Host "Web client: $build (copied to $serverWeb)"
