# Fréquence et durée d'échantillonnage

La barre d'outils a deux listes pour la longueur de la capture. La liste du haut est la durée d'échantillonnage. La liste du bas est la fréquence d'échantillonnage.

- La **durée d'échantillonnage** est la durée de la capture.
- La **fréquence d'échantillonnage** est le nombre d'échantillons par seconde, pour chaque voie.

Les valeurs disponibles changent selon l'appareil, la connexion USB, le mode de fonctionnement et le mode des voies.

## Durée d'échantillonnage maximale

**Mode tampon.** La mémoire de l'appareil limite la durée d'échantillonnage. Utilisez cette formule :

```text
maximum duration = memory size / (sample rate × number of enabled channels)
```

Le DSLogic Plus a 256 Mbit de mémoire. Voici deux exemples :

- À 100 MHz avec 16 voies, la durée d'échantillonnage maximale est d'environ 167,77 ms.
- À 400 MHz avec 1 voie, la durée d'échantillonnage maximale est d'environ 671,09 ms.

**Mode flux.** La mémoire de l'ordinateur limite la durée d'échantillonnage. L'application peut conserver 16 G échantillons pour chaque voie. Voici deux exemples :

- À 1 MHz, la durée d'échantillonnage maximale est d'environ 4,77 heures.
- À 100 MHz, la durée d'échantillonnage maximale est d'environ 2,86 minutes.

## Choisir la fréquence d'échantillonnage

Réglez la fréquence d'échantillonnage à 4 à 10 fois la fréquence la plus élevée du signal.

À 4 fois la fréquence du signal, l'application enregistre chaque front. Mais le temps de chaque front a une erreur de 25 % maximum de la période du signal. À 10 fois la fréquence du signal, l'erreur diminue à 10 %.

L'erreur de temps d'un front est égale à une période d'échantillonnage ou moins. Par exemple, à 100 MHz, la période d'échantillonnage est de 10 ns. L'erreur de chaque front est donc de ±10 ns ou moins.

![L'effet de la fréquence d'échantillonnage sur la forme d'onde enregistrée](../figures/sample-rate-effect.png)

Voici des valeurs typiques :

| Signal | Fréquence d'échantillonnage typique |
| --- | --- |
| UART à 115200 bauds | 2 MHz |
| I2C à 400 kHz | 4 MHz à 10 MHz |
| SPI à 40 MHz | 400 MHz |

## Ne pas utiliser une fréquence d'échantillonnage trop élevée

Une fréquence d'échantillonnage plus élevée donne une forme d'onde plus précise. Mais une fréquence d'échantillonnage élevée a aussi ces problèmes :

1. L'application enregistre plus de données chaque seconde. La durée d'échantillonnage maximale diminue donc. L'application utilise aussi plus de temps pour afficher et décoder les données.
2. Un signal lent peut avoir des fronts lents. À une fréquence d'échantillonnage élevée, l'application peut enregistrer de petites impulsions au niveau du seuil pendant chaque front lent. Ces impulsions peuvent provoquer des erreurs dans les décodeurs.

Si vous voyez des impulsions courtes indésirables sur des signaux lents, diminuez la fréquence d'échantillonnage. Vous pouvez aussi régler **Cibles du filtre** sur **1 période d'échantillonnage**. Consultez [Options de l'appareil](05-device-options.md).

## Régler la fréquence et la durée d'échantillonnage

1. Réglez le mode de fonctionnement et le mode des voies. Consultez [Options de l'appareil](05-device-options.md).
2. Dans la liste du bas de la barre d'outils, sélectionnez la fréquence d'échantillonnage.
3. Dans la liste du haut de la barre d'outils, sélectionnez la durée d'échantillonnage.

> [!NOTE]
> Quand vous changez le mode des voies, l'application peut changer la fréquence d'échantillonnage. Vérifiez de nouveau la fréquence d'échantillonnage après chaque changement des options de l'appareil.
