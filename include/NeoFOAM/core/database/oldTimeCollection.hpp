// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2024 NeoFOAM authors
#pragma once

#include <unordered_map>
#include <any>
#include <string>
#include <functional>
#include "NeoFOAM/core/demangle.hpp"
#include "NeoFOAM/core/error.hpp"

#include "NeoFOAM/core/database/database.hpp"
#include "NeoFOAM/core/database/collection.hpp"
#include "NeoFOAM/core/database/document.hpp"

#include "NeoFOAM/finiteVolume/cellCentred/fields/volumeField.hpp"
#include "NeoFOAM/core/database/fieldCollection.hpp"


namespace NeoFOAM::finiteVolume::cellCentred
{

class OldTimeDocument
{
public:

    OldTimeDocument(const Document& doc);

    OldTimeDocument(
        std::string nextTime, std::string previousTime, std::string currentTime, int32_t level
    );

    std::string& nextTime();

    const std::string& nextTime() const;

    std::string& previousTime();

    const std::string& previousTime() const;

    std::string& currentTime();

    const std::string& currentTime() const;

    int32_t& level();

    const int32_t& level() const;

    Document& doc();

    const Document& doc() const;

    std::string id() const;

    static std::string typeName();

private:

    Document doc_;
};


class OldTimeCollection : public CollectionMixin<OldTimeDocument>
{
public:

    OldTimeCollection(Database& db, std::string name, std::string fieldCollectionName);

    bool contains(const std::string& id) const;

    bool insert(const OldTimeDocument& cc);

    std::string findNextTime(std::string id) const;

    std::string findPreviousTime(std::string id) const;

    OldTimeDocument& oldTimeDoc(const std::string& id);

    const OldTimeDocument& oldTimeDoc(const std::string& id) const;

    template<typename FieldType>
    FieldType& getOrInsert(std::string IdOfNextField)
    {
        std::string nextId = findNextTime(IdOfNextField);
        FieldCollection& fieldCollection = FieldCollection::instance(db(), fieldCollectionName_);

        if (nextId != "") // oldField is already registered
        {
            OldTimeDocument& oldTimeDoc = oldTimeDoc(nextId);
            return fieldCollection.fieldDoc(oldTimeDoc.previousTime()).field<FieldType>();
        }
        FieldDocument& fieldDoc = fieldCollection.fieldDoc(IdOfNextField);

        std::string oldTimeName = fieldDoc.field<FieldType>().name + "_0";
        FieldType& oldField =
            fieldCollection.registerField<FieldType>(CreateFromExistingField<FieldType> {
                .name = oldTimeName,
                .field = fieldDoc.field<FieldType>(),
                .timeIndex = fieldDoc.timeIndex() - 1,
                .iterationIndex = fieldDoc.iterationIndex(),
                .subCycleIndex = fieldDoc.subCycleIndex()
            });
        OldTimeDocument oldTimeDoc(fieldDoc.field<FieldType>().key, oldField.key, "", -1);
        setCurrentFieldAndLevel(oldTimeDoc);
        insert(oldTimeDoc);
        return oldField;
    }

    template<typename FieldType>
    const FieldType& get(std::string IdOfNextField) const
    {
        std::string nextId = findNextTime(IdOfNextField);
        const FieldCollection& fieldCollection =
            FieldCollection::instance(db(), fieldCollectionName_);

        if (nextId != "") // oldField has to be registered
        {
            const OldTimeDocument& oldTimeDoc = oldTimeDoc(nextId);
            return fieldCollection.fieldDoc(oldTimeDoc.previousTime()).field<FieldType>();
        }
        else
        {
            NF_THROW("Old field not found");
        }
    }

    static OldTimeCollection&
    instance(Database& db, std::string name, std::string fieldCollectionName);

    static const OldTimeCollection& instance(const Database& db, std::string name);

    static OldTimeCollection& instance(FieldCollection& fieldCollection);

    static const OldTimeCollection& instance(const FieldCollection& fieldCollection);

private:

    /** */
    void setCurrentFieldAndLevel(OldTimeDocument& oldTimeDoc);

    std::string fieldCollectionName_;
};

/**
 * @brief Retrieves the old time field of a given field.
 *
 * This function retrieves the old time field of a given field
 *
 * @param field The field to retrieve the old time field from.
 * @return The old time field.
 */
template<typename FieldType>
FieldType& oldTime(FieldType& field)
{
    FieldCollection& fieldCollection = FieldCollection::instance(field);
    OldTimeCollection& oldTimeCollection = OldTimeCollection::instance(fieldCollection);

    FieldDocument& fieldDoc = fieldCollection.fieldDoc(field.key);

    return oldTimeCollection.getOrInsert<FieldType>(field.key);
}

/**
 * @brief Retrieves the old time field of a given field (const version).
 *
 * This function retrieves the old time field of a given field
 *
 * @param field The field to retrieve the old time field from.
 * @return The old time field.
 */
template<typename FieldType>
const FieldType& oldTime(const FieldType& field)
{
    const FieldCollection& fieldCollection = FieldCollection::instance(field);
    const OldTimeCollection& oldTimeCollection = OldTimeCollection::instance(fieldCollection);

    const FieldDocument& fieldDoc = fieldCollection.fieldDoc(field.key);

    return oldTimeCollection.get<FieldType>(field.key);
}

} // namespace NeoFOAM
