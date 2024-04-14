#include <mnemosyne/mem/process.hpp>
#include <link.h>

auto mnem::proc_module::get_memory_range() const -> memory_span {
    // TODO: this code is EXTREMELY sus bc i stole it from a linux csgo cheat
    auto* info = reinterpret_cast<dl_phdr_info*>(this->ptr);
    return { reinterpret_cast<std::byte*>(info->dlpi_addr + info->dlpi_phdr[0].p_vaddr), info->dlpi_phdr[0].p_memsz };
}

auto mnem::proc_module::get_section_range(std::string_view name) const -> std::optional<memory_span> {
    // TODO: how
}

auto mnem::get_main_proc_module() -> proc_module {
    // TODO: how
}

auto mnem::get_proc_module(const std::string& name) -> std::optional<proc_module> {
    // TODO: lmao i need a lambda wrapper cuz this is STUPID
    struct silly_struct {
        dl_phdr_info* result = nullptr;
        const std::string* name = nullptr;
    } silly { .name = &name };

    dl_iterate_phdr([](dl_phdr_info* info, size_t, void* param) {
        auto& silly = *reinterpret_cast<silly_struct*>(param);

        if (*silly.name == info->dlpi_name) {
            silly.result = info;
            return 1;
        }

        return 0;
    }, &silly);

    return silly.result ? std::make_optional(proc_module{ silly.result }) : std::nullopt;
}
