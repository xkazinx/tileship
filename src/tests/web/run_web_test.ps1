# The web client end to end: the Windows server (its WebSocket gateway serving
# deploy\server\web\, build_web.ps1) on ports 7031 / 7030, two headless browser tabs
# (kazilib web\tools\browser.js: Edge, WebGL2 on SwiftShader): one makes an account
# and logs in, the other is refused with no account, makes one and logs in. The
# names carry a random run number (WebA123456): reruns and the live database never
# collide. Screenshots in testerino\web\. Exit code = failed steps.
# The local server needs deploy\server\init.json's database (setup.ps1, README).
#   powershell -ExecutionPolicy Bypass -File src\tests\web\run_web_test.ps1 [-Url https://kazin.dev/projects/tileship/]
# -Url  a deployed page instead (no server started)
param([string]$Url = '')
$ErrorActionPreference = 'Stop'
$root = Resolve-Path (Join-Path $PSScriptRoot '..\..\..')
$shots = Join-Path $root 'testerino\web'
New-Item -ItemType Directory -Force $shots | Out-Null
$serverDir = Join-Path $root 'deploy\server'

$server = $null
if (-not $Url) {
	if (-not (Test-Path (Join-Path $serverDir 'web\index.wasm'))) { throw 'no deploy\server\web\index.wasm: run build_web.ps1' }
	$server = Start-Process -FilePath (Join-Path $serverDir 'server.exe') -ArgumentList 'port=7031', 'ws=7030' `
		-WorkingDirectory $serverDir -WindowStyle Hidden -PassThru
	Start-Sleep -Milliseconds 1500
	$Url = 'http://127.0.0.1:7030/index.html'
}

# Screenshot names in the steps land in testerino\web\; {RUN} is this run's number
$run = Get-Random -Minimum 100000 -Maximum 999999
$steps = Join-Path $shots 'two_players.json'
(Get-Content (Join-Path $PSScriptRoot 'two_players.json') -Raw) -replace '"shot": "', ('"shot": "' + ($shots -replace '\\', '/') + '/') -replace '\{RUN\}', $run |
	Set-Content -Encoding utf8 $steps

try {
	node (Join-Path $root 'src\dep\kazilib\web\tools\browser.js') --url $Url --pages 2 --steps $steps
	$failed = $LASTEXITCODE
} finally {
	if ($server) { Stop-Process -Id $server.Id -Force -ErrorAction SilentlyContinue }
}
exit $failed
