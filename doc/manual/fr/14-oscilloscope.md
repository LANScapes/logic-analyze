# Modes oscilloscope et acquisition de données

Logic Analyze peut aussi utiliser les oscilloscopes DSCope de DreamSourceLab. Un DSCope a deux modes d'appareil :

- **Oscilloscope** : pour les signaux à période constante et pour une condition de signal.
- **Acquisition** : pour les signaux lents sur une longue durée, par exemple une tension d'alimentation ou la sortie d'un capteur.

Ces modes ne sont pas disponibles sur les appareils DSLogic. Ce chapitre donne seulement les procédures principales.

## Connecter le DSCope

> [!WARNING]
> Ne connectez pas les sondes à la tension du secteur. Ne connectez pas les sondes à un circuit qui a une connexion électrique avec la tension du secteur. La tension peut provoquer des blessures ou la mort.

> [!CAUTION]
> La masse des sondes, la masse du DSCope et la masse de l'ordinateur sont reliées. Connectez la masse de la sonde uniquement à un point qui a la même tension que la masse de l'ordinateur. Une différence de tension peut endommager l'équipement.

1. Connectez le DSCope à l'ordinateur avec le câble USB.
2. Démarrez Logic Analyze. Vérifiez que la liste des appareils affiche le DSCope.
3. Connectez les sondes aux entrées du DSCope.
4. Réglez le commutateur d'atténuation de chaque sonde.
5. Connectez la pince de masse de chaque sonde à la masse du circuit.
6. Connectez la pointe de la sonde au signal.

## Options de l'appareil

Cliquez sur **Options** › **Options de l'appareil...** ou appuyez sur `O`.

- **Mode de fonctionnement** : **Normal** pour les mesures. **Test interne** sert uniquement aux tests de l'appareil.
- **Limite de bande passante** : **Pleine bande passante** ou **20MHz**. La limite de 20 MHz diminue le bruit à haute fréquence.

## Étalonner le DSCope

Le gain et le décalage des entrées changent avec la température et l'humidité. Étalonnez le DSCope pour garder des mesures précises.

### Étalonnage automatique

> [!CAUTION]
> Déconnectez toutes les sondes des entrées avant l'étalonnage. Un signal sur une entrée pendant l'étalonnage donne des valeurs d'étalonnage incorrectes.

1. Ouvrez la fenêtre **Options de l'appareil**.
2. Cliquez sur **Étalonnage automatique**.
3. Déconnectez toutes les sondes. Cliquez sur **OK**. L'étalonnage dure quelques minutes.
4. Quand l'étalonnage est terminé, cliquez sur **Enregistrer** pour conserver le résultat.

Pour arrêter l'étalonnage, cliquez sur **Abandonner**. L'appareil utilise alors les valeurs d'étalonnage précédentes.

### Étalonnage manuel

1. Ouvrez la fenêtre **Options de l'appareil**.
2. Cliquez sur **Étalonnage manuel**.
3. Cliquez sur **Démarrer** dans la barre d'outils.
4. Pour régler le décalage, connectez la sonde à la masse. Pour régler le gain, connectez la sonde à un signal de tension connue.
5. Réglez l'échelle verticale à étalonner.
6. Déplacez le curseur **VOFF** ou **VGAIN** de la voie jusqu'à ce que la forme d'onde soit correcte.
7. Refaites les étapes 5 et 6 pour chaque échelle verticale.
8. Cliquez sur **Enregistrer**.

Pour annuler les changements, cliquez sur **Abandonner**. Pour utiliser les changements seulement jusqu'à la déconnexion de l'appareil, cliquez sur **Quitter**. Pour revenir aux valeurs initiales, cliquez sur **Réinitialiser**. Après une réinitialisation, refaites l'étalonnage automatique.

## Paramètres des voies

Chaque voie a ces commandes à gauche de la zone des formes d'onde :

- **Activer** : active ou désactive la voie.
- **Échelle verticale** : la tension par division. La fenêtre a 10 divisions. Pour changer l'échelle, tournez la molette de la souris sur le bouton rotatif, ou cliquez sur la partie haute ou basse du bouton rotatif. Vous pouvez aussi appuyer sur `0` ou `1` pour sélectionner le bouton rotatif d'une voie, puis appuyer sur `↑` ou `↓`.
- **Couplage** : **DC** ou **AC**.
- **Atténuation de la sonde** : réglez **x1** ou **x10** selon le commutateur de la sonde.
- **AUTO** : règle l'échelle verticale, l'échelle horizontale et le niveau de déclenchement pour le signal présent sur l'entrée.

Pour déplacer la forme d'onde d'une voie vers le haut ou vers le bas, faites glisser l'étiquette de la voie.

## Échelle horizontale

Sélectionnez le temps par division dans la liste de la barre d'outils. Vous pouvez aussi tourner la molette de la souris dans la zone des formes d'onde.

## Démarrer et arrêter

- Cliquez sur **Démarrer** ou appuyez sur `S` pour démarrer une capture continue. Cliquez sur **Arrêter** pour l'arrêter.
- Cliquez sur **Unique** ou appuyez sur `I` pour capturer une forme d'onde puis arrêter.

## Déclenchement

Cliquez sur **Déclench.** ou appuyez sur `T` pour ouvrir le panneau de déclenchement. Le panneau contient ces paramètres :

- **Position du déclenchement** : la position du point de déclenchement dans la capture, en pourcentage.
- **Temps d'inhibition** : le temps après un déclenchement pendant lequel l'appareil ignore les nouveaux déclenchements. Utilisez-le pour obtenir une forme d'onde stable avec des groupes d'impulsions.
- **Sensibilité du déclenchement** : la variation de tension nécessaire pour un déclenchement. Une valeur plus grande ignore plus de bruit.
- **Sources de déclenchement** : **Auto**, **Voie 0**, **Voie 1**, **Voie 0 && 1** ou **Voie 0 | 1**.
- **Types de déclenchement** : **Front montant** ou **Front descendant**.

Pour régler le niveau de déclenchement, cliquez sur l'étiquette de niveau de déclenchement de la voie. Déplacez la souris. Cliquez de nouveau pour fixer le niveau.

## Mesures

### Mesures automatiques

Le bas de la zone des formes d'onde a 10 zones pour les mesures automatiques.

1. Cliquez sur une zone de mesure.
2. Sélectionnez la voie.
3. Sélectionnez la mesure. Pour vider la zone, cliquez sur **Réinitialiser**.

L'application conserve ces paramètres pour le prochain démarrage.

### Curseurs

- Pour ajouter un curseur de temps, cliquez sur la règle de temps. Vous pouvez aussi cliquer avec le bouton droit de la souris dans la zone des formes d'onde et sélectionner **Ajouter un curseur Y**.
- Pour ajouter un curseur de tension, cliquez avec le bouton droit de la souris dans la zone des formes d'onde et sélectionnez **Ajouter un curseur X**. Chaque curseur de tension a deux lignes horizontales. L'étiquette entre les lignes affiche la différence de tension.
- Pour mesurer le temps entre deux curseurs, utilisez le groupe **Écart entre curseurs** du panneau de mesure.

### Mesurer avec le pointeur

Après l'arrêt de la capture, placez le pointeur sur la forme d'onde. L'application affiche la tension de l'échantillon sous le pointeur.

Pour mesurer un temps, double-cliquez dans une zone vide de la forme d'onde. Cliquez au deuxième point. Cliquez au troisième point pour voir la fréquence, la période et le rapport cyclique. Cliquez avec le bouton droit de la souris pour annuler.

## Spectre (FFT)

1. Cliquez sur **Fonction** › **FFT**.
2. Cochez **Activer la FFT**.
3. Réglez **Longueur FFT**, **Intervalle d'échantillonnage**, **Source FFT** et **Fenêtre FFT**.
4. Réglez **Mode de l'axe Y** et **Plage dBV**.
5. Cliquez sur **OK**.

Le spectre s'affiche sous la forme d'onde. Tournez la molette de la souris dans le spectre pour zoomer sur l'échelle des fréquences. Faites glisser le spectre pour le déplacer. Placez le pointeur sur le spectre pour voir la fréquence et l'amplitude.

## Voie mathématique

1. Cliquez sur **Fonction** › **Maths**.
2. Cochez **Activer**.
3. Sélectionnez l'**Opération** : **Addition**, **Soustraction**, **Multiplication** ou **Division**.
4. Sélectionnez la **1re source** et la **2e source**.
5. Cliquez sur **OK**.

## Figure de Lissajous

1. Cliquez sur **Options** › **Affichage** › **Lissajous**.
2. Cochez **Activer**.
3. Sélectionnez la voie pour l'**Axe X** et l'**Axe Y**.
4. Cliquez sur **OK**.

## Mode acquisition de données

1. Dans la liste des modes d'appareil de la barre d'outils, sélectionnez **Acquisition**.
2. Ouvrez la fenêtre **Options de l'appareil**.
3. Pour chaque voie, réglez **Activer**, **Couplage** et **Volts/div**.
4. Pour afficher une autre unité, réglez **Unité de conversion**, **Min. de conversion** et **Max. de conversion**. Par exemple, affichez la sortie d'un capteur de température en °C.
5. Cliquez sur **OK**.
6. Sélectionnez la fréquence et la durée d'échantillonnage dans la barre d'outils.
7. Cliquez sur **Démarrer** ou appuyez sur `S`.

Vous ne pouvez pas changer les paramètres des voies pendant la capture. À la fréquence d'échantillonnage maximale de 10 MHz, la durée d'échantillonnage maximale est d'environ 10 secondes. À 1 kHz, la capture peut durer une journée.

Le mode acquisition de données utilise l'étalonnage du mode oscilloscope. Si une voie affiche un décalage, étalonnez l'appareil en mode oscilloscope.
