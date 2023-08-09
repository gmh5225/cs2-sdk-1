#pragma once

#include "interfaces.hpp"

namespace game {
    sdk::CSPlayerController* get_entity_index(std::int32_t index);

    sdk::CSPlayerController* get_player_controller(std::int32_t index);

    sdk::CSPlayerPawn* get_player_pawn(sdk::CSPlayerController* controller);
}