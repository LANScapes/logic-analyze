# Options de l'appareil

## Ouvrir les options de l'appareil

1. Cliquez sur **Options** › **Options de l'appareil...** dans la barre d'outils. Vous pouvez aussi appuyer sur `O`.
2. Changez les paramètres dans la fenêtre **Options de l'appareil**.
3. Cliquez sur **OK**.

Les paramètres de la fenêtre sont différents pour chaque modèle d'appareil. Ce chapitre donne les paramètres du DSLogic Plus.

> [!NOTE]
> Vous ne pouvez pas changer les options de l'appareil pendant une capture.

![La fenêtre Options de l'appareil](../figures/device-options.png)
<!-- TODO: new screenshot -->

## Mode de fonctionnement

Le paramètre **Mode de fonctionnement** sélectionne comment l'appareil envoie les données à l'ordinateur.

**Mode tampon.** L'appareil conserve les échantillons dans sa mémoire interne pendant la capture. Après la capture, l'appareil envoie les données à l'ordinateur par USB. La mémoire est plus rapide que l'USB. Le mode tampon donne donc les fréquences d'échantillonnage les plus élevées. La capacité de la mémoire limite la longueur de la capture. Utilisez le mode tampon pour les signaux rapides et les captures courtes.

**Mode flux.** L'appareil envoie les échantillons à l'ordinateur pendant la capture. La mémoire de l'ordinateur limite la longueur de la capture. Vous pouvez voir les données pendant la capture. La vitesse de la connexion USB limite la fréquence d'échantillonnage. Utilisez le mode flux pour les signaux lents et les captures longues.

**Test interne.** Ce mode sert uniquement aux tests de l'appareil. Ne l'utilisez pas pour des mesures.

## Options d'arrêt

Le paramètre **Options d'arrêt** s'applique uniquement au mode tampon. Il définit le fonctionnement de l'application quand vous arrêtez une capture avant la fin.

- **Arrêter immédiatement** : L'application ne récupère pas les données de l'appareil. L'application n'affiche aucune donnée.
- **Transférer les données acquises** : L'application récupère les données que l'appareil a enregistrées avant l'arrêt. L'application affiche ces données.

## Niveau de seuil

Le paramètre **Niveau de seuil** est la tension qui sépare un niveau bas d'un niveau haut. Un signal au-dessus du seuil est un niveau haut. Un signal en dessous du seuil est un niveau bas.

Vous pouvez régler une valeur de 0,0 V à 5,0 V par pas de 0,1 V. Réglez le seuil à environ 50 % de la tension logique du circuit. Pour un circuit de 3,3 V, réglez environ 1,6 V.

## Cibles du filtre

Le paramètre **Cibles du filtre** supprime les impulsions courtes des données.

- **Aucun** : L'application affiche tous les échantillons.
- **1 période d'échantillonnage** : L'application supprime chaque impulsion plus courte qu'une période d'échantillonnage.

## Hauteur maximale

Le paramètre **Hauteur max.** définit la hauteur maximale de chaque ligne de voie dans la zone des formes d'onde. **1X** est une unité de hauteur. Utilisez une valeur plus grande quand vous affichez seulement un petit nombre de voies.

## Activer la compression RLE

Quand vous sélectionnez **Activer la compression RLE**, l'appareil compresse les données dans sa mémoire (codage par plages). Ce paramètre s'applique uniquement au mode tampon. Si les signaux ont peu de fronts, l'appareil peut conserver une capture plus longue dans sa mémoire. Si les signaux ont beaucoup de fronts, la compression n'augmente pas la longueur.

## Utiliser l'horloge externe

Quand vous sélectionnez **Utiliser l'horloge externe**, l'appareil échantillonne les voies à chaque front d'horloge sur le fil CK. L'appareil n'utilise pas son horloge interne. Utilisez ce paramètre pour enregistrer un bus qui a un signal d'horloge.

## Utiliser le front descendant d'horloge

Ce paramètre s'applique uniquement avec **Utiliser l'horloge externe**. En général, l'appareil échantillonne les voies sur le front montant de l'horloge. Quand vous sélectionnez **Utiliser le front descendant d'horloge**, l'appareil échantillonne les voies sur le front descendant de l'horloge.

## Mode des voies

Le mode des voies définit le nombre de voies que l'appareil peut utiliser. Il définit aussi la fréquence d'échantillonnage maximale. Un plus petit nombre de voies donne une fréquence d'échantillonnage maximale plus élevée. Sélectionnez le mode des voies qui correspond au nombre et à la fréquence de vos signaux.

Pour le DSLogic Plus, les modes des voies sont :

| Mode de fonctionnement | Mode des voies | Fréquence d'échantillonnage maximale |
| --- | --- | --- |
| Mode tampon | Voies 0 à 15 | 100 MHz |
| Mode tampon | Voies 0 à 7 | 200 MHz |
| Mode tampon | Voies 0 à 3 | 400 MHz |
| Mode flux | 16 voies | 20 MHz |
| Mode flux | 12 voies | 25 MHz |
| Mode flux | 6 voies | 50 MHz |
| Mode flux | 3 voies | 100 MHz |

## Activer et désactiver des voies

Sous les modes des voies, la fenêtre affiche une case à cocher pour chaque voie.

1. Cochez la case de chaque voie que vous utilisez.
2. Décochez la case de chaque voie que vous n'utilisez pas.
3. Pour cocher toutes les voies, cliquez sur **Tout activer**. Pour décocher toutes les voies, cliquez sur **Tout désactiver**.

En mode flux, un plus petit nombre de voies activées peut vous permettre d'utiliser une fréquence d'échantillonnage plus élevée.
