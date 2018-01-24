# Recap Lima Gui

## C ++

Main Classes:

* `LimaGuiApplication`
* `Threads`

Configuring LIMA:

* `LimaConfiguration`
* `ConfigurationTree`
* `ConfigurationTreeModel`

Results management:

* `ConllParser`
* `ConllListModel`
* `NamedEntitiesParser`

Other tools:

* `FileTextExtractor`

## QML

Within the QML code, the object of the `LimaGuiApplication` class is declared 
as a contextual property in the` main.cpp` file, under the name `textAnalyzer`. 
This makes it accessible in any QML code used in the context of the application.

All QML files and resources are referenced in the `src/resources.qrc` file.

### Main Window

The main object of the application is the `ApplicationWindow` object that is 
declared in the `main.qml` file. As stated above, it mainly contains the 
following elements:

* Menu bar (`LimaGuiMenuBar.qml`) // not currently used
* Toolbar (`LimaGuiToolBar.qml`)
* Main view (`TabbedWorkspace.qml`)
* Analysis bar (`id: analysisBar`)
* Logs message area (`id: logView`)

Popups such as the Lima configuration interface (`LimaConfigurationView.qml`) 
are declared within the main window. They are simply invisible as long as they 
are not opened by the click of a button.

### Main view

The main view consists of the `TabbedWorkspace` element, which uses a 
`TabbedView` for its tab system. Each tab contains a `WorkView`. Dynamic 
allocation is used to create tabs with dynamic content. A WorkView is divided 
into two parts: the data view (`id: data_view`) and the result view 
(`id: result_view`).

For example, open a tab for a file or open a tab to enter text.
These two features are represented by the `AnalyzeFileWidget` and 
`AnalyzeTextWidget` widgets respectively. For example, if you want to create a 
new tab to analyze text, you add a new WorkView to the `TabbedView`, and you 
dynamically allocate its two views via the `setDataView` and `setResultView` 
functions that take the path of the file describing the widget to be inserted 
in the relevant view.

When a tab is opened, the results view is hidden by default until an analysis 
has been initiated.

#### Close a tab

We just close the tab via the eponymous function of the `TabView` object.

### Analysis

The analysis bar is linked to the window. Its parameters are therefore not 
linked to the current WorkView. If you change the tab, the choices in the 
drop-down menus will not change. However, one might want to keep for each tab 
a specific configuration of these choices

    tab 1 -> language = fr, config = config1;
    tab 2 -> language = eng, config = config4
    
without having to replace each time. The easiest way to do this would be to 
include the corresponding widget in the `WorkView` widget, or to try to update 
specific settings for the` WorkView` object regarding the language and 
configuration for it. (There are sketches of this in the code.)

The user can use the shortcut `Ctrl+Shift+A` to start the analysis of the 
current tab (This shortcut is defined by the `Shortcut` object in the 
`main.qml` file).
The content is retrieved from the `WorkView` object via `getDataView`. The 
`indiscriminateAnalyze` function of the `main.qml` file then looks at the type 
of the current `WorkView` (Text, OpenFile, etc.) and calls the 
`LimaGuiApplication` function, aka `textAnalyzer` which matches.

A reference to the results view of the `WorkView` is sent as a parameter. This 
view is notified when the analysis is complete and receives the results via 
the `displayResults` function. Depending on the format selected, the 
corresponding view receives the data in turn and uses the associated 
interpreter (example: `NamedEntitiesView` ->` NamedEntitiesParser`).

For drop-down menus:

The languages ​​are related to those declared at the initialization of LIMA
(function `LimaGuiApplication::initializeLimaAnalyzer`).

The configuration choice has no effect for the moment, but it should then refer 
to the configurations saved in the application by their name (default, etc.).

The formats are specific to the graphics application and are hard-coded in the 
object `SelectOptionComboBox; id: format_selector` and the file 
`ResultView.qml`.

### Configuration interface

See `src/treeview` for examples of TreeModel/TreeView.

### qml / basics

This directory contains more generic widgets, such as `TextEditor`,` TextView` 
and `SelectOptionComboBox`.

### qml / scripts

The global javascript functions are stored here. It is interesting to note that 
global variables of javascript scripts have a limited scope to the QML file 
that included this script.

* `DynamicObjectManager.js` contains functions for dynamic allocation of 
objects.
* `colors.js` contains some functions to manage colors.

### qml / resources

This directory contains the resources of the project, and in particular the 
images used in the interface.

### qml / styles

This directory contains style objects, rather than storing them directly in the 
object.

