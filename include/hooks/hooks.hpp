#pragma once

#include "game.hpp"
#include "utility/vmt_hook.hpp"

#include <unordered_map>

namespace hooks {
    enum class VTableIndex : std::uint16_t {
        CreateMove = 5,
        LevelInit = 23,
        FrameStageNotify = 31
    };

    extern std::unordered_map<fnv1a::hash_t, VMTHook> vmt_hook_map;

    bool setup();

    void restore();

    VMTHook& get_hook_for_interface_impl(fnv1a::hash_t hash);

    void __fastcall create_move_hook(sdk::CSGOInput* input, std::int32_t slot, bool active);
    void __fastcall frame_stage_notify_hook(sdk::Source2Client* source2_client, std::int32_t stage);
    void __fastcall level_init_hook(sdk::ClientModeShared* client_mode_shared, const char* map_name);

    template <class T>
    constexpr static VMTHook& get_hook_for_interface() {
        return get_hook_for_interface_impl(fnv1a::fnv_hash_type<T>());
    }
}