# Build the native library of LiteScript.Net for Windows and copy it in
# dotnet\artifacts\runtimes\<rid>\native (used by the tests and by "dotnet pack")
#
# Usage: dotnet\build-native.ps1 [-Rid win-x64] [-MesonOptions @('-Dwerror=true')]
#
# Needs Python with Meson and Ninja (python -m pip install meson ninja) and Visual Studio 2022
# (its environment is activated automatically if cl.exe isn't in the PATH).

param(
    [string]$Rid = "",
    [string[]]$MesonOptions = @()
)

$ErrorActionPreference = "Stop"
$Root = Resolve-Path (Join-Path $PSScriptRoot "..")

if ($Rid -eq "") {
    $arch = [System.Runtime.InteropServices.RuntimeInformation]::OSArchitecture.ToString().ToLowerInvariant()
    $Rid = "win-$arch"
}

# Find Meson: the command "meson" or the Python module
$meson = @()
if (Get-Command meson -ErrorAction SilentlyContinue) {
    $meson = @("meson")
} else {
    foreach ($python in @("python", "py", "python3")) {
        if (Get-Command $python -ErrorAction SilentlyContinue) {
            & $python -c "import mesonbuild" 2>$null
            if ($LASTEXITCODE -eq 0) {
                $meson = @($python, "-m", "mesonbuild.mesonmain")
                break
            }
        }
    }
}
if ($meson.Count -eq 0) {
    throw "Meson isn't installed, install it with: python -m pip install meson ninja"
}

function Invoke-Meson([string[]]$arguments) {
    $exe = $meson[0]
    $prefix = @()
    if ($meson.Count -gt 1) { $prefix = $meson[1..($meson.Count - 1)] }
    & $exe @prefix @arguments
    if ($LASTEXITCODE -ne 0) { throw "meson $($arguments -join ' ') failed" }
}

$buildDir = Join-Path $Root "builddir\dotnet-$Rid"
$options = @("--buildtype=release", "-Db_vscrt=mt", "-Ddotnet=true", "-Dtests=false", "-Dexamples=false", "-Dplayground=false", "-Dbenchmarks=false")
if (-not (Get-Command cl -ErrorAction SilentlyContinue)) {
    $options += "--vsenv"
}

if (-not (Test-Path (Join-Path $buildDir "meson-private\coredata.dat"))) {
    Invoke-Meson (@("setup", $buildDir, $Root) + $options + $MesonOptions)
} elseif ($MesonOptions.Count -gt 0) {
    Invoke-Meson (@("configure", $buildDir) + $MesonOptions)
}
Invoke-Meson @("compile", "-C", $buildDir)

$outDir = Join-Path $Root "dotnet\artifacts\runtimes\$Rid\native"
New-Item -ItemType Directory -Force $outDir | Out-Null
Copy-Item (Join-Path $buildDir "dotnet\native\litescript_net.dll") $outDir
Write-Host "Native library: $(Join-Path $outDir 'litescript_net.dll')"
