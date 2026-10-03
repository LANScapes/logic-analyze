# Déclenchements

Un déclenchement est une condition dans les signaux. Quand la condition se produit, l'appareil marque cet instant comme point de déclenchement. Le déclenchement vous permet de capturer la partie du signal que vous voulez examiner.

L'application a deux types de déclenchement :

- **Déclenchement simple** : Un front ou un niveau sur une ou plusieurs voies.
- **Déclenchement avancé** : Une suite de conditions ou une valeur sur un bus série.

Pour ouvrir le panneau de déclenchement, cliquez sur **Déclench.** dans la barre d'outils ou appuyez sur `T`.

> [!NOTE]
> Si le signal ne correspond pas à la condition de déclenchement, la capture continue d'attendre. Pour voir le signal sans le déclenchement, cliquez sur **Instant.**. Pour arrêter l'attente, cliquez sur **Arrêter**.

## Position du déclenchement

Le paramètre **Position du déclenchement** définit l'emplacement du point de déclenchement dans la capture. La valeur est un pourcentage de la durée d'échantillonnage.

- Une petite valeur, par exemple 10 %, affiche une plus grande partie du signal après le déclenchement.
- Une grande valeur, par exemple 90 %, affiche une plus grande partie du signal avant le déclenchement.

La position du déclenchement utilise la mémoire de l'appareil. Vous pouvez donc la régler uniquement en mode tampon. En mode flux, la position du déclenchement est toujours d'environ 1 %.

![Position du déclenchement 10 % (à gauche) et 90 % (à droite)](../figures/trigger-position.png)
<!-- TODO: new screenshot -->

## Déclenchement simple

Chaque étiquette de voie de la zone des formes d'onde a cinq boutons de déclenchement. De gauche à droite, les boutons sont :

1. Front montant
2. Niveau haut
3. Front descendant
4. Niveau bas
5. Front montant ou front descendant

![Les boutons de déclenchement sur une étiquette de voie](../figures/simple-trigger-buttons.png)

Pour régler un déclenchement simple, faites ces étapes :

1. Ouvrez le panneau de déclenchement.
2. Sélectionnez **Déclenchement simple**.
3. Sur l'étiquette d'une voie, cliquez sur le bouton de déclenchement voulu. Le bouton change de couleur.
4. Pour supprimer le déclenchement d'une voie, cliquez de nouveau sur le même bouton.
5. Réglez la **Position du déclenchement**.

Si vous réglez un déclenchement sur plusieurs voies, toutes les conditions doivent se produire sur le même échantillon (ET logique).

## Déclenchement avancé

> [!NOTE]
> Le déclenchement avancé est disponible uniquement en mode tampon. Pour l'utiliser, réglez **Mode de fonctionnement** sur **Mode tampon**. Consultez [Options de l'appareil](05-device-options.md).

Pour utiliser le déclenchement avancé, sélectionnez **Déclenchement avancé** dans le panneau de déclenchement. Ensuite, sélectionnez l'onglet **Déclenchement par étapes** ou l'onglet **Déclenchement série**.

### Valeurs pour chaque voie

Le déclenchement par étapes et le déclenchement série utilisent une ligne de 16 caractères. Chaque caractère est la condition d'une voie. Le caractère de droite est la voie 0. Le caractère de gauche est la voie 15.

| Caractère | Condition |
| --- | --- |
| `X` | Toutes les valeurs (la voie n'a pas d'effet). |
| `0` | Niveau bas. |
| `1` | Niveau haut. |
| `R` | Front montant. |
| `F` | Front descendant. |
| `C` | Front montant ou front descendant. |

### Déclenchement par étapes

Un déclenchement par étapes est une suite de conditions. Chaque condition est une étape. L'appareil examine d'abord l'étape 0. Quand la condition d'une étape se produit, l'appareil passe à l'étape suivante. Le déclenchement se produit quand la dernière étape est terminée. Vous pouvez utiliser jusqu'à 16 étapes.

Chaque étape a ces paramètres :

- Deux lignes de conditions de voie.
- Pour chaque ligne, `==` ou `!=`. Avec `==`, la condition se produit quand les voies correspondent à la ligne. Avec `!=`, la condition se produit quand les voies ne correspondent pas à la ligne.
- **Et** ou **Ou**. Ce paramètre relie les deux lignes.
- **Compteur** : Le nombre de fois que la condition doit se produire avant la fin de l'étape.
- **Contigu** : Quand vous cochez cette case, la condition doit se produire sur des échantillons qui se suivent sans interruption.

![Les paramètres du déclenchement par étapes](../figures/stage-trigger-panel.png)
<!-- TODO: new screenshot -->

Pour régler un déclenchement par étapes, faites ces étapes :

1. Dans **Nombre d'étapes de déclenchement**, sélectionnez le nombre d'étapes.
2. Dans la liste des étapes à droite, cliquez sur l'étape 0.
3. Saisissez les conditions de voie dans la première ligne.
4. Si nécessaire, saisissez les conditions de voie dans la deuxième ligne et sélectionnez **Et** ou **Ou**.
5. Saisissez une valeur dans **Compteur**.
6. Refaites les étapes 2 à 5 pour chaque autre étape.

Voici trois exemples.

**Exemple 1.** Déclencher quand la voie 0 reste au niveau haut pendant plus de 1000 échantillons :

1. Réglez **Nombre d'étapes de déclenchement** sur 1.
2. Dans l'étape 0, saisissez `1` pour la voie 0 dans la première ligne.
3. Cochez **Contigu**.
4. Réglez **Compteur** sur 1000.

![Exemple 1](../figures/stage-example-level-count.png)

**Exemple 2.** Déclencher sur un front montant de la voie 0 ou un front descendant de la voie 1 :

1. Réglez **Nombre d'étapes de déclenchement** sur 1.
2. Dans l'étape 0, saisissez `R` pour la voie 0 dans la première ligne.
3. Saisissez `F` pour la voie 1 dans la deuxième ligne.
4. Sélectionnez **Ou**.

![Exemple 2](../figures/stage-example-or.png)

**Exemple 3.** Déclencher sur un front montant de la voie 0, puis 100 fronts descendants de la voie 1, puis un niveau haut de la voie 2 :

1. Réglez **Nombre d'étapes de déclenchement** sur 3.
2. Dans l'étape 0, saisissez `R` pour la voie 0.
3. Dans l'étape 1, saisissez `F` pour la voie 1. Réglez **Compteur** sur 100.
4. Dans l'étape 2, saisissez `1` pour la voie 2.

![Exemple 3](../figures/stage-example-sequence.png)

### Déclenchement série

Un déclenchement série trouve une valeur de données sur un bus série. Il fonctionne comme un registre à décalage. Voici les paramètres :

- **Condition de début** : La condition qui démarre le déclenchement série.
- **Condition de fin** : La condition qui efface le registre à décalage.
- **Condition d'horloge** : La condition qui ajoute un bit au registre à décalage.
- **Voie de données** : La voie qui transmet les données.
- **Bits de données** : Le nombre de bits de la valeur.
- **Valeur des données** : La valeur qui provoque le déclenchement.

Après la condition de début, l'appareil lit la voie de données à chaque condition d'horloge. L'appareil place ce bit dans le registre à décalage. Quand les derniers bits du registre à décalage sont égaux à **Valeur des données**, le déclenchement se produit. Quand la condition de fin se produit, l'appareil efface le registre à décalage.

![Les paramètres du déclenchement série](../figures/serial-trigger-panel.png)
<!-- TODO: new screenshot -->

**Exemple 4.** Déclencher quand la valeur `010000100` apparaît sur un bus I2C. La voie 0 est SCL et la voie 1 est SDA.

1. Réglez **Condition de début** sur un front descendant de SDA pendant que SCL est au niveau haut : `F1` dans les deux caractères de droite.
2. Réglez **Condition de fin** sur un front montant de SDA pendant que SCL est au niveau haut : `R1`.
3. Réglez **Condition d'horloge** sur un front montant de SCL : `R` pour la voie 0.
4. Réglez **Voie de données** sur 1.
5. Réglez **Bits de données** sur 9.
6. Saisissez `010000100` dans **Valeur des données**.

![Exemple 4](../figures/serial-example-i2c.png)

**Exemple 5.** Déclencher quand la valeur `0x1234` apparaît sur MOSI d'un bus SPI. La voie 0 est CS#, la voie 1 est CLK, la voie 2 est MISO et la voie 3 est MOSI.

1. Réglez **Condition de début** sur un front descendant de CS# : `F` pour la voie 0.
2. Réglez **Condition de fin** sur un front montant de CS# : `R` pour la voie 0.
3. Réglez **Condition d'horloge** sur un front montant de CLK : `R` pour la voie 1.
4. Réglez **Voie de données** sur 3.
5. Réglez **Bits de données** sur 16.
6. Saisissez `0001001000110100` dans **Valeur des données**.

![Exemple 5](../figures/serial-example-spi.png)

Pour saisir la valeur en hexadécimal, cochez **Saisie au format hexadécimal**. Ensuite, saisissez la valeur dans le champ **Hex**.
