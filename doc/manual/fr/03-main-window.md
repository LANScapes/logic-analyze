# La fenêtre principale

## Parties de la fenêtre principale

La fenêtre principale a ces parties :

- **Barre d'outils.** La barre d'outils contient les commandes de l'appareil, de la capture et des outils.
- **Zone des formes d'onde.** La zone des formes d'onde affiche une ligne pour chaque voie. Une règle de temps se trouve au-dessus des lignes.
- **Étiquettes de voie.** Une étiquette à gauche de chaque ligne affiche le numéro de la voie, le nom et les boutons de déclenchement.
- **Panneaux.** Un panneau est une zone sur le côté de la zone des formes d'onde. Les outils de déclenchement, de décodage, de mesure et de recherche s'ouvrent dans des panneaux.

![La fenêtre principale en mode analyseur logique](../figures/main-window.png)
<!-- TODO: new screenshot -->

## La barre d'outils

La barre d'outils contient ces éléments, du début à la fin :

| Élément | Fonction |
| --- | --- |
| **Fichier** | Un menu pour ouvrir, enregistrer et exporter des données et pour enregistrer des sessions. Consultez [Fichiers et sessions](12-files.md). |
| Type d'appareil | Une étiquette qui affiche la connexion : **USB 2.0**, **USB 3.0**, **Démo** ou **Fichier**. |
| Liste des appareils | L'appareil que l'application utilise. Sélectionnez ici un autre appareil ou un appareil de démonstration. |
| Mode de l'appareil | **Analyseur logique**, **Oscilloscope** ou **Acquisition**. La liste affiche uniquement les modes disponibles pour l'appareil. |
| Durée d'échantillonnage | La durée d'une capture. |
| Fréquence d'échantillonnage | Le nombre d'échantillons par seconde, pour chaque voie. |
| **Mode** | Le mode de capture : **Unique**, **Répétitif** ou **Boucle**. |
| **Démarrer** | Démarre une capture. Pendant une capture, ce bouton devient **Arrêter**. |
| **Instant.** | Démarre une capture qui n'attend pas le déclenchement. |
| **Déclench.** | Ouvre le panneau de déclenchement. |
| **Décoder** | Ouvre le panneau des décodeurs. |
| **Mesure** | Ouvre le panneau de mesure. |
| **Chercher** | Ouvre la barre de recherche. |
| **Options** | Un menu avec **Options de l'appareil...** et le menu **Affichage**. |
| **Aide** | Un menu avec la langue, ce manuel, la page des mises à jour, les options de journalisation et la page de signalement des problèmes. |

L'étiquette du type d'appareil affiche ces valeurs :

- **USB 3.0** : L'appareil utilise une connexion USB 3.0.
- **USB 2.0** : L'appareil utilise une connexion USB 2.0. Si l'appareil a une connexion USB 3.0, connectez-le à un port USB 3.0. Une connexion USB 2.0 diminue la fréquence d'échantillonnage maximale en mode flux.
- **Démo** : L'appareil est un appareil de démonstration. L'appareil de démonstration produit des signaux de test. Utilisez-le pour essayer les fonctions de l'application.
- **Fichier** : L'application affiche les données d'un fichier. Il n'y a pas d'appareil.

## Raccourcis clavier

| Touche | Fonction |
| --- | --- |
| `S` | Démarrer ou arrêter une capture. |
| `I` | Démarrer ou arrêter une capture instantanée. En mode oscilloscope, faire une capture puis arrêter. |
| `T` | Ouvrir ou fermer le panneau de déclenchement. |
| `D` | Ouvrir ou fermer le panneau des décodeurs. |
| `M` | Ouvrir ou fermer le panneau de mesure. |
| `R` | Ouvrir ou fermer la barre de recherche. |
| `O` | Ouvrir la fenêtre **Options de l'appareil**. |
| `Page Up` | Déplacer la forme d'onde d'une largeur de fenêtre vers la gauche. |
| `Page Down` | Déplacer la forme d'onde d'une largeur de fenêtre vers la droite. |
| `←` | Zoom avant. |
| `→` | Zoom arrière. |
| `0`, `1` | En mode oscilloscope, sélectionner ou libérer le réglage d'échelle de la voie 0 ou de la voie 1. |
| `↑`, `↓` | En mode oscilloscope, changer l'échelle verticale de la voie sélectionnée. |

Les raccourcis fonctionnent quand la zone des formes d'onde a le focus du clavier.
