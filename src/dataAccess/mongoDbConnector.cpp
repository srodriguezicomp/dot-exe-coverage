#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/collection.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/pool.hpp>
#include <mongocxx/options/find.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <chrono>

#include <cryptopp/aes.h>
#include <cryptopp/modes.h>
#include <cryptopp/filters.h>
#include <cryptopp/base64.h>
#include <cryptopp/filters.h>

#include "mongoDbConnector.hpp"

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

std::string base64_encode(const std::string& input) {
    std::string encoded;
    CryptoPP::StringSource ss(reinterpret_cast<const unsigned char*>(input.data()), input.size(), true,
        new CryptoPP::Base64Encoder(
            new CryptoPP::StringSink(encoded),
            false // false = sin saltos de línea
        )
    );
    return encoded;
}

std::string base64_decode(const std::string& encoded) {
    std::string decoded;
    CryptoPP::StringSource ss(encoded, true,
        new CryptoPP::Base64Decoder(
            new CryptoPP::StringSink(decoded)
        )
    );
    return decoded;
}

std::string encryptAES(const std::string& plaintext, const std::string& key, const std::string& iv) {
    std::string cipher;
    CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption encryptor;
    encryptor.SetKeyWithIV(reinterpret_cast<const CryptoPP::byte*>(key.data()), key.size(), reinterpret_cast<const CryptoPP::byte*>(iv.data()));

    CryptoPP::StringSource ss(plaintext, true,
        new CryptoPP::StreamTransformationFilter(encryptor,
            new CryptoPP::StringSink(cipher)
        )
    );
    return base64_encode(cipher);
}

std::string decryptAES(const std::string& ciphertext, const std::string& key, const std::string& iv) {
    std::string recovered;
    CryptoPP::CBC_Mode<CryptoPP::AES>::Decryption decryptor;
    decryptor.SetKeyWithIV(reinterpret_cast<const CryptoPP::byte*>(key.data()), key.size(), reinterpret_cast<const CryptoPP::byte*>(iv.data()));

    CryptoPP::StringSource ss(base64_decode(ciphertext), true,
        new CryptoPP::StreamTransformationFilter(decryptor,
            new CryptoPP::StringSink(recovered)
        )
    );
    return recovered;
}

std::vector<bsoncxx::document::value> getMayorPrioritized(
    mongocxx::collection& collection,
    const std::vector<std::string>& fields,
    int minValue,
    const std::string& clientType,
    int limit
) {
    bsoncxx::builder::stream::document filter_builder;
    filter_builder << "clientType" << clientType;
    for (const auto& field : fields) {
        filter_builder << field << bsoncxx::builder::stream::open_document
                       << "$gt" << minValue
                       << bsoncxx::builder::stream::close_document;
    }

    bsoncxx::builder::stream::document sort_builder;
    for (const auto& field : fields) {
        sort_builder << field << -1;
    }

    mongocxx::options::find find_options;
    find_options.sort(sort_builder.view());
    find_options.limit(limit);

    auto cursor = collection.find(
        filter_builder.view(),
        find_options
    );

    std::vector<bsoncxx::document::value> documents;

    for (auto&& doc : cursor) {
        documents.emplace_back(bsoncxx::document::value(doc));
    }

    return documents;
}

int generarNuevoId() {
    // Obtener el tiempo actual desde Epoch (1970) en milisegundos
    auto now = std::chrono::system_clock::now();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

    // Convertir a int
    return static_cast<int>(millis % INT32_MAX); // Asegura que no exceda el máximo de int
}

int insertUser(
    mongocxx::collection& collection,
    const std::string& user,
    const std::string& password,
    const std::string& type,
    const std::string& key,
    const std::string& iv
) {
    std::string encryptedPassword = encryptAES(password, key, iv);
    int newId =  generarNuevoId();
    auto doc = make_document(
        kvp("id", newId),
        kvp("user", user),
        kvp("password", encryptedPassword),
        kvp("type", type)
    );

    collection.insert_one(doc.view());
    return newId;
}

bsoncxx::document::value findUser(
    mongocxx::collection& collection,
    const std::string& user,
    const std::string& key,
    const std::string& iv
) {
    auto maybe_result = collection.find_one(make_document(kvp("user", user)));
    if (!maybe_result) {
        return bsoncxx::builder::basic::make_document();
    }

    auto view = maybe_result->view();
    std::string decryptedPassword;

    if (view["password"] && view["password"].type() == bsoncxx::type::k_utf8) {
        decryptedPassword = decryptAES(view["password"].get_string().value.to_string(), key, iv);
    }
    std::cout << "Usuario encontrado: "<< view["user"].get_string().value.to_string() << std::endl;
    auto result_doc = make_document(
        kvp("id", view["id"].get_int32()),
        kvp("user", view["user"].get_string().value.to_string()),
        kvp("password", decryptedPassword),
        kvp("type", view["type"].get_string().value.to_string())
    );

    return result_doc;
}

void initializeCollections(
    mongocxx::collection& usersCollection,
    const std::string& key,
    const std::string& iv
) {
    auto maybe_admin = usersCollection.find_one(make_document(kvp("user", "admin")));

    if (!maybe_admin) {
        insertUser(usersCollection, "admin", "admin", "root", key, iv);
        std::cout << "Usuario admin insertado correctamente.\n";
    } else {
        std::cout << "Usuario admin ya existe.\n";
    }
}


void upsertDocument(
    mongocxx::collection& collection,
    const std::string& matchField,
    const bsoncxx::types::bson_value::value& matchValue,
    const bsoncxx::document::view_or_value& newValues
) {
    bsoncxx::document::value filter = bsoncxx::builder::stream::document{}
        << matchField << matchValue << bsoncxx::builder::stream::finalize;

    bsoncxx::document::value update = bsoncxx::builder::stream::document{}
        << "$set" << newValues << bsoncxx::builder::stream::finalize;

    mongocxx::options::update options;
    options.upsert(true);

    auto result = collection.update_one(filter.view(), update.view(), options);

    if (result && result->modified_count() > 0) {
        std::cout << "Documento actualizado correctamente." << std::endl;
    } else if (result && result->upserted_id()) {
        std::cout << "Documento insertado con ID: " << result->upserted_id()->get_oid().value.to_string() << std::endl;
    } else {
        std::cout << "No se realizó ninguna modificación." << std::endl;
    }
}

void updateField(
    mongocxx::collection& collection,
    const std::string& matchField,
    const bsoncxx::types::bson_value::value& matchValue,
    const std::string& fieldToUpdate,
    const bsoncxx::types::bson_value::value& newValue
) {
    // Crear el filtro para encontrar el documento
    bsoncxx::document::value filter = bsoncxx::builder::stream::document{}
        << matchField << matchValue << bsoncxx::builder::stream::finalize;

    // Crear la actualización para modificar el campo específico
    bsoncxx::document::value update = bsoncxx::builder::stream::document{}
        << "$set" << bsoncxx::builder::stream::open_document
        << fieldToUpdate << newValue
        << bsoncxx::builder::stream::close_document
        << bsoncxx::builder::stream::finalize;

    // Ejecutar la actualización
    auto result = collection.update_one(filter.view(), update.view());

    // Verificar el resultado de la operación
    if (result && result->modified_count() > 0) {
        std::cout << "Campo actualizado correctamente." << std::endl;
    } else {
        std::cout << "No se realizó ninguna modificación." << std::endl;
    }
}

bsoncxx::document::value findShipment(
    mongocxx::collection& collection,
    const int& shipmentId
) {
    auto maybe_result = collection.find_one(make_document(kvp("shipmentId", shipmentId)));
    if (!maybe_result) {
        return bsoncxx::builder::basic::make_document();
    }

    auto view = maybe_result->view();

    auto result_doc = make_document(
        kvp("clientIdSource", view["clientIdSource"].get_utf8().value.to_string()),
        kvp("clientIdDestiny", view["clientIdDestiny"].get_utf8().value.to_string()),
        kvp("inventory", make_document(
            kvp("foodBox", view["inventory"]["foodBox"].get_int32()),
            kvp("medicineBox", view["inventory"]["medicineBox"].get_int32()),
            kvp("waterBox", view["inventory"]["waterBox"].get_int32()),
            kvp("weaponryBox", view["inventory"]["weaponryBox"].get_int32())
        )),
        kvp("shipmentId", view["shipmentId"].get_int32()),
        kvp("status", view["status"].get_utf8().value.to_string())
    );

    return result_doc;
}
