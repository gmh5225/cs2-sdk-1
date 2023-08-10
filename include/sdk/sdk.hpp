#pragma once

#include <xorstr.hpp>

#include <string_view>

#include "schemas.hpp"

#include "utility/fnv1a.hpp"
#include "utility/call_virtual.hpp"

#include "misc/cs_player_controller.hpp"
#include "misc/cs_player_pawn.hpp"
#include "misc/enums.hpp"
#include "misc/user_cmd.hpp"
#include "misc/utl_string.hpp"
#include "misc/utl_ts_hash.hpp"
#include "misc/utl_vector.hpp"

#include "interfaces/client_mode_shared.hpp"
#include "interfaces/csgo_input.hpp"
#include "interfaces/cvar.hpp"
#include "interfaces/engine_client.hpp"
#include "interfaces/game_resource_service.hpp"
#include "interfaces/localize.hpp"
#include "interfaces/resource_system.hpp"
#include "interfaces/schema_system.hpp"
#include "interfaces/source_2_client.hpp"