var component;
var obj;

function createDialog() {
  createComponent("Simplex.qml");
}

function createComponent(src) {
    component = Qt.createComponent(src);
    if (component.status == Component.Ready) {
      deliverComponent();
    }
    else {
      component.statusChanged.connect(deliverComponent);
    }
}

function deliverComponent() {
    if (component.status === Component.Ready) {
      obj = component.createObject(app_window, {"x" : 100 + 21 * (app_window.pile%5), "y" : 100 + 21 * Math.round(app_window.pile/5)});
      app_window.pile++;
      if (obj === null) {
        console.log("something went wrong");
      }
    }
    else {
      console.log("error : " , component.errorString());
    }
}
