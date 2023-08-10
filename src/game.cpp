#include "game.hpp"
#include "utility/memory.hpp"

namespace game {
    sdk::CSPlayerController* get_entity_index(const std::int32_t index) {
        using Fn = sdk::CSPlayerController* (__fastcall*)(std::int32_t);

        static auto fn = memory::find_pattern(xorstr_(L"client.dll"), xorstr_("E8 ? ? ? ? 4C 3B E0")).jmp().as<Fn>();

        return fn != nullptr ? fn(index) : nullptr;
    }

    sdk::CSPlayerController* get_player_controller(const std::int32_t index) {
        using Fn = sdk::CSPlayerController* (__fastcall*)(std::int32_t);

        static auto fn = memory::find_pattern(xorstr_(L"client.dll"), xorstr_("40 53 48 83 EC ? 48 8B 05 ? ? ? ? 48 85 C0 74 3D")).as<Fn>();

        return fn != nullptr ? fn(index) : nullptr;
    }

    sdk::CSPlayerPawn* get_player_pawn(sdk::CSPlayerController* controller) {
        using Fn = sdk::CSPlayerPawn* (__fastcall*)(sdk::CSPlayerController*);

        static auto fn = memory::find_pattern(xorstr_(L"client.dll"), xorstr_("E8 ? ? ? ? 48 85 C0 74 E2")).jmp().as<Fn>();

        return fn != nullptr ? fn(controller) : nullptr;
    }
}