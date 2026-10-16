function Component()
{
    // Installazione per tutti gli utenti: collegamenti nel menu Start comune
    installer.setValue("AllUsers", "true");
}

// Ap_int_usb.dll (chiave UsKey) richiede il LabVIEW Run-Time 2011 a 64 bit
function isLabviewRte2011x64Installed()
{
    var reg = installer.value("HKEY_LOCAL_MACHINE\\SOFTWARE\\National Instruments\\LabVIEW Run-Time\\11.0\\Path");
    if (reg !== "" && installer.fileExists(reg + "/lvrt.dll"))
        return true;
    return installer.fileExists(installer.value("ApplicationsDirX64")
        + "/National Instruments/Shared/LabVIEW Run-Time/2011/lvrt.dll");
}

Component.prototype.createOperations = function()
{
    component.createOperations();

    if (systemInfo.productType !== "windows")
        return;

    // Installer NI del fornitore (data/redist/lvrte2011), silenzioso con sola barra di avanzamento.
    // Il Run-Time e' condiviso con altri software NI: non viene rimosso alla disinstallazione.
    var redist = "@TargetDir@/redist";
    if (!isLabviewRte2011x64Installed()) {
        component.addElevatedOperation("Execute", "{0,3010}",
            redist + "/lvrte2011/setup.exe",
            "/qb", "/AcceptLicenses", "yes", "/r:n", "/disableNotificationCheck",
            "workingdirectory=" + redist + "/lvrte2011",
            "errormessage=Installazione del LabVIEW Run-Time 2011 (64 bit) non riuscita. "
                + "Senza di esso la chiave UsKey non funziona.");
    }
    component.addElevatedOperation("Execute", "{0,1}", "powershell", "-NoProfile", "-Command",
        "Remove-Item -LiteralPath '" + redist + "' -Recurse -Force -ErrorAction SilentlyContinue");

    var exe = "@TargetDir@/bin/DatascanV3.exe";

    component.addOperation("CreateShortcut", exe,
        "@StartMenuDir@/DatascanV3.lnk",
        "workingDirectory=@TargetDir@",
        "iconPath=" + exe, "iconId=0",
        "description=DatascanV3");

    component.addOperation("CreateShortcut", "@TargetDir@/DatascanV3_Uninstall.exe",
        "@StartMenuDir@/Disinstalla DatascanV3.lnk",
        "workingDirectory=@TargetDir@",
        "description=Disinstalla DatascanV3");

    component.addOperation("CreateShortcut", exe,
        "@DesktopDir@/DatascanV3.lnk",
        "workingDirectory=@TargetDir@",
        "iconPath=" + exe, "iconId=0",
        "description=DatascanV3");
}
