#ifndef DBBCONNECTOR_HPP
#define DBBCONNECTOR_HPP

#include <mongocxx/collection.hpp>
#include <bsoncxx/document/value.hpp>
#include <string>
#include <vector>

std::vector<bsoncxx::document::value> getMayorPrioritized(
    mongocxx::collection& collection,
    const std::vector<std::string>& fields,
    int minValue,
    const std::string& clientType,
    int limit
);

int insertUser(
    mongocxx::collection& collection,
    const std::string& user,
    const std::string& password,
    const std::string& type,
    const std::string& key,
    const std::string& iv
);

bsoncxx::document::value findUser(
    mongocxx::collection& collection,
    const std::string& user,
    const std::string& key,
    const std::string& iv
);

void upsertDocument(
    mongocxx::collection& collection,
    const std::string& matchField,
    const bsoncxx::types::bson_value::value& matchValue,
    const bsoncxx::document::view_or_value& newValues
);

void initializeCollections(
    mongocxx::collection& usersCollection,
    const std::string& key,
    const std::string& iv
);


void updateField(
    mongocxx::collection& collection,
    const std::string& matchField,
    const bsoncxx::types::bson_value::value& matchValue,
    const std::string& fieldToUpdate,
    const bsoncxx::types::bson_value::value& newValue
);

bsoncxx::document::value findShipment(
    mongocxx::collection& collection,
    const int& shipmentId
);

int generarNuevoId();


#endif 
