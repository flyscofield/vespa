// Copyright 2017 Yahoo Holdings. Licensed under the terms of the Apache 2.0 license. See LICENSE in the project root.

#pragma once

#include "reference_attribute.h"
#include <vespa/searchcommon/attribute/iattributevector.h>
#include <vespa/vespalib/stllike/string.h>
#include <memory>

namespace search {

class AttributeGuard;
class AttributeEnumGuard;

namespace attribute {

/**
 * Attribute vector which does not store values of its own, but rather serves as a
 * convenient indirection wrapper towards a target vector, usually in another
 * document type altogether. Imported attributes are meant to be used in conjunction
 * with a reference attribute, which specifies a dynamic mapping from a local LID to
 * a target LID (via an intermediate GID).
 *
 * Any accessor on the imported attribute for a local LID yields the same result as
 * if the same accessor were invoked with the target LID on the target attribute vector.
 */
class ImportedAttributeVector : public IAttributeVector {
public:
    using SP = std::shared_ptr<ImportedAttributeVector>;
    ImportedAttributeVector(vespalib::stringref name,
                            std::shared_ptr<ReferenceAttribute> reference_attribute,
                            std::shared_ptr<AttributeVector> target_attribute);
    ~ImportedAttributeVector();

    const vespalib::string & getName() const override;
    uint32_t getNumDocs() const override;
    uint32_t getValueCount(uint32_t doc) const override;
    uint32_t getMaxValueCount() const override;
    largeint_t getInt(DocId doc) const override;
    double getFloat(DocId doc) const override;
    const char * getString(DocId doc, char * buffer, size_t sz) const override;
    EnumHandle getEnum(DocId doc) const override;
    uint32_t get(DocId docId, largeint_t * buffer, uint32_t sz) const override;
    uint32_t get(DocId docId, double * buffer, uint32_t sz) const override;
    uint32_t get(DocId docId, const char ** buffer, uint32_t sz) const override;
    uint32_t get(DocId docId, EnumHandle * buffer, uint32_t sz) const override;
    uint32_t get(DocId docId, WeightedInt * buffer, uint32_t sz) const override;
    uint32_t get(DocId docId, WeightedFloat * buffer, uint32_t sz) const override;
    uint32_t get(DocId docId, WeightedString * buffer, uint32_t sz) const override;
    uint32_t get(DocId docId, WeightedConstChar * buffer, uint32_t sz) const override;
    uint32_t get(DocId docId, WeightedEnum * buffer, uint32_t sz) const override;
    bool findEnum(const char * value, EnumHandle & e) const override;
    const char * getStringFromEnum(EnumHandle e) const override;
    std::unique_ptr<ISearchContext> createSearchContext(std::unique_ptr<QueryTermSimple> term,
                                                        const SearchContextParams &params) const override;
    const IDocumentWeightAttribute *asDocumentWeightAttribute() const override;
    BasicType::Type getBasicType() const override;
    size_t getFixedWidth() const override;
    CollectionType::Type getCollectionType() const override;
    bool hasEnum() const override;

    const std::shared_ptr<ReferenceAttribute>& getReferenceAttribute() const noexcept {
        return _reference_attribute;
    }
    const std::shared_ptr<AttributeVector>& getTargetAttribute() const noexcept {
        return _target_attribute;
    }

    /**
     * Acquire an opaque composite guard that covers both the target attribute and
     * the reference attribute. Note that these are not directly accessible via the
     * returned guard object itself; it does not expose a valid pointer (i.e. get() will
     * return nullptr).
     */
    std::unique_ptr<AttributeGuard> acquireGuard() const;
    /**
     * Acquires a composite guard similar to acquireGuard(), but the target attribute is
     * covered by an AttributeEnumGuard instead of a regular AttributeGuard.
     *
     * The reference attribute is _not_ covered by an enum guard.
     */
    std::unique_ptr<AttributeEnumGuard> acquireEnumGuard() const;

    /*
     * Create an imported attribute with a snapshot of lid to lid mapping.
     */
    std::unique_ptr<ImportedAttributeVector> makeReadGuard(bool stableEnumGuard) const;
protected:
    long onSerializeForAscendingSort(DocId doc, void * serTo, long available,
                                     const common::BlobConverter * bc) const override;
    long onSerializeForDescendingSort(DocId doc, void * serTo, long available,
                                      const common::BlobConverter * bc) const override;


    vespalib::string                    _name;
    std::shared_ptr<ReferenceAttribute> _reference_attribute;
    std::shared_ptr<AttributeVector>    _target_attribute;
};

} // attribute
} // search
