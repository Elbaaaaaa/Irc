# Fonctions système — Réseau, Signaux, Fichiers

---

## Création & configuration du socket

**`socket`**
Crée un point de communication réseau et retourne un descripteur de fichier. On précise le domaine (AF_INET pour IPv4, AF_INET6 pour IPv6), le type (SOCK_STREAM pour TCP, SOCK_DGRAM pour UDP) et le protocole.

**`close`**
Ferme un descripteur de fichier (socket ou autre) et libère les ressources noyau associées. À appeler dès que le socket n'est plus utilisé pour éviter les fuites de descripteurs.

**`setsockopt`**
Configure une option sur un socket (ex. `SO_REUSEADDR`, `SO_KEEPALIVE`). Permet de modifier le comportement bas niveau de la connexion avant ou après sa création.

**`getsockname`**
Récupère l'adresse locale (IP + port) à laquelle le socket est effectivement lié. Utile quand le système a choisi le port automatiquement (port 0 passé à `bind`).

---

## Résolution de noms & adresses

**`getprotobyname`**
Retourne le numéro de protocole correspondant à un nom (ex. `"tcp"` → 6) en consultant `/etc/protocols`. Rarement nécessaire, car `getaddrinfo` s'en charge implicitement.

**`gethostbyname`**
Résout un nom d'hôte en adresse IPv4 et retourne une structure `hostent`. Fonction obsolète, non thread-safe — préférer `getaddrinfo` pour tout nouveau code.

**`getaddrinfo`**
Convertit un nom d'hôte et un nom de service en une liste de structures `sockaddr` prêtes à l'emploi. Version moderne, thread-safe, supportant indifféremment IPv4 et IPv6.

**`freeaddrinfo`**
Libère la liste chaînée allouée par `getaddrinfo`. À appeler impérativement après utilisation pour éviter toute fuite mémoire.

---

## Conversion adresses / ordre des octets

**`htons`**
Convertit un entier 16 bits de l'ordre de l'hôte vers l'ordre réseau (big-endian). Utilisé systématiquement pour les numéros de port avant de les placer dans une `sockaddr_in`.

**`htonl`**
Même conversion sur 32 bits. Utilisé pour les adresses IPv4 avant de les stocker dans `sin_addr`.

**`ntohs`**
Convertit un entier 16 bits de l'ordre réseau vers l'ordre de l'hôte. Inverse de `htons`, pour lire un port reçu dans une structure d'adresse.

**`ntohl`**
Inverse de `htonl` sur 32 bits. Pour relire une adresse IPv4 dans l'ordre local de la machine.

**`inet_addr`**
Convertit une adresse IPv4 en notation pointée (`"192.168.1.1"`) en entier 32 bits réseau. Retourne `INADDR_NONE` en cas d'erreur — préférer `inet_pton` pour les nouveaux projets.

**`inet_ntoa`**
Fait l'inverse : transforme un entier 32 bits réseau en chaîne pointée lisible (ex. `"10.0.0.1"`). Attention : retourne un buffer statique, la fonction n'est pas thread-safe.

**`inet_ntop`**
Version moderne et thread-safe de `inet_ntoa`. Convertit une adresse binaire IPv4 ou IPv6 en chaîne lisible dans un buffer fourni par l'appelant.

---

## Cycle de vie de la connexion

**`bind`**
Associe un socket à une adresse IP et un port locaux. Obligatoire côté serveur avant d'appeler `listen` ; optionnel côté client.

**`connect`**
Côté client : déclenche le handshake TCP vers un serveur distant, ou associe simplement une adresse distante pour UDP. Bloquant jusqu'à l'établissement de la connexion (sauf socket non-bloquant).

**`listen`**
Passe le socket en mode écoute passive et fixe la taille de la file d'attente des connexions entrantes (`backlog`). À appeler après `bind`, avant `accept`.

**`accept`**
Extrait la première connexion en attente de la file et retourne un nouveau socket dédié à cet échange. Bloquant par défaut ; retourne -1 en cas d'erreur ou si le socket est non-bloquant et la file vide.

---

## Envoi & réception de données

**`send`**
Envoie des octets sur un socket connecté. Retourne le nombre d'octets effectivement envoyés, qui peut être inférieur à la demande — une boucle est souvent nécessaire.

**`recv`**
Lit des données disponibles depuis un socket connecté. Bloquant par défaut ; retourne 0 si la connexion a été fermée proprement par le pair, -1 en cas d'erreur.

---

## Surveillance de descripteurs

**`poll`**
Surveille simultanément plusieurs descripteurs de fichiers (sockets, pipes, fichiers) et se bloque jusqu'à ce qu'au moins l'un d'eux soit prêt en lecture, écriture ou en erreur. Alternative portable à `select`, sans limite fixe sur le nombre de descripteurs.

---

## Opérations sur fichiers & descripteurs

**`lseek`**
Déplace la position de lecture/écriture dans un fichier ouvert. Permet de se positionner à un offset absolu, relatif à la position courante, ou depuis la fin du fichier.

**`fstat`**
Récupère les métadonnées d'un fichier (taille, permissions, timestamps, type) à partir de son descripteur ouvert. Évite de rouvrir le chemin et fonctionne même si le fichier a été renommé ou supprimé.

**`fcntl`**
Manipule les propriétés d'un descripteur ouvert : passer un socket en mode non-bloquant (`O_NONBLOCK`), dupliquer un fd, poser des verrous de fichier, etc. Couteau suisse de la gestion des descripteurs.

---

## Signaux

**`signal`**
Installe un gestionnaire simple pour un signal donné (ex. `SIGINT`, `SIGTERM`). Interface portable mais limitée et au comportement parfois imprévisible — `sigaction` lui est préféré dans tout code sérieux.

**`sigaction`**
Version robuste et portable de `signal`. Permet de spécifier le handler, des flags de comportement (ex. `SA_RESTART` pour relancer les appels système interrompus) et un masque de signaux bloqués pendant l'exécution du handler.

**`sigemptyset`**
Initialise un masque de signaux (`sigset_t`) vide, c'est-à-dire qu'aucun signal n'est masqué. Point de départ obligatoire avant toute manipulation d'un masque.

**`sigfillset`**
Remplit un masque avec l'ensemble de tous les signaux possibles. Utile pour bloquer temporairement toute interruption pendant une section critique.

**`sigaddset`**
Ajoute un signal spécifique à un masque existant. S'utilise après `sigemptyset` pour construire précisément la liste des signaux à bloquer.

**`sigdelset`**
Retire un signal spécifique d'un masque existant. Inverse de `sigaddset`.

**`sigismember`**
Teste si un signal donné est présent dans un masque. Retourne 1 si le signal en fait partie, 0 sinon, -1 en cas d'erreur.

---
