# Jeu de la vie (lifegame)

Automate cellulaire crée par [John Conway](https://fr.wikipedia.org/wiki/Jeu_de_la_vie).

### Règles 
1- Si une cellule à exactement 3 voisines, elle vie.<br>
2- Si une cellule à strictement moins de 2 ou plus 3 voisines, elle meurt.

### Fonctionnement
Le jeu fonctionne sous forme de génération, à chaque fois que l'on en passe une, le programme applique les règles sur les différentes cellules. On peut voir le nombre de générations écoulées en haut à droite de l'écran.

#### Exemple

#### Contrôles
- `Tab` : affiche les contrôles
- `Clique droit` : déplacement
- `Clique gauche` : placer/retirer une cellule
- `Espace` : play/pause
- `C` : Effacer toutes les cellules
- `Flèche haut` : accélère le temps
- `Flèche bas` : ralentit le temps
- `Echape` : quitte l'application

### Compilation & exécution
#### Version Linux (debug)
```
./rebuild.sh linux
```
Dans le répertoire `build_debug`, on retrouve l'exécutable de l'application.

#### Version Windows (release)
```
./rebuild.sh windows
```
Après la compilation dans le répertoire `build_release`, on retrouve l'exécutable et une archive contenant tout le nécessaire pour une exportation de l'application.
