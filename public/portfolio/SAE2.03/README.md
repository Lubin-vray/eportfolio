# 🚀 SAÉ 2.03 — Lancement du projet

> Infrastructure Docker : WordPress + MariaDB + phpMyAdmin + Portfolio Symfony

\---

## 📋 Prérequis

Avant de commencer, assurez-vous d'avoir installé :

* [Docker Desktop](https://www.docker.com/products/docker-desktop/) — démarré et fonctionnel
* [Git](https://git-scm.com/) *(uniquement pour la méthode GitHub)*
* PowerShell *(Windows)* ou Terminal *(Linux/Mac)*

\---

## 📦 Méthode 1 — Depuis le fichier ZIP

### 1\. Extraire le ZIP

Téléchargez et extrayez l'archive, puis ouvrez un terminal dans le dossier :

```powershell
cd C:\\\\chemin\\\\vers\\\\SAE2.03
```

### 2\. Lancer le projet

```powershell
docker-compose up -d --build
```

C'est tout. Docker télécharge les images, construit le portfolio et démarre les 4 conteneurs.

\---

## 🐙 Méthode 2 — Depuis GitHub

### 1\. Cloner le repository

```powershell
git clone https://github.com/Lubin-vray/SAE2.03.git
cd SAE2.03
```

### 2\. Lancer le projet

```powershell
docker-compose up -d --build
```

\---

## 🌐 Accès aux services

Une fois lancé, les services sont disponibles à ces adresses :

|Service|URL|Identifiants|
|-|-|-|
|🌍 WordPress|http://127.0.0.1|compte admin WordPress|
|🗄️ phpMyAdmin|http://127.0.0.1:9000|
|💼 Portfolio|http://127.0.0.1:82|—|
|🔌 MariaDB|127.0.0.1:3306|user : `lubin` / root : `root`|

\---

## ⚡ Alternative — Script PowerShell

Si vous préférez ne pas utiliser Docker Compose, un script PowerShell fait exactement la même chose :

```powershell
.\\\\script.ps1
```

## 🛑 Arrêter le projet

```powershell
# Arrêter sans supprimer les conteneurs
docker-compose stop

# Arrêter et supprimer les conteneurs (les données sont conservées)
docker-compose down
```

\---

## 🧹 Tout supprimer

```powershell
# Supprimer conteneurs + images + réseau
.\\\\cleanup.ps1
```

Ou manuellement :

```powershell
docker-compose down
docker rmi mariadb:latest wordpress:latest phpmyadmin:latest portfolio
docker network prune -f
```

## 📁 Structure du projet

```
SAE2.03/
├── docker-compose.yml        ← Infrastructure principale
├── script.ps1                ← Déploiement alternatif (commandes docker run)
├── cleanup.ps1               ← Suppression complète
├── badowordpress/            ← Données MariaDB (volume persistant)
├── htmlwordpress/            ← Fichiers WordPress (volume persistant)
├── kaliDoS/
│   ├── Dockerfile            ← Image Kali Linux + slowhttptest
│   └── docker-compose.yml   ← Scénario attaque DoS Slowloris
└── portfolio/
    ├── Dockerfile            ← Image Symfony (clone depuis GitHub)
    └── site.conf             ← Configuration Apache
```

\---

## 🗺️ Réseau Docker

Tous les conteneurs communiquent sur le réseau privé `sae23\_vray` (`192.168.203.0/24`) :

|Conteneur|IP fixe|Port exposé|
|-|-|-|
|`mariadb\\\_vray`|192.168.203.10|3306|
|`phpmyadmin\\\_vray`|192.168.203.11|9000|
|`wordpress\\\_vray`|192.168.203.12|80|
|`mon-portfolio`|192.168.203.13|82|



