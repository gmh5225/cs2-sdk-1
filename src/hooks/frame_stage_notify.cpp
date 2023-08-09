#include "hooks/hooks.hpp"

namespace hooks {
    void __fastcall frame_stage_notify_hook(sdk::Source2Client* source_2_client, const std::int32_t stage) {
        static auto fn = get_hook_for_interface<sdk::Source2Client>().get_original_function<decltype(&frame_stage_notify_hook)>(
            static_cast<std::uint16_t>(VTableIndex::FrameStageNotify)
        );

        // TODO: Add code here.

        fn(source_2_client, stage);
    }
}