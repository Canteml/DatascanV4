<#
    Genera l'installer Windows di DatascanV4 con Qt Installer Framework.

    1. (opzionale, -Build) compila la release con mingw32-make
    2. ricrea packages/<pkg>/data con il layout di distribuzione:
           bin/  DatascanV4.exe + DLL Qt (windeployqt) + librerie terze + UsKey + EquipmentFile.txt
           res/  img, languages, templates, qml
    3. lancia binarycreator -> Output/DatascanV4_<versione>_Setup.exe

    Uso:  powershell -ExecutionPolicy Bypass -File build_installer.ps1 [-Build] [-Version 3.0.1]
          (oppure doppio clic su crea_installer.bat)

    -Version aggiorna la versione in config.xml e package.xml; la ReleaseDate
    del pacchetto viene sempre impostata alla data odierna.
#>
param(
    [switch]$Build,
    [string]$Version  = "",
    [string]$QtDir    = "C:\Qt\6.10.3\mingw_64",
    [string]$MingwDir = "C:\Qt\Tools\mingw1310_64",
    [string]$IfwDir   = "C:\Qt\Tools\QtInstallerFramework\4.11",
    # Cartella di build di Qt Creator per il kit Release (quella con Makefile.Release),
    # NON la cartella dove salvare il setup. Vuoto = dev\build\Desktop_Qt_6_10_3_MinGW_64_bit-Release
    [string]$BuildDir = "",
    # Installer NI "Ap_int_usb LV2011x64 Ccall" (cartella con setup.exe), include il LabVIEW Run-Time 2011 x64
    [string]$LabviewRteDir = "$PSScriptRoot\redist\Ap_int_usb_LV2011x64_Ccall"
)

$ErrorActionPreference = "Stop"

$Root    = Resolve-Path "$PSScriptRoot\.."
$Dev     = "$Root\dev"
if (-not $BuildDir) { $BuildDir = "$Dev\build\Desktop_Qt_6_10_3_MinGW_64_bit-Release" }
$Release = "$BuildDir\build\release"
$Config  = "$PSScriptRoot\config"
$Data    = "$PSScriptRoot\packages\com.Roboplanet.datascan\data"
$Output  = "$PSScriptRoot\Output"
$Exe     = "DatascanV4.exe"

function Step($msg) { Write-Host "`n==> $msg" -ForegroundColor Cyan }
function Fail($msg) { Write-Host "ERRORE: $msg" -ForegroundColor Red; exit 1 }
function Copy-Required($src, $dst) {
    if (-not (Test-Path $src)) { Fail "file mancante: $src" }
    Copy-Item $src $dst -Recurse -Force
}

foreach ($p in "$QtDir\bin\windeployqt.exe", "$MingwDir\bin\g++.exe", "$IfwDir\bin\binarycreator.exe") {
    if (-not (Test-Path $p)) { Fail "strumento non trovato: $p (controlla i parametri)" }
}
$env:PATH = "$QtDir\bin;$MingwDir\bin;$env:PATH"

# Versione e data di rilascio in config.xml / package.xml
$PackageXml = "$PSScriptRoot\packages\com.Roboplanet.datascan\meta\package.xml"
function Set-XmlTag($file, $tag, $value) {
    $text = [IO.File]::ReadAllText($file)
    $text = $text -replace "<$tag>[^<]*</$tag>", "<$tag>$value</$tag>"
    [IO.File]::WriteAllText($file, $text, [Text.UTF8Encoding]::new($false))
}
if ($Version) {
    if ($Version -notmatch '^\d+(\.\d+){1,3}$') { Fail "versione non valida: '$Version' (formato atteso es. 3.0.1)" }
    Set-XmlTag "$Config\config.xml" "Version" $Version
    Set-XmlTag $PackageXml "Version" $Version
}
Set-XmlTag $PackageXml "ReleaseDate" (Get-Date -Format "yyyy-MM-dd")

# Versione letta da config.xml, unica fonte per il nome del file
$Version = ([xml](Get-Content "$Config\config.xml")).Installer.Version
Write-Host "Versione installer: $Version"

# ---------------------------------------------------------------- 1. build
if ($Build) {
    Step "Compilazione release in $BuildDir"
    if (-not (Test-Path $BuildDir)) { New-Item -ItemType Directory $BuildDir | Out-Null }
    Push-Location $BuildDir
    try {
        # qmake solo per una build dir nuova: non sovrascrive quella configurata da Qt Creator
        if (-not (Test-Path "Makefile.Release")) {
            & "$QtDir\bin\qmake.exe" "$Dev\DatascanV4.pro" -spec win32-g++ "CONFIG+=release"
            if ($LASTEXITCODE) { Fail "qmake fallito" }
        }
        & "$MingwDir\bin\mingw32-make.exe" -f Makefile.Release -j $env:NUMBER_OF_PROCESSORS
        if ($LASTEXITCODE) { Fail "compilazione fallita" }
    } finally { Pop-Location }
}

if (-not (Test-Path "$Release\$Exe")) { Fail "$Release\$Exe non trovato: compila la release (o usa -Build)" }
$exeTime = (Get-Item "$Release\$Exe").LastWriteTime
Write-Host "Eseguibile: $Release\$Exe ($exeTime)"

# ---------------------------------------------------------------- 2. payload
Step "Pulizia $Data"
if (Test-Path $Data) { Remove-Item $Data -Recurse -Force }
$Bin = "$Data\bin"; $Res = "$Data\res"
New-Item -ItemType Directory $Bin, $Res | Out-Null

Step "Copia eseguibile"
Copy-Required "$Release\$Exe" $Bin

Step "Librerie di terze parti"
$thirdParty = @(
    "$Dev\lib\SDL2\bin\SDL2.dll",
    "$Dev\lib\jkqtplotter\bin\libJKQTCommon6_Release.dll",
    "$Dev\lib\jkqtplotter\bin\libJKQTMath6_Release.dll",
    "$Dev\lib\jkqtplotter\bin\libJKQTMathText6_Release.dll",
    "$Dev\lib\jkqtplotter\bin\libJKQTPlotter6_Release.dll",
    "$Dev\lib\vlcqt\bin\libVLCQtCore.dll",
    "$Dev\lib\vlcqt\bin\libVLCQtWidgets.dll",
    "$Dev\lib\vlcqt\bin\libvlc.dll",
    "$Dev\lib\vlcqt\bin\libvlccore.dll",
    "$Dev\UsKey\Ap_int_usb.dll",
    "$Dev\UsKey\USB2.dll",
    "$Dev\UsKey\USB2X64.dll",
    "$Dev\UsKey\ExeUSB2X86.exe",
    "$Dev\UsKey\Ap_int_usb.ini",
    "$Dev\UsKey\license.dat",
    "$Dev\UsKey\LecoeurUS.dll",
    "$Dev\UsKey\DecodeIntr.dll",
    "$Dev\UsKey\Decodeintr.ini",
    "$Dev\UsKey\lvanlys.dll",
    "$Root\release\win\EquipmentFile.txt"
)
foreach ($f in $thirdParty) { Copy-Required $f $Bin }

# windeployqt riceve anche le DLL terze basate su Qt, cosi' ne risolve le dipendenze
# (es. JKQTPlotter -> Qt6PrintSupport, che l'exe non importa direttamente)
Step "windeployqt"
$qtBased = "$Bin\$Exe", "$Bin\libJKQTCommon6_Release.dll", "$Bin\libJKQTMath6_Release.dll",
           "$Bin\libJKQTMathText6_Release.dll", "$Bin\libJKQTPlotter6_Release.dll", "$Bin\libVLCQtCore.dll"
& "$QtDir\bin\windeployqt.exe" --release --compiler-runtime --qmldir "$Dev\src\qml" --dir $Bin @qtBased
if ($LASTEXITCODE) { Fail "windeployqt fallito" }

# FFmpeg usato dal backend multimedia di Qt (copiato anche dal .pro)
foreach ($f in "avcodec-61", "avformat-61", "avutil-59", "swresample-5", "swscale-8") {
    if (-not (Test-Path "$Bin\$f.dll")) { Copy-Required "$QtDir\bin\$f.dll" $Bin }
}

# Plugin VLC: libvlc li cerca in <cartella di libvlc.dll>\plugins
Copy-Required "$Dev\lib\vlcqt\bin\plugins" "$Bin\plugins"
Remove-Item "$Bin\plugins\plugins.dat" -ErrorAction SilentlyContinue

Step "Risorse applicazione"
Copy-Required "$Dev\src\res\*" $Res
Copy-Required "$Dev\src\qml" "$Res\qml"

# LabVIEW Run-Time 2011 x64 richiesto da Ap_int_usb.dll: installer NI del fornitore (variante Ccall),
# eseguito da installscript.qs solo se il Run-Time manca, poi cancellato
Step "LabVIEW Run-Time 2011 x64"
if (-not (Test-Path "$LabviewRteDir\setup.exe")) { Fail "installer LabVIEW Run-Time non trovato: $LabviewRteDir\setup.exe" }
New-Item -ItemType Directory "$Data\redist" | Out-Null
Copy-Required $LabviewRteDir "$Data\redist\lvrte2011"

foreach ($f in "$Bin\$Exe", "$Bin\Qt6Core.dll", "$Bin\platforms\qwindows.dll", "$Bin\libstdc++-6.dll", "$Res\qml\main.qml") {
    if (-not (Test-Path $f)) { Fail "payload incompleto, manca: $f" }
}

# Dipendenze transitive (import statici) dall'exe e dal plugin di piattaforma:
# una DLL mancante qui = crash 0xC0000135 all'avvio sul PC di destinazione
Step "Verifica dipendenze DLL"
$objdump = "$MingwDir\bin\objdump.exe"
$queue   = [System.Collections.Queue]::new(@("$Bin\$Exe", "$Bin\platforms\qwindows.dll"))
$seen    = @{}; $missing = @()
while ($queue.Count) {
    $file = $queue.Dequeue()
    foreach ($m in (& $objdump -p $file | Select-String 'DLL Name: (\S+)')) {
        $dll = $m.Matches[0].Groups[1].Value
        if ($seen[$dll.ToLower()]) { continue }
        $seen[$dll.ToLower()] = $true
        if (Test-Path "$Bin\$dll") { $queue.Enqueue("$Bin\$dll") }
        elseif (-not (Test-Path "$env:SystemRoot\System32\$dll") -and $dll -notmatch '^(api|ext)-ms-') { $missing += "$dll (richiesta da $(Split-Path $file -Leaf))" }
    }
}
if ($missing) { Fail "DLL mancanti nel payload:`n  $($missing -join "`n  ")" }
Write-Host "OK: $($seen.Count) DLL risolte"

$size = (Get-ChildItem $Data -Recurse -File | Measure-Object Length -Sum).Sum / 1MB
Write-Host ("Payload: {0:N0} MB" -f $size)

# ---------------------------------------------------------------- 3. installer
Step "Branding installer"
Copy-Required "$Dev\src\res\img\application.ico" $Config
Copy-Required "$Dev\src\res\img\application.png" $Config
Copy-Required "$Root\release\win\res\banner.bmp" $Config

Step "binarycreator"
New-Item -ItemType Directory $Output -Force | Out-Null
$Setup = "$Output\DatascanV4_${Version}_Setup.exe"
& "$IfwDir\bin\binarycreator.exe" --offline-only -c "$Config\config.xml" -p "$PSScriptRoot\packages" $Setup
if ($LASTEXITCODE) { Fail "binarycreator fallito" }

$mb = (Get-Item $Setup).Length / 1MB
Write-Host ("`nOK: {0} ({1:N0} MB)" -f $Setup, $mb) -ForegroundColor Green
