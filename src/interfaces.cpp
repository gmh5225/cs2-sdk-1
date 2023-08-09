#include "interfaces.hpp"
#include "utility/address.hpp"
#include "utility/memory.hpp"
#include "utility/pe.hpp"

#include <algorithm>
#include <unordered_map>

#include <Windows.h>

namespace interfaces {
    using interface_bind_t = std::pair<fnv1a::hash_t, fnv1a::hash_t>;
    using interface_bind_map_t = std::unordered_map<fnv1a::hash_t, fnv1a::hash_t>;

    struct InterfaceReg {
        void* (*create_interface_fn)();
        const char* name;
        InterfaceReg* next;
    };

    std::unordered_map<fnv1a::hash_t, void*> interface_map;

    template <class T, std::size_t N>
    constexpr interface_bind_t bind_interface(const char(&name)[N]) {
        return { fnv1a::fnv_hash_const(name), fnv1a::fnv_hash_type<T>() };
    }

    void* get_interface_impl(const fnv1a::hash_t hash) {
        return interface_map[hash];
    }

    void set_interface_impl(const fnv1a::hash_t hash, void* ptr) {
        interface_map[hash] = ptr;
    }

    template <class T>
    constexpr void set_interface(void* ptr) {
        set_interface_impl(fnv1a::fnv_hash_type<T>(), ptr);
    }

    bool initialize() {
        {
            const auto client_mode_shared = memory::find_pattern(L"client.dll", "48 8D 0D ? ? ? ? 48 03 C1 48 83 C4").abs().as<void*>();

            if (client_mode_shared == nullptr)
                return false;

            set_interface<sdk::ClientModeShared>(client_mode_shared);
        }

        {
            const auto csgo_input = memory::find_pattern(L"client.dll", "48 8D 0D ? ? ? ? E8 ? ? ? ? 33 C9 C7 05").abs().as<void*>();

            if (csgo_input == nullptr)
                return false;

            set_interface<sdk::CSGOInput>(csgo_input);
        }

        const interface_bind_map_t interface_bind_map = {
            bind_interface<sdk::GameResourceService>("gameresourceservice"),
            bind_interface<sdk::Localize>("localize"),
            bind_interface<sdk::ResourceSystem>("resourcesystem"),
            bind_interface<sdk::SchemaSystem>("schemasystem"),
            bind_interface<sdk::Source2Client>("source2client"),
            bind_interface<sdk::EngineClient>("source2enginetoclient")
        };

        const auto modules = pe::get_loaded_modules();

        if (!modules)
            return false;

        for (const auto& module : *modules) {
            const HMODULE module_handle = GetModuleHandleW(module.name.c_str());

            if (module_handle == nullptr)
                continue;

            const auto create_interface_export = Address(
                reinterpret_cast<std::uint64_t>(GetProcAddress(module_handle, "CreateInterface"))
            );

            if (!create_interface_export)
                continue;

            if (*reinterpret_cast<std::uint8_t*>(create_interface_export.address()) != 0x4C)
                continue;

            auto interface_reg = create_interface_export.abs().get<InterfaceReg*>();

            if (interface_reg == nullptr)
                continue;

            const auto get_interface_name = [](const InterfaceReg* interface_reg) -> std::string {
                auto interface_name = std::string(interface_reg->name);

                // Convert the interface name to lowercase.
                std::ranges::transform(interface_name, interface_name.begin(), [](const auto& c) {
                    return std::tolower(c);
                });

                // Remove the first character from the interface name if it is 'v'.
                if (interface_name.front() == 'v')
                    interface_name.erase(interface_name.begin());

                // Remove the last three characters from the interface name.
                if (interface_name.size() >= 3)
                    interface_name.erase(interface_name.end() - 3, interface_name.end());

                // Remove the last character from the interface name if it is 'v'.
                if (interface_name.back() == 'v')
                    interface_name.pop_back();

                return interface_name;
            };

            while (interface_reg != nullptr) {
                const auto interface_name = get_interface_name(interface_reg);

                if (const auto it = interface_bind_map.find(fnv1a::fnv_hash(interface_name.c_str())); it != interface_bind_map.end())
                    set_interface_impl(it->second, interface_reg->create_interface_fn());

                interface_reg = interface_reg->next;
            }
        }

        return !interface_map.empty();
    }
}