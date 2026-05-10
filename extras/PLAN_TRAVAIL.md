# Plan de travail — ft_irc (3 personnes)

> Projet 42 — Serveur IRC en C++98  
> Exécutable : `./ircserv <port> <password>`  
> Contrainte absolue : **un seul `poll()`** pour toutes les E/S, **pas de fork**, tout **non-bloquant**.

---

## Étape 0 — Squelette commun (tous ensemble, ~1 demi-journée)

**But :** que tout le monde compile dès le départ, et que les interfaces soient figées avant de se séparer.

### Fichiers à créer ensemble

| Fichier | Contenu |
|---------|---------|
| `Makefile` | `NAME=ircserv`, flags `-Wall -Wextra -Werror -std=c++98`, règles `all clean fclean re` |
| `main.cpp` | Parse `argv` (port + password), instancie `Server`, lance la boucle |
| `irc_replies.hpp` | Toutes les constantes de codes numériques IRC (voir liste ci-dessous) |
| `Server.hpp` | Déclaration complète avec **tous** les getters/setters et méthodes partagées |
| `Client.hpp` | `_fd`, `_nickname`, `_username`, `_realname`, `_password`, `_authenticated`, `_buffer` (std::string), `_writeQueue` (std::string) |
| `Channel.hpp` | Squelette de la classe Channel (Person C remplit `Channel.cpp`) |

### Interface `Server` à figer dès l'Étape 0

```cpp
// Getters
Client*   getClientByFd(int fd);
Client*   getClientByNick(const std::string& nick);
Channel*  getChannel(const std::string& name);

// Infrastructure réseau — implémentée par Person A
void  sendToClient(int fd, const std::string& msg);
void  disconnectClient(int fd);

// Gestion des salons — implémentée par Person C, appelée par Person B
Channel* createChannel(const std::string& name, Client& op);
void     removeChannel(const std::string& name);
void     removeClientFromAllChannels(Client& client); // appelé par QUIT
```

> Ces signatures **ne bougent plus** une fois l'Étape 0 validée.
> Person B peut coder ses commandes sans attendre Person C en incluant juste `Channel.hpp`.

### `irc_replies.hpp` — codes à définir ensemble

```cpp
// Succès connexion
#define RPL_WELCOME          "001"
#define RPL_YOURHOST         "002"
#define RPL_CREATED          "003"
#define RPL_MYINFO           "004"

// Salons
#define RPL_INVITING         "341"
#define RPL_NOTOPIC          "331"
#define RPL_TOPIC            "332"
#define RPL_NAMREPLY         "353"
#define RPL_ENDOFNAMES       "366"

// Erreurs auth (Person B)
#define ERR_NONICKNAMEGIVEN  "431"
#define ERR_ERRONEUSNICKNAME "432"
#define ERR_NICKNAMEINUSE    "433"
#define ERR_NOTREGISTERED    "451"
#define ERR_NEEDMOREPARAMS   "461"
#define ERR_ALREADYREGISTERED "462"
#define ERR_PASSWDMISMATCH   "464"

// Erreurs salons (Person C)
#define ERR_NOSUCHNICK       "401"
#define ERR_NOSUCHCHANNEL    "403"
#define ERR_CANNOTSENDTOCHAN "404"
#define ERR_USERNOTINCHANNEL "441"
#define ERR_NOTONCHANNEL     "442"
#define ERR_USERONCHANNEL    "443"
#define ERR_CHANNELISFULL    "471"
#define ERR_UNKNOWNMODE      "472"
#define ERR_INVITEONLYCHAN   "473"
#define ERR_BADCHANNELKEY    "475"
#define ERR_CHANOPRIVSNEEDED "482"
```

---

## Personne A — Architecture Réseau (Le "Moteur")

**But :** faire circuler les octets entre les clients et le serveur sans jamais bloquer.

### Fichiers

- `Server.cpp` — implémentation complète de Server

### Responsabilités

#### 1. Initialisation du socket serveur
```
socket() → setsockopt(SO_REUSEADDR) → bind() → listen() → fcntl(O_NONBLOCK)
```
Le fd serveur est ajouté au tableau `pollfd` avec `POLLIN`.

#### 2. La boucle `poll()` — coeur du serveur

```
while (true)
{
    poll(fds, nfds, -1)
    
    si fd_serveur est prêt  → acceptNewClient()
    pour chaque client prêt en lecture  → readFromClient(fd)
    pour chaque client prêt en écriture → writeToClient(fd)
}
```

> **Règle absolue du sujet** : tout `recv`/`send` doit passer par ce `poll()`.
> Un `send` direct hors de la boucle = **note 0**.

#### 3. Accepter un nouveau client
```
accept() → fcntl(O_NONBLOCK) → ajouter à pollfd[] → créer Client → stocker dans _clients
```
Afficher : `"New client #fd connected from IP:port"`

#### 4. Lire depuis un client (`readFromClient`)
```
recv() → ajouter au client._buffer
tant que '\r\n' dans _buffer :
    extraire la ligne complète
    appeler le dispatcher (Person B) avec cette ligne
```
Si `recv` retourne `0` ou `-1` : appeler `disconnectClient(fd)`.

#### 5. Écrire vers un client (`writeToClient` + `sendToClient`)
- `sendToClient(fd, msg)` : **ajoute** `msg` dans `client._writeQueue` et positionne `POLLOUT` sur ce fd.
- `writeToClient(fd)` : appelée par `poll()` quand fd est prêt en écriture → `send()` depuis `_writeQueue`, retirer les octets envoyés, désactiver `POLLOUT` si queue vide.

#### 6. Déconnecter un client
```
close(fd) → retirer de pollfd[] → retirer de _clients
(Person C s'occupe de retirer le client des salons via removeClientFromAllChannels)
```

### Points d'attention
- **Données fragmentées** : IRC envoie `\r\n` en fin de ligne mais les paquets TCP peuvent arriver par morceaux. Il faut toujours accumuler dans `_buffer` et n'extraire que les lignes complètes.
- Test du sujet : `nc -C 127.0.0.1 6667` puis envoyer `com` + `man` + `d\n` en 3 fois → le serveur doit reconstruire `command\r\n`.

---

## Personne B — Parser, Authentification & Messages (Le "Cerveau")

**But :** transformer une ligne brute en action concrète.

### Fichiers

- `Parser.cpp` — découpe une ligne IRC en composantes
- `Dispatcher.cpp` — relie un nom de commande à sa fonction
- `Commands_connection.cpp` — PASS, NICK, USER, QUIT, CAP
- `Commands_messages.cpp` — PING, PONG, PRIVMSG, NOTICE

### Responsabilités

#### 1. Le parser IRC

Format d'un message IRC :
```
[:prefix] COMMAND [param1] [param2] [...] [:trailing]\r\n
```

Créer une struct :
```cpp
struct IrcMessage {
    std::string prefix;
    std::string command;
    std::vector<std::string> params;
};
IrcMessage parseMessage(const std::string& raw);
```

#### 2. Le dispatcher

```cpp
std::map<std::string, void (Server::*)(int, IrcMessage&)> _commands;

// Dans le constructeur :
_commands["PASS"]    = &Server::cmdPass;
_commands["NICK"]    = &Server::cmdNick;
_commands["USER"]    = &Server::cmdUser;
_commands["QUIT"]    = &Server::cmdQuit;
_commands["CAP"]     = &Server::cmdCap;
_commands["PING"]    = &Server::cmdPing;
_commands["PRIVMSG"] = &Server::cmdPrivmsg;
_commands["NOTICE"]  = &Server::cmdNotice;
// ...
```

Si la commande n'existe pas → ignorer silencieusement (ne pas crasher).

#### 3. Commandes de connexion

##### `CAP` — à gérer en premier
Les clients modernes (HexChat, WeeChat) envoient `CAP LS 302` **avant PASS**.
Répondre : `CAP * LS :` puis `CAP * END`. Si `CAP END` reçu : ne rien faire.
Sans ça, certains clients restent bloqués au handshake.

##### `PASS`
- Vérifier que le client n'est pas encore authentifié.
- Stocker le mot de passe tenté dans `_password`.
- Erreurs : `461` si pas de paramètre, `462` si déjà enregistré.

##### `NICK`
- Vérifier que le nick n'est pas déjà pris (`getClientByNick`).
- Vérifier le format (lettres, chiffres, `_-[]{}|`).
- Si nick + user déjà reçus + password correct → envoyer `001 RPL_WELCOME` → marquer `_authenticated = true`.
- Erreurs : `431` si vide, `432` si format invalide, `433` si nick déjà pris.

##### `USER`
- Paramètres : `USER username 0 * :realname`
- Stocker `_username` et `_realname`.
- Même logique que NICK : si tout est complet → envoyer `001`.
- Erreur : `462` si déjà enregistré.

##### `QUIT`
```
sendToClient(fd, ":nick!user@host QUIT :message")
server.removeClientFromAllChannels(client)   // Person C implémente
server.disconnectClient(fd)                  // Person A implémente
```

#### 4. Commandes de messages

##### `PING` — CRITIQUE
```
Reçu  : PING :token
Réponse : PONG :token\r\n
```
Sans ça, **le client se déconnecte tout seul** après quelques secondes.

##### `PRIVMSG`
- `PRIVMSG nick :message` → trouver le client par nick → `sendToClient`.
- `PRIVMSG #channel :message` → trouver le channel → `channel.broadcast(msg, except=fd_émetteur)`.
- Erreurs : `401` si nick inconnu, `403` si channel inconnu, `404` si on ne peut pas envoyer, `442` si on n'est pas dans le salon.
- Format du message envoyé : `:nick!user@host PRIVMSG cible :message\r\n`

##### `NOTICE`
Même logique que PRIVMSG, **sans envoyer d'erreur** en cas d'échec.
Nécessaire car beaucoup de clients IRC l'utilisent automatiquement.

### Points d'attention
- **Ordre d'authentification** : PASS doit être reçu avant NICK/USER. Si un client envoie NICK sans PASS → `451 ERR_NOTREGISTERED`.
- **Toute commande** autre que CAP/PASS/NICK/USER reçue d'un client non authentifié doit renvoyer `451`.

---

## Personne C — Salons & Modes (La "Logique Métier")

**But :** gérer les groupes, les droits et les règles des salons.

### Fichiers

- `Channel.cpp` — implémentation de la classe Channel
- `Commands_channel.cpp` — JOIN, PART, TOPIC, KICK, INVITE
- `Commands_mode.cpp` — MODE

### La classe `Channel`

```cpp
class Channel {
    std::string _name;
    std::string _topic;
    std::string _key;          // mot de passe (+k)
    int         _userLimit;    // 0 = pas de limite (+l)
    bool        _inviteOnly;   // mode +i
    bool        _topicRestricted; // mode +t

    std::map<int, Client*> _members;   // fd → Client
    std::set<int>          _operators; // fds des opérateurs
    std::set<int>          _invited;   // fds invités (pour +i)
};
```

Méthodes clés :
```cpp
void    addMember(Client& c, bool isOp = false);
void    removeMember(int fd);
bool    isMember(int fd) const;
bool    isOperator(int fd) const;
void    broadcast(const std::string& msg, int except_fd = -1);
```

`broadcast` appelle `server.sendToClient(fd, msg)` pour chaque membre.

### Responsabilités

#### `JOIN`
```
Vérifications :
  - Client authentifié ?
  - Channel existe ? sinon le créer (client devient opérateur).
  - Mode +i : client dans _invited ?
  - Mode +k : mot de passe correct ?
  - Mode +l : nombre de membres < limite ?

Actions si OK :
  - addMember(client)
  - broadcast(":nick!u@h JOIN #channel")
  - envoyer 332/331 (topic)
  - envoyer 353 (liste des membres) + 366
```
Erreurs : `473` (+i), `475` (+k), `471` (+l), `403` (channel invalide).

#### `PART`
```
broadcast(":nick!u@h PART #channel :raison")
removeMember(fd)
si channel vide → supprimer le channel du serveur
```
Erreur : `442` si pas dans le salon.

#### `TOPIC`
- Sans paramètre : afficher le topic (`332`) ou "pas de topic" (`331`).
- Avec paramètre : si mode +t, vérifier que le client est opérateur.
- `broadcast(":nick!u@h TOPIC #channel :nouveau topic")`
- Erreur : `482` si pas opérateur et +t actif.

#### `KICK`
- Syntaxe : `KICK #channel nick :raison`
- Vérifier que l'émetteur est opérateur.
- `broadcast(":op!u@h KICK #channel nick :raison")`
- `removeMember(fd_victime)`
- Erreurs : `482` (pas opérateur), `441` (cible pas dans le salon), `403` (salon inconnu).

#### `INVITE`
- Syntaxe : `INVITE nick #channel`
- Vérifier que l'émetteur est opérateur (requis en mode +i).
- Ajouter `fd_cible` dans `_invited`.
- `sendToClient(fd_cible, ":op!u@h INVITE nick #channel")`
- Répondre `341 RPL_INVITING` à l'émetteur.
- Erreurs : `401` (nick inconnu), `443` (déjà dans le salon), `482` (pas opérateur).

#### `MODE`
Syntaxe : `MODE #channel +/-flags [paramètres]`

| Mode | Action | Paramètre |
|------|--------|-----------|
| `+i` / `-i` | Salon sur invitation uniquement | aucun |
| `+t` / `-t` | TOPIC réservé aux opérateurs | aucun |
| `+k key` / `-k` | Définir / supprimer le mot de passe | `key` (pour `+k`) |
| `+o nick` / `-o nick` | Donner / retirer le statut opérateur | `nick` |
| `+l N` / `-l` | Définir / supprimer la limite d'utilisateurs | `N` (pour `+l`) |

Après chaque changement :
```
broadcast(":op!u@h MODE #channel +flag [param]")
```
Erreurs : `482` (pas opérateur), `472` (mode inconnu), `461` (pas assez de paramètres).

### Points d'attention
- `removeClientFromAllChannels(Client& c)` : itérer sur tous les channels, appeler `removeMember(c.getFd())` et broadcaster le QUIT sur chaque salon où il était présent.
- Quand un salon se vide : le supprimer de `Server::_channels` (sinon fuite mémoire).
- Pour `MODE +o nick` : chercher le nick dans les membres du salon (`getClientByNick` du serveur).

---

## Flux de données global

```
Client TCP
    │  (octets fragmentés)
    ▼
Person A — poll() + recv()
    │  accumule dans _buffer, extrait lignes \r\n
    ▼
Person B — Parser + Dispatcher
    │
    ├─ commandes auth (PASS/NICK/USER/QUIT/CAP/PING) → Person B gère
    │
    └─ commandes salon (JOIN/PART/KICK/INVITE/TOPIC/MODE) → Person C gère
           │                    │
           │                    └─ Channel::broadcast()
           │                              │
           └──────────────────────────────▼
                               Person A — sendToClient() → poll() → send()
```

---

## Répartition des fichiers `.cpp` / `.hpp`

| Fichier | Personne |
|---------|----------|
| `Makefile` | Tous (Étape 0) |
| `main.cpp` | Tous (Étape 0) |
| `irc_replies.hpp` | Tous (Étape 0) |
| `Server.hpp` | Tous (Étape 0) |
| `Client.hpp` | Tous (Étape 0) |
| `Channel.hpp` | Tous (Étape 0, squelette) |
| `Server.cpp` | **Person A** |
| `Parser.cpp` | **Person B** |
| `Dispatcher.cpp` | **Person B** |
| `Commands_connection.cpp` | **Person B** |
| `Commands_messages.cpp` | **Person B** |
| `Channel.cpp` | **Person C** |
| `Commands_channel.cpp` | **Person C** |
| `Commands_mode.cpp` | **Person C** |

---

## Points de synchronisation entre personnes

| Situation | Person A fournit | Person B fournit | Person C fournit |
|-----------|-----------------|-----------------|-----------------|
| Person B code PRIVMSG #chan | `sendToClient()` | — | `Channel::broadcast()` |
| Person B code QUIT | `disconnectClient()` | — | `removeClientFromAllChannels()` |
| Person C code JOIN broadcast | `sendToClient()` | — | — |
| Person A lit une ligne | — | `dispatch(fd, line)` | — |

> Règle d'or : **si Person B ou C ne peut pas compiler car la méthode de l'autre n'est pas encore là**, écrire un stub vide dans le `.cpp` correspondant pour débloquer. On complète après.

---

## Checklist finale avant évaluation

- [ ] `./ircserv 6667 motdepasse` compile et démarre sans erreur
- [ ] Un client IRC (HexChat / WeeChat / irssi) se connecte sans erreur
- [ ] PASS + NICK + USER → réception du `001 RPL_WELCOME`
- [ ] PING automatique du client → réponse PONG du serveur (sinon le client se déco)
- [ ] JOIN #salon → le salon est créé, le créateur est opérateur
- [ ] PRIVMSG #salon :msg → reçu par tous les membres du salon
- [ ] PRIVMSG nick :msg → message privé entre deux clients
- [ ] KICK, INVITE, TOPIC, MODE (+i +t +k +o +l) fonctionnent
- [ ] Test données fragmentées : `nc -C 127.0.0.1 6667` avec Ctrl+D
- [ ] Pas de crash si un client se déconnecte brutalement
- [ ] Pas de crash si le serveur manque de mémoire
- [ ] `README.md` en anglais avec les sections obligatoires 42
