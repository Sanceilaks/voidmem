#pragma once

#include "memory_signature.hpp"
#include <array>
#include <memory>
#include <optional>
#include <string_view>
#include <system_error>
#include <span>

#if defined (INCLUDE_MEMORY_SIGNATURE)
#include <memory_signature.hpp>
#endif

namespace memory {
    template<std::size_t size, class CharType = char>
    struct FixedString {
        constexpr FixedString(const CharType (&array)[size]) {
            std::ranges::copy(array, str.begin());
        }

        constexpr operator auto() const noexcept {
            return std::string_view(str.data(), size - 1);
        }

        std::array<CharType, size> str;
    };

    constexpr auto pointer_size = sizeof(void*);
    constexpr auto is_x64 = sizeof(void*) == 8;

    struct Addr {
        uintptr_t address;
        
        constexpr Addr operator+(ptrdiff_t offset) const noexcept {
            return { address + offset };
        }

        inline constexpr auto add(ptrdiff_t offset) {
            return Addr{address + offset};
        }

        inline auto to_abs(ptrdiff_t offset, ptrdiff_t size) {
            return Addr{ address + size + static_cast<ptrdiff_t>(*reinterpret_cast<int*>(address + offset)) };
        }
        
        template<class PointerType = void*> requires std::is_pointer_v<PointerType>
        inline constexpr auto to_ptr() {
            return reinterpret_cast<PointerType>(address);
        }
    };

    struct ScanData {
        Addr begin;
        size_t size;
    };

    template<class MemoryScannerType> concept MemoryScannerT = requires(MemoryScannerType scanner) {
        { scanner.scan<FixedString{""}>(ScanData{}) } -> std::convertible_to<std::optional<Addr>>;
    };

    #if __has_include(<memory_signature.hpp>)
    template<FixedString pattern>
    struct JmMemorySignatureScanner {
        inline constexpr auto scan(ScanData& data) -> std::optional<Addr> {
            auto result = jm::memory_signature(pattern)
                .find(data.begin.to_ptr<uint8_t*>(), data.begin.add(data.size).to_ptr<uint8_t*>());
            if (result == nullptr)
                return std::nullopt;
            return Addr{reinterpret_cast<uintptr_t>(result)};
        }
    };
    #endif
}