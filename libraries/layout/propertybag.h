#ifndef PROPERTYBAG_H
#define PROPERTYBAG_H

#include <functional>
#include <iostream>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include <memory>
#include <optional>

#include "color.h"

// Type alias for property values
using VariantPropertyBase = std::variant<int, float, std::string, bool, mssm::Color>;
using LazyVariant = std::function<VariantPropertyBase()>;
using VariantProperty = std::variant<int, float, std::string, bool, LazyVariant, mssm::Color>;

class PropertyBag {
public:
    // Enable or disable logging
    static void enableLogging(bool enable);

    // Set a property
    void setProperty(const std::string& key, const VariantProperty& value);

    // Get a property
    template<typename T>
    T getProperty(const std::string& key) const;

    // Check if a property exists
    bool hasProperty(const std::string& key) const;

    // Remove a property
    void removeProperty(const std::string& key);

    // Merge properties with optional override behavior
    void merge(const PropertyBag& other, bool overrideExisting = true);

    // Validate required properties
    void validate(const std::vector<std::string>& requiredKeys) const;

    // Debug print all properties
    void debugPrint() const;

    PropertyBag withModifications(const std::unordered_map<std::string, std::optional<VariantProperty>>& modifications) const;

    PropertyBag withModification(const std::string& key, const std::optional<VariantProperty>& value) const {
        return withModifications({{key, value}});
    }

private:
    std::unordered_map<std::string, VariantProperty> properties; // Map of properties
    static bool loggingEnabled; // Logging flag
};

// Get a property
template<typename T>
T PropertyBag::getProperty(const std::string& key) const {
    auto it = properties.find(key);
    if (it != properties.end()) {
        if (std::holds_alternative<LazyVariant>(it->second)) {
            // Resolve lazy evaluation
            if (loggingEnabled) {
                std::cout << "Resolving lazy property: " << key << "\n";
            }
            return std::get<T>(std::get<LazyVariant>(it->second)());
        }
        return std::get<T>(it->second);
    }
    throw std::runtime_error("Property not found or incorrect type: " + key);
}

class COWPropertyBag {
public:
    // Constructor
    explicit COWPropertyBag(const PropertyBag& original)
        : originalBag(&original), copyBag(nullptr) {}

    // Set a property (triggers copy-on-write)
    void setProperty(const std::string& key, const VariantProperty& value) {
        ensureCopy();
        copyBag->setProperty(key, value);
    }

    // Remove a property (triggers copy-on-write)
    void removeProperty(const std::string& key) {
        ensureCopy();
        copyBag->removeProperty(key);
    }

    // Get a property (reads from the original or the copy)
    template<typename T>
    const VariantProperty* getProperty(const std::string& key) const {
        if (copyBag) {
            return copyBag->getProperty<T>(key);
        }
        return originalBag->getProperty<T>(key);
    }

    // Check if a property exists
    bool hasProperty(const std::string& key) const {
        if (copyBag) {
            return copyBag->hasProperty(key);
        }
        return originalBag->hasProperty(key);
    }

    // Cast operator to get a const reference to the underlying PropertyBag
    operator const PropertyBag&() const {
        return copyBag ? *copyBag : *originalBag;
    }

private:
    const PropertyBag* originalBag;         // Reference to the original bag
    std::unique_ptr<PropertyBag> copyBag;  // Copy created on write

    // Ensure a copy of the original bag exists before modifying
    void ensureCopy() {
        if (!copyBag) {
            copyBag = std::make_unique<PropertyBag>(*originalBag);
        }
    }
};

#endif // PROPERTYBAG_H
