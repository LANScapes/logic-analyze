# Mesures

Vous pouvez mesurer la forme d'onde avec la souris ou avec des curseurs. Un curseur est une ligne verticale à un instant de la capture.

## Mesurer une impulsion avec le pointeur

Placez le pointeur sur une impulsion d'une voie. Une zone près du pointeur affiche ces valeurs pour l'impulsion :

- **Largeur** : la durée de l'impulsion.
- **Période** : le temps entre un front et le front suivant de même sens.
- **Fréquence** : 1 divisé par la période.
- **Rapport cyclique** : le temps au niveau haut en pourcentage de la période.

![La mesure au pointeur](../figures/hover-measurement.png)

Pour afficher ou masquer cette zone, ouvrez le panneau de mesure et utilisez **Activer la mesure flottante**.

## Compter les fronts dans une zone

1. Placez le pointeur sur la forme d'onde de la voie, entre le niveau haut et le niveau bas.
2. Déplacez le pointeur au début de la zone.
3. Cliquez avec le bouton gauche de la souris.
4. Déplacez le pointeur à la fin de la zone. L'application affiche le nombre de fronts, de fronts montants et de fronts descendants.
5. Cliquez de nouveau avec le bouton gauche de la souris pour terminer la mesure.

## Mesurer le temps entre deux fronts

1. Placez le pointeur sur le premier front.
2. Cliquez avec le bouton gauche de la souris.
3. Déplacez le pointeur sur le deuxième front. L'application affiche le temps et le nombre d'échantillons entre les deux fronts.
4. Cliquez de nouveau avec le bouton gauche de la souris pour terminer la mesure.

![Le temps entre deux fronts](../figures/edge-distance.png)

## Ajouter un curseur

Utilisez une de ces méthodes :

- Dans la zone des formes d'onde, double-cliquez avec le bouton gauche de la souris à l'instant voulu. Si le pointeur est près d'un front, le curseur se place sur le front.
- Dans la règle de temps, cliquez avec le bouton gauche de la souris. Une flèche apparaît sur la règle. Cliquez sur la flèche pour ajouter un curseur.

![Ajouter un curseur depuis la règle de temps](../figures/ruler-insert-cursor.png)

Chaque curseur a un numéro. Les numéros commencent à 1.

## Déplacer un curseur

Utilisez une de ces méthodes :

- Placez le pointeur sur le curseur. La ligne du curseur devient plus épaisse. Cliquez sur le curseur. Déplacez la souris. Cliquez de nouveau pour relâcher le curseur. Près d'un front, le curseur se place sur le front.
- Dans la règle de temps, cliquez avec le bouton gauche de la souris au nouvel instant. La règle affiche les numéros de tous les curseurs. Cliquez sur le numéro du curseur à déplacer.

![Déplacer un curseur depuis la règle de temps](../figures/ruler-move-cursor.png)

## Aller à un curseur

1. Dans la règle de temps, cliquez avec le bouton droit de la souris. La règle affiche les numéros de tous les curseurs.
2. Cliquez sur le numéro d'un curseur. La forme d'onde se déplace à la position de ce curseur.

![Aller au curseur 3](../figures/ruler-jump-cursor.png)

## Mesurer avec des curseurs

Pour ouvrir le panneau de mesure, cliquez sur **Mesure** dans la barre d'outils ou appuyez sur `M`. Le panneau contient ces groupes :

- **Écart entre curseurs** : le temps et le nombre d'échantillons entre deux curseurs.
- **Fronts** : le nombre de fronts sur une voie entre deux curseurs.
- **Curseurs** : le temps et le numéro d'échantillon de chaque curseur.

Pour ajouter une mesure de temps, faites ces étapes :

1. Dans le groupe **Écart entre curseurs**, cliquez sur le bouton **+**.
2. Cliquez sur le champ de début et sélectionnez le premier curseur.
3. Cliquez sur le champ de fin et sélectionnez le deuxième curseur.

Le panneau affiche le résultat dans la colonne **Temps/échantillons**.

Pour ajouter un comptage de fronts, faites ces étapes :

1. Dans le groupe **Fronts**, cliquez sur le bouton **+**.
2. Sélectionnez le curseur de début et le curseur de fin.
3. Sélectionnez la voie.

Le panneau affiche le nombre de fronts montants, de fronts descendants et de tous les fronts.

Pour supprimer une mesure, cliquez sur le bouton **×** de sa ligne.

## Supprimer un curseur

Utilisez une de ces méthodes :

- Cliquez sur le **×** de l'étiquette du curseur dans la règle de temps.
- Cliquez sur le bouton **×** du curseur dans le groupe **Curseurs** du panneau de mesure.

L'application donne de nouveaux numéros aux curseurs qui restent.
