# L'outil dslcap

L'outil `dslcap` capture des données d'un appareil DSLogic sans la fenêtre principale. Utilisez-le dans des scripts et des tests automatiques. L'outil écrit les échantillons dans un fichier binaire. Il écrit un objet JSON avec le résultat sur la sortie standard.

L'outil se trouve dans le paquet de l'application :

```sh
"/Applications/Logic Analyze.app/Contents/MacOS/dslcap"
```

> [!NOTE]
> Un seul programme à la fois peut utiliser l'appareil. Quittez Logic Analyze avant d'utiliser `dslcap`.

## Lister les appareils

Pour lister les appareils que la bibliothèque peut trouver, saisissez cette commande :

```sh
dslcap --list
```

Pour lister l'identifiant USB de chaque appareil DSLogic connecté, saisissez cette commande :

```sh
dslcap --list-ids
```

La commande `--list-ids` lit uniquement les informations que macOS conserve sur les appareils USB. Elle n'envoie pas de données à l'appareil. La sortie donne le modèle, l'emplacement USB et un identifiant de registre pour chaque appareil.

## Capturer des données

Cette commande capture 1000000 échantillons sur les voies 0 et 1 à 10 MHz :

```sh
dslcap --channels 0,1 --samplerate 10000000 --samples 1000000 --out /tmp/capture
```

L'outil écrit les échantillons dans `/tmp/capture.bin`. Si un fichier de ce nom existe, l'outil s'arrête avec une erreur. L'outil ne remplace pas un fichier.

Voici les options de capture :

| Option | Fonction | Valeur au départ |
| --- | --- | --- |
| `--channels LIST` | Les voies à enregistrer, par exemple `0,1,2`. | `0` |
| `--samplerate HZ` | La fréquence d'échantillonnage en Hz. | `10000000` |
| `--samples N` | Le nombre d'échantillons pour chaque voie. | `1000000` |
| `--vth VOLTS` | La tension de seuil. | `1.6` |
| `--mode MODE` | `buffer` ou `stream`. | `buffer` |
| `--trigger CH[:T]` | Un déclenchement sur la voie CH. Utilisez `R` (front montant), `F` (front descendant), `C` (front montant ou front descendant), `1` (niveau haut) ou `0` (niveau bas) pour T. | Pas de déclenchement. `R` si vous donnez seulement CH. |
| `--trigpos PERCENT` | La position du déclenchement en pourcentage des échantillons. | `10` |
| `--timeout SEC` | La durée maximale de la capture en secondes. | `30` |
| `--out PATH` | Le chemin du fichier de sortie, sans l'extension `.bin`. | Cette option est nécessaire. |
| `--log-level N` | La quantité de messages de la bibliothèque sur la sortie d'erreur standard, de 0 (aucun) à 5 (tous). | `1` |

L'outil examine toutes les options avant d'utiliser l'appareil. Si une option n'est pas correcte, l'outil s'arrête et donne une erreur.

## Le fichier de sortie

Le fichier `.bin` contient les voies dans l'ordre de leurs numéros, à partir du plus petit numéro. Pour chaque voie, le fichier contient tous les échantillons de cette voie. Chaque octet contient 8 échantillons. Le premier échantillon est le bit de poids faible. Les données de chaque voie remplissent un nombre entier d'unités de 8 octets. Chaque voie utilise donc `ceil(samples / 64) × 8` octets.

## Le résultat JSON

L'outil écrit un objet JSON sur une ligne. Après une capture correcte, l'objet donne le nom de l'appareil, la fréquence d'échantillonnage, le nombre d'échantillons et les voies. Il donne aussi la tension de seuil, le mode, le déclenchement, la durée de la capture et le chemin du fichier `.bin`. Si la capture n'est pas correcte, l'objet contient une clé `error`. Dans ce cas, l'outil ne crée pas de fichier `.bin`.

Utilisez le résultat uniquement quand le code de sortie est 0 et que l'objet JSON est complet.

## Code de sortie

| Code | Signification |
| --- | --- |
| 0 | La capture est terminée. |
| 1 | Une erreur s'est produite pendant l'opération, par exemple une erreur d'entrée/sortie. |
| 2 | Une option n'est pas correcte, ou un paramètre n'est pas disponible sur l'appareil. |
| 3 | La capture n'est pas terminée. |

## Options pour les programmes qui démarrent dslcap

- `--parent-fd N` : L'outil s'arrête quand le programme qui l'a démarré ferme le tube avec le descripteur N. Ensuite, l'outil supprime son fichier de sortie si la capture n'est pas terminée.
- `--res DIR` : Le dossier des fichiers de firmware. En général, l'outil trouve ce dossier automatiquement. Vous pouvez aussi régler la variable d'environnement `DSLCAP_RES`.
- `--res-manifest FD` : L'outil examine la valeur SHA-256 de chaque fichier de firmware avant d'envoyer le fichier à l'appareil.

Le fichier `tools/dslcap/README.md` du code source donne toutes les informations sur ces options.
