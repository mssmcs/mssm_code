#ifndef VULKUTIL_H
#define VULKUTIL_H

#include "volk.h"
#include <cstdint>
#include <vector>
#include <functional>
#include <memory>

template<typename OBJ, typename T, typename COUNT = uint32_t>
std::vector<T> getVulkArray(OBJ obj, std::function<void(OBJ, COUNT *, T *)> func)
{
    COUNT count = 0;
    func(obj, &count, nullptr);
    std::vector<T> result(count);
    if (count > 0) {
        func(obj, &count, result.data());
    }
    return result;
}

template<typename OBJ1, typename OBJ2, typename T, typename COUNT = uint32_t>
std::vector<T> getVulkArray(OBJ1 obj1, OBJ2 obj2, std::function<void(OBJ1, OBJ2, COUNT *, T *)> func)
{
    COUNT count = 0;
    func(obj1, obj2, &count, nullptr);
    std::vector<T> result(count);
    if (count > 0) {
        func(obj1, obj2, &count, result.data());
    }
    return result;
}

template <typename OBJ, typename T>
std::vector<T> getVulkArray(OBJ obj, std::function<void(OBJ, unsigned int*, T*)> func)
{
    return getVulkArray<OBJ, T, unsigned int>(obj, func);
}

template <typename OBJ1, typename OBJ2, typename T>
std::vector<T> getVulkArray(OBJ1 obj1, OBJ2 obj2, std::function<void(OBJ1, OBJ2, unsigned int*, T*)> func)
{
    return getVulkArray<OBJ1, OBJ2, T, unsigned int>(obj1, obj2, func);
}


// template <typename CTXT, typename T>
// class VulkUnique {
//     struct Inner {
//         T obj;
//         CTXT context;
//         std::function<void(CTXT, T, const VkAllocationCallbacks*) > destroyer;
//         Inner(CTXT context, T obj, std::function<void(CTXT, T, const VkAllocationCallbacks*) > destroyer) : context{context}, obj{obj}, destroyer{destroyer} {}
//         ~Inner() {
//             destroyer(context, obj, nullptr);
//         }
//     };
//     std::unique_ptr<Inner> inner;
// public:
//     VulkUnique() : inner{} {}
//     VulkUnique(CTXT context, T obj, std::function<void(CTXT, T, const VkAllocationCallbacks*) > destroyer) : inner{std::make_unique<Inner>(context, obj, destroyer)} {}
//     operator T() { return inner->obj; }
// };

// template <typename CTXT, typename T>
// class VulkShared {
//     struct Inner {
//         T obj;
//         CTXT context;
//         std::function<void(CTXT, T, const VkAllocationCallbacks*) > destroyer;
//         Inner(CTXT context, T obj, std::function<void(CTXT, T, const VkAllocationCallbacks*) > destroyer) : context{context}, obj{obj}, destroyer{destroyer} {}
//         ~Inner() {
//             destroyer(context, obj, nullptr);
//         }
//     };
//     std::shared_ptr<Inner> inner;
// public:
//     VulkShared() : inner{} {}
//     VulkShared(CTXT context, T obj, std::function<void(CTXT, T, const VkAllocationCallbacks*) > destroyer) : inner{std::make_shared<Inner>(context, obj, destroyer)} {}
//     operator T() { return inner->obj; }

// };



#endif // VULKUTIL_H
