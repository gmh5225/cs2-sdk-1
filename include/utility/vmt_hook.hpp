#pragma once

#include "virtual_raii_protect.hpp"

#include <memory>

class VMTHook {
public:
    VMTHook() = default;

    bool initialize(void* base_class) {
        if (base_class == nullptr)
            return false;

        base_class_ = reinterpret_cast<std::uintptr_t**>(base_class);

        VirtualRAIIProtect protector(base_class_, sizeof(std::uintptr_t), PAGE_READWRITE);

        original_vmt_ = *base_class_;

        vmt_size_ = calculate_vmt_size();

        if (vmt_size_ == 0)
            return false;

        new_vmt_ = std::make_unique<std::uintptr_t[]>(vmt_size_ + 1);

        std::copy(&original_vmt_[-1], &original_vmt_[vmt_size_ - 1], new_vmt_.get());

        *base_class_ = &new_vmt_.get()[1];

        return true;
    }

    bool restore() {
        if (original_vmt_ == nullptr)
            return false;

        VirtualRAIIProtect protector(base_class_, sizeof(std::uintptr_t), PAGE_READWRITE);

        *base_class_ = original_vmt_;

        original_vmt_ = nullptr;

        return true;
    }

    template <typename Fn>
    __forceinline Fn get_original_function(const std::uint16_t function_index) const {
        if (function_index > vmt_size_)
            return nullptr;

        return reinterpret_cast<Fn>(original_vmt_[function_index]);
    }

    __forceinline bool hook_function(const std::uint16_t function_index, void* new_function) {
        if (function_index > vmt_size_)
            return false;

        new_vmt_[function_index + 1] = reinterpret_cast<std::uintptr_t>(new_function);

        return true;
    }

    __forceinline bool unhook_function(const std::uint16_t function_index) {
        if (function_index > vmt_size_)
            return false;

        new_vmt_[function_index + 1] = original_vmt_[function_index];

        return true;
    }

private:
    [[nodiscard]] __forceinline std::size_t calculate_vmt_size() const {
        std::size_t size = 0;

        while (original_vmt_[size] != 0 && size <= 500)
            ++size;

        return size;
    }

private:
    std::uintptr_t** base_class_{ nullptr };

    std::uintptr_t* original_vmt_{ nullptr };

    std::uint16_t vmt_size_{ 0 };

    std::unique_ptr<std::uintptr_t[]> new_vmt_;
};