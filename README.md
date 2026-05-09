# ft_irc — projet 42

## Principe du sujet

**ft_irc** consiste à écrire un **serveur IRC** en **C++** (souvent interdit d’utiliser des bibliothèques réseau « toutes faites » au-delà de ce que le sujet autorise). Un client IRC se connecte au serveur en **TCP**, échange des **messages texte** selon le protocole IRC, et peut rejoindre des **salons** (*channels*), envoyer des messages privés ou publics, etc.

En résumé, le serveur :

- **écoute** un ou plusieurs ports ;
- **accepte** les nouvelles connexions ;
- **relaye** et **valide** les commandes IRC attendues par le sujet (authentification, pseudos, salons, messages, modes, etc.) ;
- gère la **déconnexion** proprement et les erreurs réseau.

## Contrainte technique centrale : le réseau non bloquant

Le sujet impose en général d’utiliser **une seule fonction** parmi `poll`, `select`, `epoll` ou `kqueue` pour savoir **quels descripteurs de fichier** sont prêts à être lus ou écrits. L’idée est de ne **jamais bloquer** indéfiniment sur une seule connexion : le serveur doit traiter **plusieurs clients en parallèle** dans une même boucle d’événements.

Schéma habituel :

1. préparer les ensembles de fds à surveiller ;
2. appeler `poll` / `select` / … ;
3. pour chaque fd « prêt », lire ou écrire ;
4. recommencer.

## Ce dépôt : squelette `bircd`

Le dossier `bircd/` fournit un **exemple minimal** en C : boucle infinie avec `select`, sockets serveur, acceptation de clients, buffers de lecture/écriture. C’est une base pédagogique pour comprendre la **machine à états** autour des fds — à adapter et étendre vers un vrai **ft_irc** (C++, commandes IRC, modèle des utilisateurs et des channels, etc.).

---

*Projet école 42 — IRC en collaboration avec nawtam et SylAnceee*
