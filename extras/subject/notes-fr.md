port : Le numéro de port sur lequel votre serveur IRC écoutera les connexions IRC entrantes.  


password : Le mot de passe de connexion. Il sera nécessaire pour tout client IRC qui tente de se connecter à votre serveur.  

Bien que poll() soit mentionné dans le sujet et le barème d'évaluation, vous pouvez utiliser n'importe quel équivalent tel que select(), kqueue(), ou epoll().

Spécifications techniques
Lancement : ./ircserv <port> <password>. 
Protocole : La communication doit se faire via TCP/IP (v4 ou v6). 
Gestion des clients : Le serveur doit gérer plusieurs clients simultanément sans bloquer.  
I/O Non-bloquantes : Le "forking" est interdit. 
Toutes les opérations d'entrée/sortie doivent être non-bloquantes.  
Multiplexage : Un seul poll() (ou équivalent comme select, kqueue, epoll) doit être utilisé pour toutes les opérations (lire, écrire, écouter, etc.).  
Consommation de ressources : Toute tentative de read/recv ou write/send sans utiliser poll() (ou équivalent) entraînera une note de 0.  
Fonctionnalités requises
Vous devez implémenter les fonctionnalités suivantes avec un client de référence de votre choix:  
    S'authentifier, définir un pseudonyme (nickname), un nom d'utilisateur (username).  
    Rejoindre un channel (join) et envoyer/recevoir des messages privés.  
    Tous les messages envoyés à un channel doivent être transmis à tous les membres de ce channel.  
    Distinction entre opérateurs et utilisateurs réguliers.  Commandes opérateurs :
        KICK : Expulser un client du channel.  
        INVITE : Inviter un client sur un channel.  
        TOPIC : Modifier ou afficher le sujet du channel.  MODE : Gérer les modes du channel :
            i : channel sur invitation uniquement.  
            t : Restriction de la commande TOPIC aux opérateurs.  
            k : Définir/supprimer le mot de passe du channel.  
            o : Donner/retirer le privilège d'opérateur.  
            l : Définir/supprimer la limite d'utilisateurs du channel.  

MacOS et TestsMacOS : Vous pouvez utiliser fcntl(fd, F_SETFL, O_NONBLOCK) pour obtenir un comportement similaire aux autres UNIX.  
Test de données partielles : Votre serveur doit être capable d'agréger des paquets reçus par morceaux (ex: via nc -C) avant de traiter une commande