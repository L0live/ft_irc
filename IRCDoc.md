**RFC 1459 Ultra-Complet et Adapté pour ft_irc**

# 1. Introduction

## 1.1 Présentation du protocole IRC

IRC (Internet Relay Chat) est un protocole textuel permettant la communication en temps réel entre plusieurs utilisateurs via un réseau de serveurs. Il repose sur un modèle **client-serveur**, où les messages sont relayés par un serveur central.

IRC utilise le protocole **TCP/IP** et fonctionne généralement sur le port **6667**, bien que d'autres ports puissent être utilisés. Ce document fournit une **référence complète** pour l’implémentation d’un serveur IRC minimaliste dans le cadre du projet **ft_irc**, en détaillant **toutes les commandes, leurs usages, la gestion des erreurs et des mécaniques avancées**.

---

# 2. Architecture et Concepts Fondamentaux

## 2.1 Modèle Client-Serveur

- Les **clients** (utilisateurs) se connectent à un **serveur** via un socket TCP/IP.
- Le **serveur** est chargé de gérer les connexions, la transmission des messages et la synchronisation des canaux.
- Une architecture multi-serveurs peut exister (non nécessaire pour **ft_irc**), où plusieurs serveurs sont interconnectés en arbre.

## 2.2 Gestion des Connexions

- Les connexions sont initiées via une **requête TCP**.
- Une fois la connexion établie, le client doit **s’authentifier** en envoyant une séquence de commandes (`PASS`, `NICK`, `USER`).
- Le serveur maintient une liste des clients actifs et gère les déconnexions.

## 2.3 Identification et Nicknames

- Chaque client est identifié par un **nickname** unique.
- Un nickname est une chaîne alphanumérique de **9 caractères max**, sensible à la casse.
- Le serveur doit gérer **les conflits de pseudo** et les surnoms déjà attribués.

## 2.4 Canaux IRC

- Un **canal** est un espace de discussion identifié par un nom commençant par `#`.
- Un canal est **créé automatiquement** lorsqu'un utilisateur le rejoint pour la première fois et **disparaît** lorsqu'il est vide.
- Les canaux peuvent être **publics, privés, protégés par mot de passe, limités en nombre d’utilisateurs**, etc.

## 2.5 Modes des Utilisateurs et Canaux

Les modes permettent de modifier les permissions et règles de gestion des canaux.

Modes **Utilisateur** :

- `+i` : Invisible (non listé par la commande `WHO`)
- `+o` : Opérateur IRC (admin du serveur)

Modes **Canal** :

- `+o` : Donne le statut d’opérateur à un utilisateur
- `+i` : Invite-only (seuls les invités peuvent rejoindre)
- `+t` : Seuls les opérateurs peuvent modifier le topic
- `+m` : Canal modéré (seuls ceux ayant `+v` peuvent parler)
- `+b` : Ban d’un utilisateur
- `+l <nombre>` : Limite le nombre d’utilisateurs
- `+k <motdepasse>` : Protège le canal par mot de passe

---

# 3. Structure des Messages

Tous les messages envoyés et reçus sur IRC suivent un format strict :

```
:[préfixe] COMMANDE param1 param2 ... :<message>
```

- `[préfixe]` (optionnel) : identifie l'expéditeur (nick, serveur, etc.)
- `COMMANDE` : le type de message (ex : `JOIN`, `PRIVMSG`, `PING`, etc.)
- `param1, param2...` : liste des arguments
- `<message>` (optionnel) : partie texte du message

---

# 4. Commandes Essentielles et Gestion des Erreurs

## 4.1 Connexion et Identification

### PASS <password>

- **Usage** : Envoie un mot de passe pour l’authentification
- **Erreurs** :
  - `461 ERR_NEEDMOREPARAMS` : Paramètres manquants
  - `462 ERR_ALREADYREGISTRED` : Connexion déjà enregistrée

### NICK <nickname>

- **Usage** : Définit ou change le pseudo
- **Erreurs** :
  - `431 ERR_NONICKNAMEGIVEN` : Aucun pseudo fourni
  - `432 ERR_ERRONEUSNICKNAME` : Format invalide
  - `433 ERR_NICKNAMEINUSE` : Pseudo déjà pris
  - `436 ERR_NICKCOLLISION` : Conflit de pseudo

### USER <username> <hostname> <servername> <realname>

- **Usage** : Enregistre un utilisateur
- **Erreurs** :
  - `461 ERR_NEEDMOREPARAMS` : Paramètres manquants
  - `462 ERR_ALREADYREGISTRED` : Connexion déjà enregistrée

### QUIT [:message]

- **Usage** : Ferme la connexion du client

---

## 4.2 Gestion des Canaux

### JOIN <channel>

- **Usage** : Rejoindre un canal
- **Erreurs** :
  - `403 ERR_NOSUCHCHANNEL` : Canal inexistant
  - `405 ERR_TOOMANYCHANNELS` : Nombre maximal atteint
  - `474 ERR_BANNEDFROMCHAN` : Banni du canal

### PART <channel>

- **Usage** : Quitter un canal
- **Erreurs** :
  - `442 ERR_NOTONCHANNEL` : Pas dans le canal

### MODE <channel> <mode> [param]

- **Usage** : Modifier la configuration d'un canal

### KICK <channel> <user> [:reason]

- **Usage** : Expulse un utilisateur
- **Erreurs** :
  - `482 ERR_CHANOPRIVSNEEDED` : Permission insuffisante

---

## 4.3 Communication

### PRIVMSG <target> :<message>

- **Usage** : Envoi de messages privés ou à un canal
- **Erreurs** :
  - `401 ERR_NOSUCHNICK` : Utilisateur inexistant

### NOTICE <target> :<message>

- **Usage** : Message sans notification

---

# 5. Mécaniques Avancées

## 5.1 Gestion des Timeout et Keep-Alive

- Le serveur envoie périodiquement des `PING`
- Si aucun `PONG` n’est reçu en retour, le client est déconnecté

## 5.2 Protection contre le Flood

- Limiter le nombre de messages envoyés par seconde
- Détecter les spams et appliquer des bans automatiques

## 5.3 Logs et Surveillance

- Stocker les logs des connexions, erreurs et actions critiques

## 5.4 Gestion des Déconnexions Inattendues

- Détection des **netsplits** (perte de connexion entre serveurs)
- Synchronisation des canaux après une reconnexion

---

# 6. Conclusion

Ce document constitue une **référence complète et détaillée** pour l’implémentation d’un serveur IRC dans **ft_irc**. Il couvre **toutes les commandes essentielles, la gestion des erreurs, les mécaniques avancées et les bonnes pratiques** pour assurer un développement robuste et conforme aux standards du protocole IRC.

#define CLIENT(nick, user)							(std::string(nick) + "!" + std::string(user) + "@localhost")
#define PRIVMSG(client, target, message)			(":" + client + " PRIVMSG " + target + " :" + message + "\r\n")

#define RPL_JOIN(client, channel)					(":" + std::string(client) + " JOIN " + std::string(channel) + std::string("\r\n"))
#define RPL_ENDOFNAMES(client, channel)				(": 366 " + client + " " + channel + " :End of /NAMES list.\r\n")
#define RPL_PART(client, channel)					(":" + client + " PART " + channel + "\r\n")
#define RPL_PARTMESSAGE(client, channel, message)	(":" + client + " PART " + channel + " :" + message +"\r\n")
#define RPL_MODE(client, channel, mode, name)		(":" + client + " MODE " + channel + " " + mode + " " + name + "\r\n")
#define RPL_KICK(client, channel, target, reason)	(":" + client + " KICK " + channel + " " + target + " " + reason + "\r\n")
#define RPL_INVITERCVR(client, invitee, channel)	(":" + client + " INVITE " + invitee + " " + channel + "\r\n")
#define RPL_NICK(oldNick, newNick)					(":" + oldNick + " NICK " + newNick + "\r\n")
#define RPL_TOPIC(client, channel, topic)			(":" + client + " TOPIC " + channel + " :" + topic + "\r\n")
#define RPL_QUIT(client, message)					(":" + client + " QUIT :" + message + "\r\n")
#define RPL_WELCOME(nickname, client)				(": 001 " + nickname + " :Welcome to the IRC world, " + client + "\r\n")
#define RPL_CHANNELMODEIS(client, channel, modes) 	(": 324 " + client + " " + channel + " " + modes + "\r\n")
#define RPL_NOTOPIC(client, channel)				(": 331 " + client + " " + channel + " :No topic is set\r\n")
#define RPL_SEETOPIC(client, channel, topic)		(": 332 " + client + " " + channel + " :" + topic + "\r\n")
#define RPL_INVITESNDR(client, invitee, channel)	(": 341 " + client + " " + invitee + " " + channel + "\r\n")
#define RPL_NAMEREPLY(nick, channel, nicknames)		(": 353 " + nick + " = " + channel + " :" + nicknames + "\r\n")

#define ERR_TOOMUCHPARAMS(client, cmd)				(client + " " + cmd + " :Too much parameters\r\n")
#define ERR_USERONCHANNEL(nick, channel)			(": 443 " + nick + " " + channel + " :is already on channel\r\n")
#define ERR_NOSUCHNICK(client, nickname) 			(": 401 " + client + " " + nickname + " :No such nick/channel\r\n")
#define ERR_NOSUCHNICKCHAN(server, client, nickname)(":" + server + " 401 " + client + " " + nickname + " :No such nick/channel\r\n")
#define ERR_NOSUCHCHANNEL(client, channel)			(": 403 " + client + " " + channel + " :No such channel\r\n")
#define ERR_CANNOTSENDTOCHAN(client, channel)		(": 404 " + client + " " + channel + " :Cannot send to channel\r\n")
#define ERR_NOTEXTTOSEND(client)					(": 412 " + client + " :No text to send\r\n")

#define ERR_UNKNOWNCOMMAND(client, command)			(": 421 " + client + " " + command + " :Unknown command\r\n")

#define ERR_NONICKNAMEGIVEN(client, nick)			(": 431 " + client + " " + nick + " :No nickname given\r\n")

#define ERR_ERRONEUSNICKNAME(client, nickname)		(": 432 " + client + " " + nickname + " :Erroneus nickname\r\n")
#define ERR_NICKNAMEINUSE(client, nick)				(": 433 * " + nick + " :Nickname is already in use\r\n")
#define ERR_ERRONEUSUSERNAME(client, username)		(": 432 " + client + " " + username + " :Erroneus username\r\n")
#define ERR_USERNAMEINUSE(client, username)			(": 433 * " + client + " " + username + " :Username is already in use\r\n")
#define ERR_NORECIPIENT(client, command)			(": 411 " + client + " :No recipient given (" + command + ")\r\n")

#define ERR_USERNOTINCHANNEL(client, nick, channel)	(": 441 " + client + " " + nick + " " + channel + " :They aren't on that channel\r\n")
#define ERR_NOTONCHANNEL(client, channel)			(": 442 " + client + " " + channel + " :You're not on that channel\r\n")
#define ERR_NOTREGISTERED()							(": 451 :You have not registered\r\n")
#define ERR_NEEDMOREPARAMS(client, cmd)				(": 461 " + client + " " + cmd + " :Not enough parameters\r\n")
#define ERR_ALREADYREGISTRED(client)				(": 462 " + client + " ::Unauthorized command (already registered)\r\n")
#define ERR_PASSWDMISMATCH(client)					(": 464 " + client + " :Password incorrect\r\n")
#define ERR_KEYSET(channel)							(": 467 " + channel + " :Channel key already set\r\n")
#define ERR_CHANNELISFULL(client, channel)			(": 471 " + client + " " + channel + " :Cannot join channel (+l)\r\n")
#define ERR_UNKNOWNMODE(client, mode)				(": 472 " + client + " " + mode + " :is unknown mode char to me\r\n")
#define ERR_INVITEONLYCHAN(client, channel)			(": 473 " + client + " " + channel + " :Cannot join channel (+i)\r\n")
#define ERR_BADCHANNELKEY(client, channel)			(": 475 " + client + " " + channel + " :Cannot join channel (+k)\r\n")
#define ERR_NOCHANMODES(channel)					(": 477 " + channel + " :Channel doesn't support modes\r\n")
#define ERR_CHANOPRIVSNEEDED(client, channel)		(": 482 " + client + " " + channel + " :You're not channel operator\r\n")
