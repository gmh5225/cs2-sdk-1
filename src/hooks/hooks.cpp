#include "hooks/hooks.hpp"

#include <iostream>
#include <ranges>
#include <unordered_map>

namespace hooks {
    using hook_interface_pair_t = std::pair<std::uint16_t, void*>;
    using hook_interface_list_t = std::vector<hook_interface_pair_t>;
    using hook_interface_entry_t = std::pair<fnv1a::hash_t, hook_interface_list_t>;
    using hook_interface_map_t = std::vector<hook_interface_entry_t>;

    template <class T>
    struct HookInterface : hook_interface_entry_t {
        constexpr HookInterface(const std::initializer_list<hook_interface_pair_t>& hooks)
            : hook_interface_entry_t(fnv1a::fnv_hash_type<T>(), hooks)
        {}
    };

    std::unordered_map<fnv1a::hash_t, VMTHook> vmt_hook_map;

    hook_interface_map_t hook_interface_map = {
        HookInterface<sdk::ClientModeShared> {
            { static_cast<std::uint16_t>(VTableIndex::LevelInit), reinterpret_cast<void*>(&level_init_hook) }
        },

        HookInterface<sdk::CSGOInput> {
            { static_cast<std::uint16_t>(VTableIndex::CreateMove), reinterpret_cast<void*>(&create_move_hook) }
        },

        HookInterface<sdk::Source2Client> {
            { static_cast<std::uint16_t>(VTableIndex::FrameStageNotify), reinterpret_cast<void*>(&frame_stage_notify_hook) }
        }
    };

    VMTHook& get_hook_for_interface_impl(const fnv1a::hash_t hash) {
        return vmt_hook_map[hash];
    }

    bool setup() {
        for (const auto& [hash, hooks] : hook_interface_map) {
            auto& vmt_hook = get_hook_for_interface_impl(hash);

            void* interface_ptr = interfaces::get_interface_impl(hash);

            if (interface_ptr == nullptr) {
#ifdef _DEBUG
                std::cerr << "Failed to get interface. Hash: " << std::hex << hash << std::endl;
#endif

                return false;
            }

            if (!vmt_hook.initialize(interface_ptr)) {
#ifdef _DEBUG
                std::cerr << "Failed to initialize VMT hook. Hash: " << std::hex << hash << std::endl;
#endif

                return false;
            }

            for (const auto& [index, new_function] : hooks) {
                if (!vmt_hook.hook_function(index, new_function)) {
#ifdef _DEBUG
                    std::cerr << "Failed to hook function. Index: " << std::dec << index << ", Hash: " << std::hex << hash << std::endl;
#endif

                    return false;
                }
            }
        }

        return true;
    }

    void restore() {
        for (auto& vmt_hook : vmt_hook_map | std::views::values)
            vmt_hook.restore();
    }
}