# Plugins and components

## LIMA plugins mechanism

In Lima, plugins are shared libraries that can be loaded and linked dynamically.
This task is handled by the class `Lima::AmosePluginsManager`. It allows to add new [process units](../reference/process-units/index.md) or resources without having to recompile all LIMA executables.

### AmosePluginsManager
The class `Lima::AmosePluginsManager` is declared and defined in the following files:

* `lima_common/src/common/AbstractFactoryPattern/AmosePluginsManager.h`
* `lima_common/src/common/AbstractFactoryPattern/AmosePluginsManager.cpp`

It is exported to the library `lima-common-factory`.

The class `Lima::AmosePluginsManager` publicly inherits from `Singleton<AmosePluginsManager>` allowing it to exists in the form of a single instance at runtime.

An instance of that class is responsible for the plugins loading.
This task is done by the call of its unique method `loadPlugins()` in its constructor
at instanciation time.

What loadPlugins method does is to look for a plugins directory under the `$LIMA_CONF` folder.
Then, for each file found under plugins directory, it reads plugin names and deduces
the shared libraries that need to be loaded.
Once a shared library is identified, `Lima::AmosePluginsManager` delegates the task to
`DynamicLibrariesManager` so that it can actually load the library.

### Plugins files
Any Lima project (`lima_common`, `lima_linguisticprocessings`) can have a corresponding plugins file.
These files are automatically filled during project compilation step.
In order to declare a library as a LIMA plugin, the developer needs to use the LIMA cmake macro
`DECLARE_LIMA_PLUGIN` (defined in each project's root CMakeLists.txt). This macro appends 
a line containing the library name in the project plugins file, and add a cmake rule to create
a shared library (using the `add_library` command).

### DynamicLibrariesManager
The class `Lima::DynamicLibrariesManager` is defined in the following files:

* `lima_common/src/common/AbstractFactoryPattern/DynamicLibrariesManager.h`
* `lima_common/src/common/AbstractFactoryPattern/DynamicLibrariesManager.cpp`

It is exported to the library `lima-common-factory`.

The class `Lima::DynamicLibrariesManager` publicly inherits from `Singleton<DynamicLibrariesManager>` allowing it to exist in the form of a single instance at runtime.

An instance of that class is responsible for the dynamic loading of shared libraries that are passed to it. Indeed, it provides a method `loadLibrary(const std::string& libname)` for that task. 

This class use the Qt [`QLibrary`](http://doc.qt.io/qt-5/qlibrary.html) class to dynamically load libraries in a portable way. In order to prevent loading a library multiple times, it holds a map, called `m_handles`, where the key is the library name and the value is the instance of QLibrary class.

### Usage

```cpp
Lima::AmosePluginsManager::single();
```

Only one call is needed. As written above, the constructor of `AmosePluginsManager` will load the plugins found under `$LIMA_CONF/plugins`.

:warning: **Note:** This may actually have to change as all plugins are loaded even if some of them would not be used.

## LIMA Components

### Introduction and objectives

LIMA components are based on the "facory of factories" design pattern. Their goal is to design software modules that are independent from each others and that can easily be implemented either as local tool or as distant ones, like REST or CORBA services.

There is currently one such component in LIMA, the LIMA client, but other LVIC components (text and image index, search engine client…) use the same elements.

### Rules and Regulations

#### Independence

Components must be independent. That is, they can only use the libraries available in a common project, and the public APIs of other components. The public API of a component must be clearly identified, and must be independent of the rest of the component code (this is possible with the AbstractFactory pattern).

Each component will be the subject of a separate subproject. Common libraries are grouped together in the lima-common project.

#### Decoupling client / core of component

The component must be able to execute, at the request of the user, using a distant server (REST, WSDL, CORBA…), or as an integrated local tool. The core of the component must be completely independent of the way it is served, i.e. it is possible to compile a component without using a distant server.

#### Configuration

Each component is initialized from a configuration file (which can offer references to other files). This configuration file is in the Lima XML format (module / group / param / list / map). The XMLConfigurationFile library of the lima-common project offers all the tools to read this type of files, this libraries can be enriched.

#### initialization

Each component offers an entry point for its initialization. Initialization is the responsibility of the calling program, a component must never initialize itself. Similarly, a component must never initialize another component.

#### Public API

The public API of a component will be placed in a particular subproject. This API must contain at least the abstract class defining the component's client, and the main factory for configuring the component and creating clients. There may also be other objects, needed for interaction with the component, or result structures. The public part of a component must have NO DEPENDENCY with the core of the component.

### LIMA component skeleton

#### Organization of sub-projects

In this part: 'component' is the name of the LIMA component (for example: linguisticProcessing).

src / component: a subproject with the name of the component
src / component / client: Public API
src / component / client / AbstractComponentClient.h: definition of the client interface
src / component / client / ComponentClientFactory.h: main factory
src / component / core: client core definition
src / component / corba: definition of client corba
test / test programs

The public part of the component is located under src / component / client. Each client type is located in a separate, src / component / core subproject for the core client.

#### ClientFactory

In the lima-common projects: common / clientFactory / clientFactory.h is defined a client factory template. This template avoids rewriting the component factory in each component.

This template contains 2 classes: ClientFactory (abstract factory for a client) and MainClientFactory (mainframe).

##### ClientFactory <ComponentClient>

This class represents an abstract client object factory. For each type of client (core, corba), it is necessary to define a factory inheriting from ClientFactory <ComponentClient>, performing 2 operations: initialization and creation of the client. Initialization is called once before any client is created. Initialization must perform all necessary operations to create clients. The creation of a client can be called as many times as necessary, and must create a new client of the desired type.

Each factory must be a singleton, and register with the corresponding MainClientFactory.

##### MainClientFactory <ComponentClient>

This class is actually the main factory of the component's client. It allows you to initialize a component to create a particular client type, and create clients. (In practice it delegates these calls to the corresponding ClientFactory <ComponentClient>).

##### Benefits of abstract clients

An architecture with abstract clients has the advantage of being modular and flexible. The core client performs the requested processing. You can then create a corba client, whose role will only pass the call to a server, which will execute the call with a core client. Then, on the day when it is desired to use several servers, it is possible to write a third client, sending the call to all the servers and merging the results.
