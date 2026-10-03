# Fichiers et sessions

Cliquez sur **Fichier** dans la barre d'outils pour ouvrir le menu des fichiers. Le menu contient ces éléments :

- **Config** : un menu pour charger et enregistrer des sessions.
- **Ouvrir...** : ouvrir un fichier de données.
- **Enregistrer...** : enregistrer les données de la capture.
- **Exporter...** : exporter les données dans un autre format.
- **Capture d’écran...** : enregistrer une image de la fenêtre.

## Sessions

Un fichier de session contient les paramètres, mais pas les données de la capture. Une session contient les options de l'appareil, les voies activées, les noms et couleurs des voies et les paramètres de déclenchement. Un fichier de session a l'extension `.dsc`.

### Enregistrer une session

1. Cliquez sur **Fichier** › **Config** › **Enreg. session**.
2. Sélectionnez le dossier et saisissez le nom du fichier.
3. Cliquez sur **Enregistrer**.

### Charger une session

1. Cliquez sur **Fichier** › **Config** › **Charger session**.
2. Sélectionnez le fichier de session.
3. Cliquez sur **Ouvrir**.

### Revenir aux paramètres initiaux

Cliquez sur **Fichier** › **Config** › **Session par défaut**. L'application remet tous les paramètres de l'appareil à leurs valeurs initiales.

L'application enregistre automatiquement les paramètres quand vous quittez. Quand vous démarrez de nouveau l'application, elle charge les paramètres de la dernière session.

## Enregistrer les données

1. Cliquez sur **Fichier** › **Enregistrer...**.
2. Sélectionnez le dossier et saisissez le nom du fichier.
3. Cliquez sur **Enregistrer**.

L'application enregistre les données et les paramètres dans un fichier avec l'extension `.dsl`. Vous pouvez ouvrir de nouveau ce fichier dans Logic Analyze.

> [!CAUTION]
> L'application n'enregistre pas les données automatiquement. Enregistrez les données avant de démarrer une nouvelle capture ou de quitter l'application. Une nouvelle capture remplace les données de la capture précédente.

## Ouvrir un fichier de données

1. Cliquez sur **Fichier** › **Ouvrir...**.
2. Sélectionnez un fichier avec l'extension `.dsl`.
3. Cliquez sur **Ouvrir**.

L'application affiche les données dans la zone des formes d'onde. L'étiquette du type d'appareil affiche **Fichier**.

## Exporter les données

L'exportation crée un fichier que d'autres programmes peuvent lire.

1. Cliquez sur **Fichier** › **Exporter...**. La fenêtre **Exporter** s'ouvre.
2. Cliquez sur **chemin**.
3. Sélectionnez le dossier, saisissez le nom du fichier et sélectionnez le format.
4. Cliquez sur **Enregistrer**.
5. Si le format est CSV, sélectionnez **Données d'origine** ou **Données compressées**. Les données compressées contiennent une ligne uniquement pour chaque changement de valeur.
6. Cliquez sur **OK**.

En mode analyseur logique, ces formats sont disponibles :

| Format | Extension | Utilisation |
| --- | --- | --- |
| CSV | `.csv` | Tableurs et scripts. |
| VCD | `.vcd` | Programmes de formes d'onde, par exemple GTKWave. |
| Gnuplot | `.gnuplot` | Le programme Gnuplot. |
| srzip | `.srzip` | Programmes sigrok, par exemple PulseView. |

En mode oscilloscope et en mode acquisition de données, seul le format CSV est disponible.

![La fenêtre d'exportation pour CSV](../figures/export-csv.png)
<!-- TODO: new screenshot -->

## Enregistrer une image de la fenêtre

1. Cliquez sur **Fichier** › **Capture d’écran...**.
2. Sélectionnez le dossier et saisissez le nom du fichier.
3. Sélectionnez PNG ou JPEG.
4. Cliquez sur **Enregistrer**.
