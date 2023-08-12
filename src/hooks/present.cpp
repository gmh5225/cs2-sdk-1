#include "hooks/hooks.hpp"

namespace hooks {
    HRESULT __fastcall present_hook(IDXGISwapChain* swap_chain, const UINT sync_interval, const UINT flags) {
        static auto fn = get_hook_for_interface<IDXGISwapChain>().get_original_function<decltype(&present_hook)>(
            static_cast<std::uint16_t>(VTableIndex::Present)
        );

        // TODO: Add code here.

        return fn(swap_chain, sync_interval, flags);
    }
}