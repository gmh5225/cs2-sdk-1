#include "hooks/hooks.hpp"

#include <spdlog/spdlog.h>

#include <ranges>
#include <unordered_map>

namespace hooks {
    using HookInterfacePair = std::pair<std::uint16_t, void*>;
    using HookInterfaceList = std::vector<HookInterfacePair>;
    using HookInterfaceEntry = std::pair<fnv1a::Hash, HookInterfaceList>;
    using HookInterfaceMap = std::vector<HookInterfaceEntry>;

    template <class T>
    struct HookInterface : HookInterfaceEntry {
        constexpr HookInterface(const std::initializer_list<HookInterfacePair>& hooks)
            : HookInterfaceEntry(fnv1a::fnv_hash_type<T>(), hooks)
        {}
    };

    std::unordered_map<fnv1a::Hash, VMTHook> vmt_hook_map;

    HookInterfaceMap hook_interface_map = {
        HookInterface<sdk::ClientModeShared> {
            { static_cast<std::uint16_t>(VTableIndex::LevelInit), reinterpret_cast<void*>(&level_init_hook) }
        },

        HookInterface<sdk::CSGOInput> {
            { static_cast<std::uint16_t>(VTableIndex::CreateMove), reinterpret_cast<void*>(&create_move_hook) }
        },

        HookInterface<IDXGISwapChain> {
            { static_cast<std::uint16_t>(VTableIndex::Present), reinterpret_cast<void*>(&present_hook) },
            { static_cast<std::uint16_t>(VTableIndex::ResizeBuffers), reinterpret_cast<void*>(&resize_buffers_hook) }
        },

        HookInterface<sdk::Source2Client> {
            { static_cast<std::uint16_t>(VTableIndex::FrameStageNotify), reinterpret_cast<void*>(&frame_stage_notify_hook) }
        }
    };

    VMTHook& get_hook_for_interface_impl(const fnv1a::Hash hash) {
        return vmt_hook_map[hash];
    }

    bool setup() {
        for (const auto& [hash, hooks] : hook_interface_map) {
            auto& vmt_hook = get_hook_for_interface_impl(hash);

            void* interface_ptr = interfaces::get_interface_impl(hash);

            if (interface_ptr == nullptr) {
#ifdef _DEBUG
                spdlog::error("Failed to get interface. Hash: {0:X}", hash);
#endif

                return false;
            }

            if (!vmt_hook.initialize(interface_ptr)) {
#ifdef _DEBUG
                spdlog::error("Failed to initialize VMT hook. Hash: {0:X}", hash);
#endif

                return false;
            }

            for (const auto& [index, new_function] : hooks) {
                if (!vmt_hook.hook_function(index, new_function)) {
#ifdef _DEBUG
                    spdlog::error("Failed to hook function at index {0}. Hash: {1:X}", index, hash);
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