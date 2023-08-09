#pragma once

#include <cstddef>
#include <cstdint>

#include <Windows.h>

class VirtualRAIIProtect {
public:
    VirtualRAIIProtect(void* address, const std::size_t size, const DWORD new_protection) : address_(address), size_(size) {
        VirtualProtect(address_, size_, new_protection, &old_protection_);
    }

    VirtualRAIIProtect(const std::uintptr_t address, const std::size_t size, const DWORD new_protection) :
        VirtualRAIIProtect(reinterpret_cast<void*>(address), size, new_protection)
    {}

    ~VirtualRAIIProtect() {
        VirtualProtect(address_, size_, old_protection_, &old_protection_);
    }

private:
    void* address_;

    std::size_t size_;

    DWORD old_protection_{ 0 };
};