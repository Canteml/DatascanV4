# Logiciel DATASCANV4.x.x

Versions V4.x.x du logiciel d'inspection DatascanV4. Le logiciel est développé par Roboplanet.

Le logiciel prend en charge deux protocoles de communication :

## Environnement de compilation

IDE : Qt Creator 19.0.0

Qt : 6.10.3

Compilateur : MinGW 64 bits (kit *Desktop Qt 6.10.3 MinGW 64-bit*)

Système de build : **qmake**, avec le fichier `dev/DatascanV4.pro`

Fichier 'dev/CMakeLists.txt' incomplet : il ne lie aucune des bibliothèques externes. Le projet doit être compilé avec qmake.

### Modules Qt requis

`core`, `gui`, `quick`, `qml`, `xml`, `widgets`, `serialport`, `network`, `printsupport`, `multimedia`, `opengl`

Installez-les avec le Qt Maintenance Tool. `serialport` et `multimedia` se trouvent dans *Additional Libraries*.

## Bibliothèques nécessaires à la compilation

La compilation nécessite les bibliothèques suivantes. Elles doivent être présentes dans les dossiers indiqués, chemins relatifs à `dev/`.

|       Bibliothèque      | Version |       Dossier      |                                               Rôle                                              |
|---|---|---|---|
| UsKey                   |    —    | `UsKey/`           | Pilotes du module ultrason US-Key (DLL propriétaires)                                           |
| VLC-Qt (Core, Widgets)  |  1.2.1  | `lib/vlcqt/`       | Affichage du flux vidéo des caméras (en-têtes, bibliothèques, DLL et dossier `plugins` de VLC)  |
| libVLC SDK              |  3.0.23 | `vlc-sdk/sdk/`     | En-têtes et bibliothèques de libVLC (`vlc/vlc.h`), utilisés par `VlcVideoSink`                  |
| JKQtPlotter             |  Qt 6   | `lib/jkqtplotter/` | Graphiques A-Scan et B-Scan                                                                     |
| QXlsx                   |  1.5.0  | `lib/QXlsx/`       | Export Excel                                                                                    |
| SDL2                    |  2.0.12 | `lib/SDL2/`        | Gestion du joystick                                                                             |

À la compilation, qmake copie automatiquement les DLL nécessaires à côté de l'exécutable : UsKey, VLC-Qt, libVLC, les plugins VLC, JKQtPlotter et SDL2.


### Fichier de clé du protocole D4 (obligatoire)

Le fichier `dev/ext/fw_common/dev/src/v2/prot/ProtD4Key.h` contient la clé HMAC-SHA-256 utilisée pour l'authentification des appareils D4. **Il n'est pas versionné** : il est exclu par `.gitignore`.

Demandez la clé au responsable du projet, puis créez le fichier avec le contenu suivant :

```cpp
#ifndef PROTD4KEY_H
#define PROTD4KEY_H

static const char D4_HMAC_KEY_HEX[] = "<CLÉ EN HEXADÉCIMAL>";

#endif // PROTD4KEY_H
```

Ne publiez jamais ce fichier et ne le partagez jamais par un canal non sécurisé.


## Compilation


1. Clonez le dépôt.
2. Placez les bibliothèques dans les dossiers indiqués ci-dessus. Les sources tierces sont exclues de git et doivent être récupérées séparément.
3. Créez le fichier `ProtD4Key.h`.
4. Ouvrez `dev/DatascanV4.pro` dans Qt Creator.
5. Sélectionnez le kit *Desktop Qt 6.10.3 MinGW 64-bit*.
6. Lancez **Compiler**. Après un changement de version ou de branche, utilisez **Recompiler** pour forcer une compilation complète.


## Licences des composants tiers


|        Composant      |                                 Licence                                 |
|---|---|
|           Qt          |                         LGPL v3 / commerciale                           |
|  VLC, libVLC, VLC-Qt  |                                LGPL v2.1                                |
|       JKQtPlotter     |                                LGPL v2.1                                |
|         QXlsx         |                                   MIT                                   |
|          SDL2         |                                  zlib                                   |
|         UsKey         | Lecoeur Electronique (Ver64Bit, en cas d'absence, demander ExeUSB2x86)  |
