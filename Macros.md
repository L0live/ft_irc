# IRC Response Macros

Ce fichier décrit les macros utilisées pour générer les réponses et erreurs dans un serveur IRC personnalisé.

## 🧩 Utilitaires de composition

- `CLIENT(nick, user)`  
  Concatène `nick` et `user` dans le format IRC standard :  
  `nick!user@localhost`

- `PRIVMSG(client, target, message)`  
  Génère une ligne de message privé :  
  `:<client> PRIVMSG <target> :<message>`

## ✅ Réponses IRC classiques

- `RPL_JOIN(client, channel)`  
  Indique qu’un client a rejoint un canal.

- `RPL_ENDOFNAMES(client, channel)`  
  Signale la fin de la liste des noms dans un canal (`366`).

- `RPL_PART(client, channel)`  
  Indique qu’un client a quitté un canal.

- `RPL_PARTMESSAGE(client, channel, message)`  
  Même que `RPL_PART`, mais avec un message personnalisé.

- `RPL_MODE(client, channel, mode, name)`  
  Réponse indiquant un changement de mode.

- `RPL_KICK(client, channel, target, reason)`  
  Un utilisateur a été expulsé d’un canal.

- `RPL_INVITERCVR(client, invitee, channel)`  
  Une invitation est envoyée à `invitee`.

- `RPL_NICK(oldNick, newNick)`  
  Un utilisateur change de pseudo.

- `RPL_TOPIC(client, channel, topic)`  
  Le sujet d’un canal est défini ou modifié.

- `RPL_QUIT(client, message)`  
  Un client quitte le serveur.

- `RPL_WELCOME(nickname, client)`  
  Message de bienvenue (`001`), après la connexion.

- `RPL_CHANNELMODEIS(client, channel, modes)`  
  Affiche les modes actuels d’un canal (`324`).

- `RPL_NOTOPIC(client, channel)`  
  Aucun sujet n'est défini pour le canal (`331`).

- `RPL_SEETOPIC(client, channel, topic)`  
  Affiche le sujet actuel d’un canal (`332`).

- `RPL_INVITESNDR(client, invitee, channel)`  
  Confirme qu’une invitation a été envoyée (`341`).

- `RPL_NAMEREPLY(nick, channel, nicknames)`  
  Liste des utilisateurs d’un canal (`353`).

## ❌ Messages d'erreur

- `ERR_TOOMUCHPARAMS(client, cmd)`  
  Trop de paramètres donnés.

- `ERR_USERONCHANNEL(nick, channel)`  
  L’utilisateur est déjà dans le canal (`443`).

- `ERR_NOSUCHNICK(client, nickname)`  
  Aucun utilisateur/canal avec ce nom (`401`).

- `ERR_NOSUCHNICKCHAN(server, client, nickname)`  
  Variante avec serveur précisé (`401`).

- `ERR_NOSUCHCHANNEL(client, channel)`  
  Le canal demandé n’existe pas (`403`).

- `ERR_CANNOTSENDTOCHAN(client, channel)`  
  L’envoi de message dans le canal est interdit (`404`).

- `ERR_NOTEXTTOSEND(client)`  
  Aucune donnée textuelle à envoyer (`412`).

- `ERR_UNKNOWNCOMMAND(client, command)`  
  La commande est inconnue (`421`).

- `ERR_NONICKNAMEGIVEN(client, nick)`  
  Aucun pseudo fourni (`431`).

- `ERR_ERRONEUSNICKNAME(client, nickname)`  
  Pseudo invalide (`432`).

- `ERR_NICKNAMEINUSE(client, nick)`  
  Le pseudo est déjà utilisé (`433`).

- `ERR_ERRONEUSUSERNAME(client, username)`  
  Nom d’utilisateur invalide (`432` bis).

- `ERR_USERNAMEINUSE(client, username)`  
  Nom d’utilisateur déjà pris (`433` bis).

- `ERR_NORECIPIENT(client, command)`  
  Aucun destinataire précisé pour la commande (`411`).

- `ERR_USERNOTINCHANNEL(client, nick, channel)`  
  L’utilisateur n’est pas dans le canal (`441`).

- `ERR_NOTONCHANNEL(client, channel)`  
  L’expéditeur n’est pas dans le canal (`442`).

- `ERR_NOTREGISTERED()`  
  Le client n’est pas encore enregistré (`451`).

- `ERR_NEEDMOREPARAMS(client, cmd)`  
  Paramètres insuffisants pour une commande (`461`).

- `ERR_ALREADYREGISTRED(client)`  
  Tentative de se réenregistrer (`462`).

- `ERR_PASSWDMISMATCH(client)`  
  Mot de passe incorrect (`464`).

- `ERR_KEYSET(channel)`  
  Une clé de canal est déjà définie (`467`).

- `ERR_CHANNELISFULL(client, channel)`  
  Le canal est plein (`471`).

- `ERR_UNKNOWNMODE(client, mode)`  
  Le mode demandé est inconnu (`472`).

- `ERR_INVITEONLYCHAN(client, channel)`  
  Canal en mode invitation seulement (`473`).

- `ERR_BADCHANNELKEY(client, channel)`  
  Mauvaise clé fournie pour le canal (`475`).

- `ERR_NOCHANMODES(channel)`  
  Le canal ne prend pas en charge les modes (`477`).

- `ERR_CHANOPRIVSNEEDED(client, channel)`  
  L’utilisateur n’est pas opérateur du canal (`482`).