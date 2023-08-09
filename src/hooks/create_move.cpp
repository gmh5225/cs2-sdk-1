#include "hooks/hooks.hpp"

namespace hooks {
    void __fastcall create_move_hook(sdk::CSGOInput* input, const std::int32_t slot, const bool active) {
        static auto fn = get_hook_for_interface<sdk::CSGOInput>().get_original_function<decltype(&create_move_hook)>(
            static_cast<std::uint16_t>(VTableIndex::CreateMove)
        );

        fn(input, slot, active);

        sdk::UserCmd* cmd = sdk::CSGOInput::get_user_cmd(input, slot);

        if (cmd == nullptr)
            return;

        sdk::CSPlayerController* local_controller = game::get_entity_index(-1);

        if (local_controller == nullptr)
            return;

        sdk::CSPlayerPawn* local_pawn = game::get_player_pawn(local_controller);

        if (local_pawn == nullptr)
            return;

        if (cmd->buttons & sdk::InJump && !(local_pawn->get_flags() & sdk::OnGround))
            cmd->buttons &= ~sdk::InJump;
    }
}