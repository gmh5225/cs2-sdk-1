#pragma once

namespace sdk {
    class EngineClient {
    public:
        std::int32_t get_max_clients() {
            return call_virtual<29, std::int32_t>(this);
        }

        bool is_in_game() {
            return call_virtual<30, bool>(this);
        }

        bool is_connected() {
            return call_virtual<31, bool>(this);
        }
    };
}