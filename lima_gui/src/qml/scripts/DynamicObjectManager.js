/// current component
var component;

/// parent of current object
var parent;

/// all dynamically created objects in this script
var objects = [];

/// current object and last created
var obj;

/// resize parent to fit the element
var fit = false;

/// success in creating the object
var success = true;

var verbose = true;

/// example
function createDialog() {
  createComponent("Simplex.qml", app_window);
  /// app_window being the identifier of the Window object
}

/// This function creates a component from specified source
/// argument :p: is the parent to attach the resulting object to
/// path is relative to this file
function createComponent(src, p, b) {
    component = Qt.createComponent("../" + src);
    fit = b;
    parent = p;
    if (component.status == Component.Ready) {
      deliverComponent();
    }
    else {
      ///
      /// while (component.status != Component.Ready);
      /// deliverComponent();
      ///
      /// return;
      ///
      if (verbose) console.log("Component is not yet ready");
      component.statusChanged.connect(deliverComponent);
    }
}

/// from QML text
function createObject(src, parent, filepath) {
  Qt.createQmlObject(src, parent, filepath);
}

/// In case the component is not ready yet, we can delay the object creation
function deliverComponent() {
  if (component.status === Component.Ready) {
    if(verbose) console.log("Component is ready");
    obj = component.createObject(parent, {"x": 50,"y": 50});
    app_window.pile++;
    if (obj === null) {
      if (verbose) console.log("something went wrong");
      success = false;
    }
    else {
      objects.push(obj);
      if (fit) {
        parent.width += obj.width;
        parent.height += obj.height;
        fit = false;
      }
      success = true;
      checkSum();
    }
  }
  else {
    if (verbose) console.log("xerror : " , component.errorString());
    success = false;
  }
}

/// Deletes all dynamically created objects (by this script)
function clear() {
  for (var i = 0; i<objects.length; i++) {
    objects[i].destroy()
  }
  objects = [];
}

/// Deletes object
function deleteObject(ob) {
  var id = objects.indexOf(ob);
  if (id >= 0) {
    if (id == objects.length - 1 && objects.length > 1) {
      obj = objects[objects.length - 2];
    }
    objects.splice(id, 1);
  }
  ob.destroy();
}

/// Deletes last object, and updates current obj reference
function pop() {
  popObject()
}

function popObject() {
  obj.color = "red";
  if (objects.length > 0) {
    objects.pop();
    obj.destroy();
  }
  if (objects.length > 0) {
    obj = objects[objects.length - 1];
  }
}

/// Deletes :nb: last objects
function popMultipleObjects(nb) {
  for (var i = 0; i < nb && objects.length; i++) {
    popObject();
  }
}

function indexOf(o) {
  for (var i = 0; i<objects.length; i++) {
    if (objects[i] == o) {
      return i;
    }
  }
  return -1;
}

function checkSum() {
  if (parent != obj.parent) {
    console.log("parent(", parent, ") is not obj.parent(", obj.parent, ")");
  }
}
