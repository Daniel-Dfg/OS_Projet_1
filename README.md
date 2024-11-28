# OS_Projet_1
 Projet pour le cours d'OS (INFO-F201), ULB, novembre 2024

## Utilisation
Exécutez `chat pseudo_utilisateur pseudo_destinataire [--bot] [--manuel]` dans deux terminaux séparés pour lancer une conversation entre deux utilisateurs (ou entre un utilisateur et un bot)

**Exemple d'utilisation (2 utilisateurs) :**
```bash
#terminal 1
chat A B

#terminal 2
chat B A
```

### Arguments optionnels

- `bot` : vous permet d'initier une conversation avec un bot, dont le nom sera le `pseudo_destinataire` fourni.
- `manuel` : n'affiche pas les messages reçus immédiatement à la réception, mais envoie plutôt une notification sonore. Pour afficher les messages "en attente" (reçus, mais non affichés), entrez `Ctrl + C` ou envoyez vous-même un message. Les messages en attente sont stockés dans une "mémoire" : si leur taille excède la taille de cette mémoire, alors les messages à l'intérieur seront affichés (vidant donc la mémoire).


## Sources
  Nous vous invitons à consulter notre [rapport](https://github.com/Daniel-Dfg/OS_Projet_1/blob/main/rapport.pdf) pour comprendre les intentions et spécificités derrière le développement de notre application.
