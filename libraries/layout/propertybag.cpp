#include <iostream>
#include <string>
#include <optional>

#include "propertybag.h"

// Static member initialization
bool PropertyBag::loggingEnabled = false;

void PropertyBag::enableLogging(bool enable)
{
    loggingEnabled = enable;
}

void PropertyBag::setProperty(const std::string &key, const VariantProperty &value)
{
    if (loggingEnabled) {
        std::cout << "Setting property: " << key << "\n";
    }
    properties[key] = value;
}

bool PropertyBag::hasProperty(const std::string &key) const
{
    return properties.find(key) != properties.end();
}

void PropertyBag::removeProperty(const std::string &key)
{
    if (loggingEnabled) {
        std::cout << "Removing property: " << key << "\n";
    }
    properties.erase(key);
}

void PropertyBag::merge(const PropertyBag &other, bool overrideExisting)
{
    for (const auto &[key, value] : other.properties) {
        if (overrideExisting || !hasProperty(key)) {
            if (loggingEnabled) {
                std::cout << "Merging property: " << key << "\n";
            }
            properties[key] = value;
        }
    }
}

void PropertyBag::validate(const std::vector<std::string> &requiredKeys) const
{
    for (const auto &key : requiredKeys) {
        if (!hasProperty(key)) {
            throw std::runtime_error("Missing required property: " + key);
        }
    }
}

void PropertyBag::debugPrint() const
{
    for (const auto &[key, value] : properties) {
        std::cout << "Property: " << key << " = ";
        if (std::holds_alternative<int>(value)) {
            std::cout << std::get<int>(value);
        } else if (std::holds_alternative<float>(value)) {
            std::cout << std::get<float>(value);
        } else if (std::holds_alternative<std::string>(value)) {
            std::cout << std::get<std::string>(value);
        } else if (std::holds_alternative<bool>(value)) {
            std::cout << (std::get<bool>(value) ? "true" : "false");
        } else if (std::holds_alternative<LazyVariant>(value)) {
            std::cout << "[Lazy property]";
        }
        std::cout << "\n";
    }
}

// Create a modified copy of the PropertyBag
PropertyBag PropertyBag::withModifications(
    const std::unordered_map<std::string, std::optional<VariantProperty>> &modifications) const
{
    PropertyBag copy;             // Create an empty copy
    copy.properties = properties; // Copy existing properties in one operation

    for (const auto &[key, value] : modifications) {
        if (value.has_value()) {
            copy.properties[key] = value.value(); // Add or update property
        } else {
            copy.properties.erase(key); // Remove property
        }
    }

    return copy;
}

