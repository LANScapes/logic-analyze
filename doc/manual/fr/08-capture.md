# Capturer des données

Avant de démarrer une capture, réglez ces éléments :

1. Les options de l'appareil. Consultez [Options de l'appareil](05-device-options.md).
2. La fréquence et la durée d'échantillonnage. Consultez [Fréquence et durée d'échantillonnage](06-sample-rate.md).
3. Le déclenchement, si nécessaire. Consultez [Déclenchements](07-trigger.md).
4. Le mode de capture. Consultez [Modes de capture](#capture-modes).

## Démarrer une capture

Il y a deux types de capture :

- **Démarrer** démarre une capture standard. L'appareil attend le déclenchement si vous avez réglé un déclenchement.
- **Instant.** démarre immédiatement une capture. L'appareil n'utilise pas les paramètres de déclenchement.

Pour démarrer une capture standard, cliquez sur **Démarrer** ou appuyez sur `S`. Pour démarrer une capture instantanée, cliquez sur **Instant.** ou appuyez sur `I`. Pendant une capture, le bouton devient **Arrêter**. Cliquez sur **Arrêter** pour arrêter la capture.

### Déroulement d'une capture standard en mode tampon

1. Vous cliquez sur **Démarrer**.
2. L'application envoie les paramètres à l'appareil.
3. S'il n'y a pas de déclenchement, l'appareil commence immédiatement à enregistrer. S'il y a un déclenchement, l'appareil attend le déclenchement.
4. L'appareil enregistre jusqu'à la fin de la durée d'échantillonnage ou jusqu'à ce que sa mémoire soit pleine.
5. L'appareil envoie les données à l'ordinateur.
6. L'application affiche la forme d'onde dans la zone des formes d'onde.

### Déroulement d'une capture standard en mode flux

1. Vous cliquez sur **Démarrer**.
2. L'application envoie les paramètres à l'appareil.
3. S'il y a un déclenchement, l'appareil attend le déclenchement. En mode boucle, l'appareil n'utilise pas le déclenchement.
4. L'appareil envoie les données à l'ordinateur pendant la capture.
5. L'application affiche la forme d'onde pendant la capture.
6. La capture s'arrête à la fin de la durée d'échantillonnage. En mode boucle, la capture continue jusqu'à ce que vous cliquiez sur **Arrêter**.

## Utiliser la capture instantanée

La capture instantanée est identique à la capture standard, mais elle n'utilise pas les paramètres de déclenchement. Utilisez-la dans ces cas :

- La capture standard attend longtemps parce que la condition de déclenchement ne se produit pas.
- Vous voulez voir les signaux maintenant.
- Vous voulez examiner les signaux avant de changer le déclenchement.

S'il n'y a pas de signal, une capture standard attend à la position du déclenchement. L'état affiche **En attente du déclenchement !**. Une capture instantanée enregistre immédiatement les signaux.

## Modes de capture {#capture-modes}

Pour sélectionner le mode de capture, cliquez sur **Mode** dans la barre d'outils. Ensuite, sélectionnez un de ces éléments :

| Mode de capture | Mode tampon | Mode flux |
| --- | --- | --- |
| **Unique** | Oui | Oui |
| **Répétitif** | Oui | Oui |
| **Boucle** | Non | Oui |

![Le menu des modes de capture](../figures/capture-mode-menu.png)
<!-- TODO: new screenshot -->

### Unique

L'appareil fait une capture. Ensuite, la capture s'arrête.

En mode tampon, l'application affiche la forme d'onde après la capture. En mode flux, l'application affiche la forme d'onde pendant la capture.

Utilisez ce mode pour capturer une condition de signal ou la forme d'onde actuelle.

### Répétitif

L'appareil fait une capture. Ensuite, il démarre automatiquement la capture suivante. Cela continue jusqu'à ce que vous cliquiez sur **Arrêter**.

En mode tampon, l'application affiche une fenêtre pour l'intervalle entre les captures. Vous pouvez régler une valeur de 0,1 s à 10 s.

Utilisez ce mode pour voir une condition de signal qui se produit plusieurs fois. Par exemple, utilisez-le pour voir les signaux après chaque réinitialisation du circuit ou après chaque appui sur un bouton. Utilisez-le avec un déclenchement.

### Boucle

Ce mode est disponible uniquement en mode flux. La capture continue jusqu'à ce que vous cliquiez sur **Arrêter**. Quand les données sont plus longues que la durée d'échantillonnage, les premières données sortent de la fenêtre à gauche. Les données les plus récentes entrent à droite. L'application supprime les données qui sortent.

Utilisez ce mode quand vous ne connaissez pas l'instant de la condition de signal. Regardez la forme d'onde pendant la capture. Quand vous voyez la condition, cliquez sur **Arrêter**.

> [!NOTE]
> En mode boucle, l'appareil n'utilise pas les paramètres de déclenchement.

## État de la capture

Pendant une capture, la zone des formes d'onde affiche l'état :

- **En attente du déclenchement !** : L'appareil attend la condition de déclenchement.
- **Déclenché !** : Le déclenchement s'est produit.
- **% acquis** : Le pourcentage de la capture qui est terminé.

Après une capture, le bas de la zone des formes d'onde affiche **Heure du déclenchement**.
