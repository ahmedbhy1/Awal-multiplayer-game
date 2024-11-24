
# **Projet : Jeu Awalé en Réseau**

## **Participants**
- Ahmed BEL HADJ YOUSSEF
- Mateo CARVAJAL SANCHEZ

## **Introduction à l'Awalé**
L'Awalé est un jeu de société traditionnel africain appartenant à la famille des jeux de semailles. Il se joue sur un plateau comportant deux rangées de six trous appelés "maisons", avec un certain nombre de graines réparties dans ces maisons. 

### **Objectif du jeu :**
Le but est de collecter plus de graines que son adversaire. Les joueurs, à tour de rôle, choisissent une maison de leur côté du plateau et distribuent ses graines dans les maisons suivantes, selon un mouvement circulaire. Lorsqu'un joueur "sème" les graines, il peut capturer celles de l'adversaire sous certaines conditions. L'Awalé est à la fois un jeu de stratégie et de réflexion.

Pour plus d'informations, vous pouvez consulter l'article détaillé sur l'Awalé :  
[https://fr.wikipedia.org/wiki/Awal%C3%A9](https://fr.wikipedia.org/wiki/Awal%C3%A9)

Ce projet implémente l'Awalé dans un environnement client-serveur, permettant à deux joueurs de s'affronter en ligne ou à d'autres utilisateurs de rejoindre les parties en tant qu'observateurs.

---

## **Structure du Projet**

Le projet est organisé dans les dossiers suivants :
- **Client :** Contient le code source pour le client.  
- **Serveur :** Contient le code source pour le serveur.  

---

## **Fonctionnalités du Client**

### **Commandes Disponibles**
1. **`commands`**  
   Affiche la liste des commandes disponibles pour le client.  
   - **Commande :** `commands`  

2. **`getPlayersList`**  
   Permet de voir la liste des joueurs actuellement connectés au serveur.  
   - **Commande :** `1`  

3. **`request Starting Game with Player`**  
   Permet de demander à un autre joueur de commencer une partie.  
   - **Commande :** `2 [NomPlayer]`  
   - Exemple : `2 Ahmed`

4. **`get The list of active games`**  
   Affiche une liste des jeux actifs et des joueurs impliqués dans ces parties.  
   - **Commande :** `ag`  

5. **`reject/accept game`**  
   Permet au joueur de rejeter ou d'accepter une demande de jeu. Si accepté, la partie commence.  
   - **Commande :** Lorsqu'une demande est reçue, tapez `y` pour accepter ou `n` pour refuser.  

6. **`join game as an observer`**  
   Permet à l'utilisateur de rejoindre une partie active en tant qu'observateur.  
   - **Commande :** `o [index du jeu]`  
   - Exemple : `o 1`  

7. **`play turn Awalé`**  
   Permet au joueur de sélectionner une "maison" (case) pour jouer son tour dans le jeu Awalé.  
   - **Commande :** `p [numéro de la maison (1-6)]`  
   - Exemple : `p 3`  
   - **Note :** Les "maisons" contenant 0 graines ne peuvent pas être choisies.

8. **quitter la Partie**  
   Permet à l'observateur d'arreter de regarder une partie active.  
   - **Commande :** `q`  
   - **Usage :** Tapez `q` dans la terminal et appuyez sur **Entrée** pour quitter.  

9. **abandonner la Partie**  
   Permet à un joueur d'abondonner une partie active.  
   - **Commande :** `sr`  
   - **Usage :** Tapez `sr` dans la terminal et appuyez sur **Entrée** pour abandonner.  

10. **Reconnexion Joueur ou Observateur**  
   Si un joueur est déconnecté pendant une partie, il peut se reconnecter et retrouver la partie active en cours, à condition que l'autre joueur soit toujours connecté.  
   - **Commande pour la reconnexion :**  
     ```bash
     client [ipAddress] [NomPlayer]
     ```  
   - **Note :** Le joueur doit utiliser le même nom (**NomPlayer**) qu'avant la déconnexion.





---

## **Fonctionnalités du Serveur**

1. **Gestion des Connexions :**  
   Gère plusieurs connexions simultanées pour les joueurs et les observateurs.  

2. **Gestion des Jeux Actifs :**  
   Gère et met à jour en temps réel l'état des parties actives, y compris les mouvements des joueurs et les connexions des observateurs.  

3. **Validation des Mouvements :**  
   Vérifie que les mouvements effectués par les joueurs sont valides et conformes aux règles du jeu Awalé.  

4. **Support pour les Observateurs :**  
   Permet aux utilisateurs de suivre les parties en temps réel sans participer.  

5. **Messages d'Erreur et d'État :**  
   Gère les erreurs, telles que des commandes invalides ou des mouvements non autorisés, et fournit des messages clairs aux utilisateurs.  

6. **Restrictions du Nom**  
    - Le nom d'un client doit être unique. Si un joueur ou observateur tente de se connecter avec un nom déjà utilisé par un client actif, le serveur refusera la connexion.  
---

## **Instructions de Compilation**

1. **Extraire les Fichiers :**  
   Extrayez le contenu de l'archive `.zip`.  

2. **Compiler le Serveur :**  
   Accédez au dossier `Serveur` et exécutez :  
   ```bash
   gcc playerState.c game.c server2.c -o server
   ```  

3. **Compiler le Client :**  
   Accédez au dossier `Client` et exécutez :  
   ```bash
   gcc client2.c -o client
   ```  

---

## **Instructions d'Exécution**

1. **Lancer le Serveur :**  
   ```bash
   ./server
   ```  

2. **Lancer le Client :**  
   ```bash
   ./client [ipAddress] [NomPlayer]
   ```  
   - **ipAddress :** Adresse IP du serveur.  
   - **NomPlayer :** Nom du joueur ou de l'observateur.  

---

## **Notes Supplémentaires**

- Les joueurs et observateurs doivent utiliser le même nom (**NomPlayer**) pour se reconnecter après une déconnexion.  
- Les mises à jour des parties actives et des plateaux sont reflétées en temps réel, mais les observateurs doivent attendre qu'un joueur effectue un mouvement pour voir le plateau actualisé.  
- Le serveur doit être lancé avant les clients.  

---
