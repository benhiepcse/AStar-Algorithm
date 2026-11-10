param([switch]$Sanitize)

$ErrorActionPreference = 'Stop'
Push-Location (Split-Path -Parent $PSScriptRoot)
try {
    New-Item -ItemType Directory -Path .\build\qa -Force | Out-Null

    $flags = @('-std=c++11', '-Wall', '-Wextra', '-Werror', '-pedantic', '-I.\src')
    if ($Sanitize) {
        $flags += '-fsanitize=address,undefined'
    }

    $tests = @(Get-ChildItem .\tests\test_*.cpp | Sort-Object Name)
    foreach ($test in $tests) {
        $exe = ".\build\qa\$($test.BaseName).exe"
        $sources = @($test.FullName, '.\src\PathNode.cpp')

        if ($test.BaseName -ne 'test_base2' -and
            -not (Select-String -LiteralPath $test.FullName -Pattern '^\s*#\s*include\s+"Algo\.cpp"' -Quiet)) {
            $sources += '.\src\Algo.cpp'
        }

        Write-Host "`n=== $($test.Name) ==="
        & g++ @flags @sources -o $exe
        if ($LASTEXITCODE -ne 0) { throw "Build failed: $($test.Name)" }

        & $exe
        if ($LASTEXITCODE -ne 0) { throw "Test failed: $($test.Name)" }
    }

    & g++ @flags .\src\main.cpp .\src\Algo.cpp .\src\PathNode.cpp -o .\build\qa\demo.exe
    if ($LASTEXITCODE -ne 0) { throw 'Demo build failed.' }

    & .\build\qa\demo.exe
    if ($LASTEXITCODE -ne 0) { throw 'Demo failed.' }

    Write-Host "`nPASS: $($tests.Count) executables and demo"
} finally {
    Pop-Location
}
