#pragma once
#ifndef MNEMOSYNE_MEM_PROCESS_HPP
#define MNEMOSYNE_MEM_PROCESS_HPP

#include <string_view>
#include <optional>

#include "../core/memory_range.hpp"

namespace mnem {
    // TODO: We need result type instead of std::optional NOW

    struct proc_module {
        void* ptr;

        [[nodiscard]] memory_span get_memory_range() const;
        [[nodiscard]] std::optional<memory_span> get_section_range(std::string_view name) const;
    };

    [[nodiscard]] proc_module get_main_proc_module();
    [[nodiscard]] std::optional<proc_module> get_proc_module(const std::string& name);
    // NOTE: get_proc_module uses std::string because we need a null-terminated string in the win32 implementation.
}

#endif
