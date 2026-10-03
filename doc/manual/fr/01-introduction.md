# Introduction

## À propos de Logic Analyze

Logic Analyze est une application macOS pour les analyseurs logiques de DreamSourceLab. LANScapes fournit l'application. L'application vient de DSView, un programme de DreamSourceLab. DSView utilise des logiciels du projet sigrok.

L'application enregistre des signaux numériques avec un analyseur logique DSLogic. Ensuite, elle affiche les signaux sous forme de formes d'onde. Vous pouvez mesurer les formes d'onde et décoder des protocoles série. Vous pouvez aussi enregistrer les données et les exporter dans d'autres formats.

Ce manuel utilise le DSLogic Plus pour les exemples. Les autres modèles DSLogic utilisent les mêmes procédures. Leurs limites de voies, de mémoire et de fréquence d'échantillonnage sont différentes.

L'application contient aussi l'outil `dslcap`. Cet outil capture des données sans la fenêtre principale. Consultez [L'outil dslcap](13-dslcap.md).

## À propos de ce manuel

Ce manuel applique les règles de l'ASD-STE100 Simplified Technical English. Chaque phrase est courte. Chaque étape d'une procédure donne une seule instruction. Chaque terme technique de ce manuel a un seul sens. [Termes techniques et verbes](15-terms.md) donne la liste des termes techniques.

Ce manuel utilise ces formats de texte :

- Le **texte en gras** indique un libellé de l'application, par exemple un bouton, un élément de menu ou un champ.
- Le `texte de code` indique une touche du clavier, une commande, un nom de fichier ou une valeur à saisir.
- Un chemin dans les menus utilise le signe ›, par exemple **Fichier** › **Enregistrer...**.
- Une liste d'étapes numérotées est une procédure. Faites les étapes dans l'ordre indiqué.

## Consignes de sécurité

Ce manuel utilise ces libellés pour les consignes de sécurité :

- **AVERTISSEMENT** indique un risque de blessure ou de mort.
- **ATTENTION** indique un risque de dommage à l'équipement ou un risque pour vos données.
- **REMARQUE** donne une information utile. L'information après **REMARQUE** ne donne pas d'instruction.

Une consigne de sécurité se trouve avant l'étape à laquelle elle s'applique. Lisez toutes les consignes de sécurité avant de commencer une procédure.

> [!WARNING]
> Ne connectez pas les sondes à la tension du secteur. Ne connectez pas les sondes à un circuit qui a une connexion électrique avec la tension du secteur. Les sondes ont une connexion électrique avec l'ordinateur. La tension peut provoquer des blessures ou la mort.

> [!CAUTION]
> N'appliquez pas sur une entrée de voie une tension supérieure à la limite de la spécification de l'appareil. Une tension trop élevée peut endommager l'analyseur logique.

> [!CAUTION]
> Les fils de masse de l'analyseur logique sont reliés à la masse de votre ordinateur par le câble USB. Connectez les fils de masse uniquement à la masse du circuit que vous mesurez. Si les deux masses ont des tensions différentes, un courant élevé peut circuler et endommager le circuit, l'analyseur logique et l'ordinateur.

## Configuration requise

Cet équipement est nécessaire :

- Un Mac avec macOS. Les notes de version donnent la version minimale de macOS.
- Un port USB. Un port USB 3.0 donne la vitesse la plus élevée. Un port USB 2.0 fonctionne aussi.
- Un analyseur logique DSLogic, son câble USB et son câble de sondes.

Vous pouvez utiliser l'application sans analyseur logique. L'appareil **Démo** produit des signaux de test. Vous pouvez aussi ouvrir un fichier de données d'une capture précédente.
