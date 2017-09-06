# Récapitulatif Lima Gui

## C++

Classes principales :

* `LimaGuiApplication`
* `Threads`

Configuration de LIMA :

* `LimaConfiguration`
* `ConfigurationTree`
* `ConfigurationTreeModel`

Gestion des résultats :

* `ConllParser`
* `ConllListModel`
* `NamedEntitiesParser`

Autres outils :

* `FileTextExtractor`

## QML

Au sein du code QML, l'objet de la classe `LimaGuiApplication` est déclaré en tant que propriété contextuelle dans le fichier `main.cpp`, sous le nom `textAnalyzer`. Cela le rend accessible dans tout code QML utilisé dans le contexte de l'application.

Tous les fichiers QML et les ressources utilisées sont référencés dans le fichier `src/ressources.qrc`.

### Fenêtre principale

L'objet principal de l'application est l'objet `ApplicationWindow`déclaré dans le fichier main.qml. Comme dit précédemment, elle contient principalement les éléments suivants :

* Barre menu (`LimaGuiMenuBar.qml`) // inutilisée pour le moment
* Barre d'outils (`LimaGuiToolBar.qml`)
* Vue principale (`TabbedWorkspace.qml`)
* Barre d'analyse (`id:analysisBar`)
* Zone de messages logs (`id:logView`)

Les popups tels que l'interface de configuration de Lima (`LimaConfigurationView.qml`) sont déclarés au sein de la fenêtre principale. Ils sont simplement invisibles tant qu'ils ne sont pas ouverts par le click d'un bouton.

### Vue principale

La vue principale est composé de l'élément `TabbedWorkspace`, lequel utilise une `TabbedView` pour son système d'onglets.
Chaque onglet contient une `WorkView`. On utilise de l'allocation dynamique pour pouvoir créer des onglets au contenu dynamique. Une `WorkView` est divisée en deux parties : la vue des données (`id: data_view`) et la vue des résultats (`id: result_view`).

Par exemple, ouvrir un onglet pour un fichier ou ouvrir un onglet pour saisir du texte.
Ces deux fonctionnalités sont représentées par les widgets`AnalyzeFileWidget`et `AnalyzeTextWidget` respectivement. Quand on veut par exemple créer un nouvel onglet pour analyser du texte, on ajoute une nouvelle `WorkView` à la `TabbedView`, et on alloue dynamiquement ses deux vues via les fonctions `setDataView` et `setResultView` qui prennent le chemin du fichier décrivant le widget à insérer dans la vue concernée.

À l'ouverture d'un onglet, la vue des résultats est cachée par défaut tant qu'aucune analyse n'a été lancée.

#### Fermer un onglet

On se contente de fermer l'onglet via la fonction éponyme de l'objet `TabView`, mais cela semble avoir un comportement inattendu quant au fonctionnement de l'application.
See related issue

### Analyse

La barre d'analyse est liée à la fenêtre. Ses paramètres ne sont donc pas liés au `WorkView` actuel. Si on change d'onglet, les choix dans les menus déroulants ne changeront pas. Or, on pourrait vouloir  garder pour chaque onglet une configuration spécifique de ces choix 

    onglet 1 -> langue=fr,  config=config1; 
    onglet 2 -> langue=eng, config=config4 
    
sans avoir à rechanger à chaque fois. La manière la plus simple de faire cela serait d'inclure le widget correspondant dans le widget `WorkView`, ou d'essayer de mettre à jour des paramètres spécifiques pour l'objet `WorkView` concernant la langue et la configuration qui le concerne. (Il y a des ébauches de cela dans le code.)

L'utilisateur peut utiliser le raccourci ` Ctrl+Maj+A` pour lancer l'analyse de l'onglet courant (Ce raccourci est défini par l'objet `Shortcut` dans le fichier `main.qml`). 
Le contenu est récupéré depuis l'objet `WorkView` via `getDataView`. La fonction `indiscriminateAnalyze` du fichier `main.qml` regarde alors le type du `WorkView` actuel (Text, OpenFile, etc.) et appelle la fonction de `LimaGuiApplication`, aka `textAnalyzer`qui correspond.

Une référence vers la vue des résultats du `WorkView`est envoyé en paramètre. Cette vue  est notifiée une fois l'analyse terminée et en reçoit les résultats via la fonction `displayResults`. Selon le format sélectionné, la vue correspondante reçoit les données à sont tour et utilise l'interpéteur qui lui est associé (exemple : `NamedEntitiesView`  -> `NamedEntitiesParser`).

Pour les menus déroulants :

Les langues sont liées à celles déclarées à l'initialisation de LIMA
(fonction `LimaGuiApplication::initializeLimaAnalyzer`). 

Le choix de configuration n'a pour le moment aucun effet, mais il devrait ensuite faire référence aux configurations enregistrées dans l'application par leur nom (default, etc.). 

Les formats sont propres à l'application graphique et sont déclarés en dur dans l'objet `SelectOptionComboBox; id: format_selector` et le fichier `ResultView.qml`.

### Interface de configuration

Cf. `src\treeview` pour les exemples de TreeModel / TreeView.

### qml/basics

Ce répertoire contient des widgets plus génériques, comme `TextEditor`, `TextView` et `SelectOptionComboBox`.

### qml/scripts

Les fonctions javascript globales sont stockés ici. Il est intéressant de noter que variables globales des scripts javascript ont une portée limitée au fichier QML qui a inclus ce script.

* `DynamicObjectManager.js` contient des fonctions permettant l'allocation dynamique d'objets.
* `colors.js`contient quelques fonctions pour gérer des couleurs.

### qml/resources

Ce répertoire contient les ressources du projet, et notamment les images utilisées dans l'interface. Ces images devraient être remplacées par des images libres de droit ou avec une licence compatible avec celle de l'application par le futur.

### qml/styles

Ce répertoire contient les objets de style, plutôt que de les stocker directement dans l'objet concerné.
