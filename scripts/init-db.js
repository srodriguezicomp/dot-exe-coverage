db = db.getSiblingDB("dhl_db");

function createCollectionIfNotExists(db, collectionName) {
  var collections = db.getCollectionNames();
  if (collections.indexOf(collectionName) === -1) {
    db.createCollection(collectionName);
    print("Collection created: " + collectionName);
  } else {
    print("Collection already exists: " + collectionName);
  }
}

// Crear colecciones si no existen
createCollectionIfNotExists(db, "users");
createCollectionIfNotExists(db, "inventory");
createCollectionIfNotExists(db, "shipments");