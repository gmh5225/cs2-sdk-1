#include "hooks/hooks.hpp"

namespace hooks {
    void __fastcall level_init_hook(sdk::ClientModeShared* client_mode_shared, const char* map_name) {
        static auto fn = get_hook_for_interface<sdk::ClientModeShared>().get_original_function<decltype(&level_init_hook)>(
            static_cast<std::uint16_t>(VTableIndex::LevelInit)
        );

        // TODO: Add code here.

        fn(client_mode_shared, map_name);
    }
}