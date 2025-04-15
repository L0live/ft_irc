# Mini-Manuel des Fonctions Autorisées pour ft_irc

Ce document liste **toutes les fonctions externes autorisées** dans le projet ft_irc, avec une description détaillée de leur rôle, de leurs paramètres et de leur valeur de retour.

---

## 1. Fonctions Réseau

### `socket(int domain, int type, int protocol)`

- **Description** : Crée un socket pour la communication réseau.
- **Paramètres** :
  - **`domain`** : Type d’adresse utilisé.
    - `AF_INET` → IPv4.
    - `AF_INET6` → IPv6.
  - **`type`** : Type de socket.
    - `SOCK_STREAM` → TCP (connexion fiable).
    - `SOCK_DGRAM` → UDP (connexion sans garantie).
  - **`protocol`** : Protocole spécifique à utiliser (0 pour sélectionner le protocole par défaut pour TCP/UDP).
- **Retour** : Descripteur de socket (`int`) en cas de succès, `-1` en cas d’erreur.

---

### `close(int sockfd)`

- **Description** : Ferme un socket ou un descripteur de fichier.
- **Paramètres** :
  - **`sockfd`** : Descripteur du socket ou fichier à fermer.
- **Retour** : `0` en cas de succès, `-1` en cas d’échec.

---

### `setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen)`

- **Description** : Configure une option du socket.
- **Paramètres** :
  - **`sockfd`** : Socket concerné.
  - **`level`** : Niveau auquel l’option est définie (par exemple, `SOL_SOCKET`).
  - **`optname`** : Nom de l’option à configurer (par exemple, `SO_REUSEADDR`, `SO_KEEPALIVE`, etc.).
  - **`optval`** : Pointeur vers la valeur de l’option.
  - **`optlen`** : Taille en octets de la valeur pointée par `optval`.
- **Retour** : `0` en cas de succès, `-1` sinon.

---

### `bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)`

- **Description** : Associe un socket à une adresse (et à un port).
- **Paramètres** :
  - **`sockfd`** : Descripteur du socket.
  - **`addr`** : Pointeur vers une structure `sockaddr` contenant l’adresse et le port.
  - **`addrlen`** : Taille de la structure pointée par `addr`.
- **Retour** : `0` en cas de succès, `-1` en cas d’échec.

---

### `connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen)`

- **Description** : Établit une connexion depuis un socket client vers un serveur.
- **Paramètres** :
  - **`sockfd`** : Descripteur du socket client.
  - **`addr`** : Pointeur vers une structure `sockaddr` contenant l’adresse du serveur.
  - **`addrlen`** : Taille de la structure `addr`.
- **Retour** : `0` en cas de succès, `-1` en cas d’erreur.

---

### `listen(int sockfd, int backlog)`

- **Description** : Place le socket en mode écoute, afin d’accepter des connexions entrantes.
- **Paramètres** :
  - **`sockfd`** : Descripteur du socket.
  - **`backlog`** : Nombre maximal de connexions en attente.
- **Retour** : `0` en cas de succès, `-1` sinon.

---

### `accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)`

- **Description** : Accepte une connexion entrante sur un socket en écoute.
- **Paramètres** :
  - **`sockfd`** : Descripteur du socket en écoute.
  - **`addr`** : Pointeur vers une structure `sockaddr` où sera stockée l’adresse du client.
  - **`addrlen`** : Pointeur sur la taille de la structure `addr` (sera mis à jour avec la taille réelle de l’adresse du client).
- **Retour** : Un nouveau descripteur de socket pour la connexion établie, ou `-1` en cas d’erreur.

---

### `getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen)`

- **Description** : Récupère l’adresse locale (adresse et port) associée à un socket.
- **Paramètres** :
  - **`sockfd`** : Descripteur du socket.
  - **`addr`** : Pointeur vers une structure `sockaddr` où sera stockée l’adresse.
  - **`addrlen`** : Pointeur sur la taille de la structure `addr`; mis à jour avec la taille réelle.
- **Retour** : `0` en cas de succès, `-1` sinon.

---

### `getprotobyname(const char *name)`

- **Description** : Récupère les informations relatives à un protocole réseau à partir de son nom.
- **Paramètres** :
  - **`name`** : Chaîne de caractères contenant le nom du protocole (par exemple, `"tcp"` ou `"udp"`).
- **Retour** : Pointeur vers une structure `protoent` contenant les informations sur le protocole, ou `NULL` en cas d’échec.

---

### `gethostbyname(const char *name)`

- **Description** : Traduit un nom d’hôte en adresse IP.
- **Paramètres** :
  - **`name`** : Chaîne contenant le nom de l’hôte (exemple : `"irc.example.com"`).
- **Retour** : Pointeur vers une structure `hostent` contenant l’adresse IP (et d’autres informations), ou `NULL` en cas d’échec.  
- **Remarque** : Fonction obsolète mais encore utilisable en C++98.

---

### `getaddrinfo(const char *node, const char *service, const struct addrinfo *hints, struct addrinfo **res)`

- **Description** : Résout un nom d’hôte et/ou un service en une ou plusieurs structures d’adresse utilisables.
- **Paramètres** :
  - **`node`** : Nom d’hôte ou adresse IP (exemple : `"localhost"` ou `"192.168.1.1"`).
  - **`service`** : Nom du service ou numéro de port sous forme de chaîne (exemple : `"6667"` pour IRC).
  - **`hints`** : Pointeur vers une structure `addrinfo` servant à filtrer les résultats (peut être `NULL`).
  - **`res`** : Pointeur vers une variable où sera stockée l’adresse de la liste chaînée des résultats.
- **Retour** : `0` en cas de succès, ou un code d’erreur en cas d’échec.

---

### `freeaddrinfo(struct addrinfo *res)`

- **Description** : Libère la mémoire allouée pour la liste d’adresses obtenue avec `getaddrinfo`.
- **Paramètres** :
  - **`res`** : Pointeur vers la liste chaînée à libérer.
- **Retour** : Aucun.

---

### `inet_addr(const char *cp)`

- **Description** : Convertit une adresse IPv4 en notation décimale pointée (exemple : `"192.168.1.1"`) en une valeur de type `in_addr_t` (entier 32 bits en format réseau).
- **Paramètres** :
  - **`cp`** : Chaîne contenant l’adresse IPv4.
- **Retour** : Valeur de l’adresse en format réseau, ou `INADDR_NONE` en cas d’échec.

---

### `inet_ntoa(struct in_addr in)`

- **Description** : Convertit une adresse IPv4 (contenue dans une structure `in_addr`) en une chaîne de caractères lisible.
- **Paramètres** :
  - **`in`** : Structure `in_addr` contenant l’adresse IPv4.
- **Retour** : Pointeur vers une chaîne statique contenant l’adresse IPv4 sous forme textuelle.

---

### Fonctions de conversion d’entiers (ordre hôte / réseau)

#### `htons(uint16_t hostshort)` et `htonl(uint32_t hostlong)`

- **Description** : Convertissent un entier de l’ordre hôte vers l’ordre réseau (big-endian).
- **Paramètres** :
  - **`hostshort` / `hostlong`** : Entier à convertir.
- **Retour** : Entier converti en format réseau.

---

#### `ntohs(uint16_t netshort)` et `ntohl(uint32_t netlong)`

- **Description** : Convertissent un entier de l’ordre réseau vers l’ordre hôte.
- **Paramètres** :
  - **`netshort` / `netlong`** : Entier à convertir.
- **Retour** : Entier converti en format hôte.

---

## 2. Fonctions d'Entrée/Sortie (E/S)

### `send(int sockfd, const void *buf, size_t len, int flags)`

- **Description** : Envoie des données via un socket.
- **Paramètres** :
  - **`sockfd`** : Descripteur du socket sur lequel envoyer les données.
  - **`buf`** : Pointeur vers le buffer contenant les données à envoyer.
  - **`len`** : Taille, en octets, des données à envoyer.
  - **`flags`** : Options d’envoi (souvent `0`).
- **Retour** : Nombre d’octets effectivement envoyés, ou `-1` en cas d’erreur.

---

### `recv(int sockfd, void *buf, size_t len, int flags)`

- **Description** : Reçoit des données depuis un socket.
- **Paramètres** :
  - **`sockfd`** : Descripteur du socket depuis lequel recevoir les données.
  - **`buf`** : Pointeur vers le buffer où stocker les données reçues.
  - **`len`** : Taille, en octets, du buffer.
  - **`flags`** : Options de réception (souvent `0`).
- **Retour** : Nombre d’octets reçus, ou `-1` en cas d’erreur.

---

## 3. Gestion des Signaux

### `signal(int signum, void (*handler)(int))`

- **Description** : Associe un signal à un gestionnaire (fonction de rappel).
- **Paramètres** :
  - **`signum`** : Numéro du signal (exemple : `SIGINT`, `SIGTERM`, etc.).
  - **`handler`** : Pointeur vers la fonction qui sera appelée lors de la réception du signal.
- **Retour** : Ancien gestionnaire de signal, ou `SIG_ERR` en cas d’échec.

---

### `sigaction(int signum, const struct sigaction *act, struct sigaction *oldact)`

- **Description** : Configure de manière plus fine le comportement d’un signal.
- **Paramètres** :
  - **`signum`** : Numéro du signal à configurer.
  - **`act`** : Pointeur vers une structure `sigaction` définissant la nouvelle action (gestionnaire, flags, etc.).  
  - **`oldact`** : Pointeur vers une structure `sigaction` où sera stockée l’ancienne action (peut être `NULL` si non utilisé).
- **Retour** : `0` en cas de succès, ou `-1` en cas d’erreur.

---

## 4. Gestion de Fichiers

### `lseek(int fd, off_t offset, int whence)`

- **Description** : Déplace le pointeur de lecture/écriture dans un fichier.
- **Paramètres** :
  - **`fd`** : Descripteur du fichier.
  - **`offset`** : Déplacement en octets par rapport à la position de référence.
  - **`whence`** : Indicateur de la position de référence :
    - `SEEK_SET` : Début du fichier.
    - `SEEK_CUR` : Position actuelle.
    - `SEEK_END` : Fin du fichier.
- **Retour** : Nouvelle position dans le fichier en cas de succès, ou `-1` en cas d’erreur.

---

### `fstat(int fd, struct stat *buf)`

- **Description** : Récupère les informations sur un fichier ouvert.
- **Paramètres** :
  - **`fd`** : Descripteur du fichier.
  - **`buf`** : Pointeur vers une structure `stat` qui sera remplie avec les informations du fichier.
- **Retour** : `0` en cas de succès, ou `-1` en cas d’erreur.

---

### `fcntl(int fd, int cmd, ...)`

- **Description** : Manipule les propriétés et les options d’un descripteur de fichier.
- **Paramètres** :
  - **`fd`** : Descripteur du fichier.
  - **`cmd`** : Commande à appliquer (par exemple, `F_SETFL` pour modifier les flags).
  - **`...`** : Argument additionnel selon la commande (exemple typique : `O_NONBLOCK` pour rendre le descripteur non bloquant).
- **Retour** : Dépend de la commande utilisée (souvent `0` ou une valeur spécifique en cas de succès, ou `-1` en cas d’erreur).

---

## 5. Gestion des E/S Asynchrones

### `poll(struct pollfd *fds, nfds_t nfds, int timeout)`

- **Description** : Surveille un ensemble de descripteurs de fichiers pour détecter l’apparition d’événements (lecture, écriture, etc.).
- **Paramètres** :
  - **`fds`** : Pointeur vers un tableau de structures `pollfd`, chacune décrivant un descripteur à surveiller et les événements d’intérêt.
  - **`nfds`** : Nombre d’éléments (descripteurs) dans le tableau `fds`.
  - **`timeout`** : Durée maximale d’attente en millisecondes :
    - `-1` : Attente infinie.
    - `0` : Vérification immédiate (non bloquant).
- **Retour** :
  - Nombre de descripteurs pour lesquels un événement a été détecté (supérieur à 0),
  - `0` si le délai est expiré sans événement,
  - `-1` en cas d’erreur.

## Remarque sur les alternatives

Bien que le sujet mentionne **poll()**, il est également autorisé d’utiliser des fonctions équivalentes telles que `select()`, `kqueue()`, ou `epoll()` pour gérer les E/S asynchrones, à condition de respecter les consignes du projet.

## Fonctions Équivalentes à `poll()`

### `select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout)`

- **Description** : Surveille un ensemble de descripteurs de fichiers pour détecter l’apparition d’événements (lecture, écriture, erreurs).
- **Paramètres** :
  - **`nfds`** : Nombre maximal de descripteurs surveillés +1.
  - **`readfds`** : Pointeur vers un ensemble de descripteurs surveillés pour la lecture (peut être `NULL`).
  - **`writefds`** : Pointeur vers un ensemble de descripteurs surveillés pour l’écriture (peut être `NULL`).
  - **`exceptfds`** : Pointeur vers un ensemble de descripteurs surveillés pour les erreurs (peut être `NULL`).
  - **`timeout`** : Durée maximale d’attente :
    - `NULL` : Attente infinie.
    - Structure contenant `tv_sec` (secondes) et `tv_usec` (microsecondes).
- **Retour** :
  - Nombre de descripteurs prêts en cas de succès.
  - `0` si le délai est expiré sans événement.
  - `-1` en cas d’erreur.

---

### `int kqueue(void)`

- **Description** : Crée une file d’attente d’événements pour surveiller des descripteurs de fichiers (uniquement sur BSD/macOS).
- **Paramètres** :
  - Aucun.
- **Retour** :
  - Un descripteur de file d’événements (`int`) en cas de succès.
  - `-1` en cas d’échec.

---

### `int epoll_create(int size)`

- **Description** : Crée une instance `epoll` pour surveiller des descripteurs de fichiers (uniquement sur Linux).
- **Paramètres** :
  - **`size`** : Indice initial du nombre de descripteurs surveillés (ignoré dans les versions récentes).
- **Retour** :
  - Un descripteur `epoll` (`int`) en cas de succès.
  - `-1` en cas d’erreur.
