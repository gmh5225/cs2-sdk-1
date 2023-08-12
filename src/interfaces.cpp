#include "interfaces.hpp"
#include "utility/address.hpp"
#include "utility/memory.hpp"
#include "utility/pe.hpp"

#include <spdlog/spdlog.h>

#include <unordered_map>

#include <d3d11.h>

namespace interfaces {
    using InterfaceBind = std::pair<fnv1a::Hash, fnv1a::Hash>;
    using InterfaceBindMap = std::unordered_map<fnv1a::Hash, fnv1a::Hash>;

    struct InterfaceReg {
        void* (*create_interface_fn)();
        const char* name;
        InterfaceReg* next;
    };

    std::unordered_map<fnv1a::Hash, void*> interface_map;

    template <class T, std::size_t N>
    constexpr InterfaceBind bind_interface(const char(&name)[N]) {
        return { fnv1a::fnv_hash_const(name), fnv1a::fnv_hash_type<T>() };
    }

    void* get_interface_impl(const fnv1a::Hash hash) {
        return interface_map[hash];
    }

    void set_interface_impl(const fnv1a::Hash hash, void* ptr) {
        interface_map[hash] = ptr;
    }

    template <class T>
    constexpr void set_interface(void* ptr) {
        set_interface_impl(fnv1a::fnv_hash_type<T>(), ptr);
    }

    bool initialize() {
        {
            const auto client_mode_shared = memory::find_pattern(xorstr_(L"client.dll"), xorstr_("48 8D 0D ? ? ? ? 48 03 C1 48 83 C4")).abs().as<void*>();

            if (client_mode_shared == nullptr)
                return false;

            set_interface<sdk::ClientModeShared>(client_mode_shared);
        }

        {
            const auto csgo_input = memory::find_pattern(xorstr_(L"client.dll"), xorstr_("48 8D 0D ? ? ? ? E8 ? ? ? ? 33 C9 C7 05")).abs().as<void*>();

            if (csgo_input == nullptr)
                return false;

            set_interface<sdk::CSGOInput>(csgo_input);
        }

        {
            const auto cvar = memory::find_pattern(xorstr_(L"tier0.dll"), xorstr_("4C 8D 3D ? ? ? ? 0C 01")).abs().as<void*>();

            if (cvar == nullptr)
                return false;

            set_interface<sdk::CVar>(cvar);
        }

        {
            const auto swap_chain = memory::find_pattern(xorstr_(L"rendersystemdx11.dll"), xorstr_("48 8B 0D ? ? ? ? 48 85 C9 75 09"))
                .abs()
                .get()
                .add(0x8)
                .get(2)
                .add(0x178)
                .get<IDXGISwapChain*>();

            if (swap_chain == nullptr)
                return false;

            set_interface<IDXGISwapChain>(swap_chain);
        }

        const InterfaceBindMap interface_bind_map = {
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
                reinterpret_cast<std::uint64_t>(GetProcAddress(module_handle, xorstr_("CreateInterface")))
            );

            if (!create_interface_export)
                continue;

            if (*reinterpret_cast<std::uint8_t*>(create_interface_export.address()) != 0x4C)
                continue;

            auto interface_reg = create_interface_export.abs().get<InterfaceReg*>();

            if (interface_reg == nullptr)
                continue;

            const auto clean_interface_name = [](std::string& name) -> std::string {
                // Convert the name to lowercase.
                std::transform(name.begin(), name.end(), name.begin(), [](const auto& c) {
                    return std::tolower(c);
                });

                // Remove the first character if it's `v`.
                if (name.front() == 'v')
                    name.erase(name.begin());

                // Remove the last three characters.
                name.erase(name.end() - 3, name.end());

                // Remove the last character if it's `v` or `_`.
                while (name.back() == 'v' || name.back() == '_')
                    name.pop_back();

                return name;
            };

            while (interface_reg != nullptr) {
                void* interface_pointer = interface_reg->create_interface_fn();

                auto interface_name = std::string(interface_reg->name);

#ifdef _DEBUG
                const auto interface_pointer_rva = reinterpret_cast<std::uint64_t>(interface_pointer) - reinterpret_cast<std::uint64_t>(module_handle);

                #pragma warning(push)
                #pragma warning(disable: 4244)
                const auto module_name = std::string(module.name.begin(), module.name.end());
                #pragma warning(pop)

                spdlog::info(
                    "Found interface {} @ 0x{:X} ({} + 0x{:X}).",
                    interface_name,
                    reinterpret_cast<std::uint64_t>(interface_pointer),
                    module_name,
                    interface_pointer_rva
                );
#endif

                clean_interface_name(interface_name);

                if (const auto it = interface_bind_map.find(fnv1a::fnv_hash(interface_name.c_str())); it != interface_bind_map.end())
                    set_interface_impl(it->second, interface_pointer);

                interface_reg = interface_reg->next;
            }
        }

        return !interface_map.empty();
    }
}