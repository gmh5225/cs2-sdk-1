#include "hooks/hooks.hpp"

namespace hooks {
    HRESULT __fastcall resize_buffers_hook(IDXGISwapChain* swap_chain, const UINT buffer_count, const UINT width, const UINT height,
                                           const DXGI_FORMAT new_format, const UINT swap_chain_flags) {
        static auto fn = get_hook_for_interface<IDXGISwapChain>().get_original_function<decltype(&resize_buffers_hook)>(
            static_cast<std::uint16_t>(VTableIndex::ResizeBuffers)
        );

        // TODO: Add code here.

        return fn(swap_chain, buffer_count, width, height, new_format, swap_chain_flags);
    }
}