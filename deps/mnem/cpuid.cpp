#include "cpuid.hpp"
#include <string>
#include <bitset>
#include <vector>
#include <array>
#include <cstring>

#ifdef __GNUC__

#include <cpuid.h>

static inline void cpuid(int cpuInfo[4], int leaf) {
    int eax, ebx, ecx, edx;
    __cpuid(leaf, eax, ebx, ecx, edx);

    cpuInfo[0] = eax;
    cpuInfo[1] = ebx;
    cpuInfo[2] = ecx;
    cpuInfo[3] = edx;
}

#else

#include <intrin.h>

static inline void cpuid(int cpuInfo[4], int leaf) {
    __cpuid(cpuInfo, leaf);
}

#endif

namespace {
    // https://learn.microsoft.com/en-us/cpp/intrinsics/cpuid-cpuidex?view=msvc-170
    class cpuinfo_impl {
    public:
        cpuinfo_impl() {
            int nIds, nExIds;
            std::vector<std::array<int, 4>> data, extdata;

            std::array<int, 4> cpui{};
            cpuid(cpui.data(), 0);
            nIds = cpui[0];

            for (int i = 0; i <= nIds; ++i) {
                __cpuidex(cpui.data(), i, 0);
                data.push_back(cpui);
            }

            char vendor[0x20];
            memset(vendor, 0, sizeof(vendor));
            *reinterpret_cast<int*>(vendor) = data[0][1];
            *reinterpret_cast<int*>(vendor + 4) = data[0][3];
            *reinterpret_cast<int*>(vendor + 8) = data[0][2];
            vendor_ = vendor;

            if (vendor_ == "GenuineIntel") {
                isIntel_ = true;
            } else if (vendor_ == "AuthenticAMD") {
                isAMD_ = true;
            }

            if (nIds >= 1) {
                f_1_ECX_ = data[1][2];
                f_1_EDX_ = data[1][3];
            }

            if (nIds >= 7) {
                f_7_EBX_ = data[7][1];
                f_7_ECX_ = data[7][2];
            }

            cpuid(cpui.data(), 0x80000000);
            nExIds = cpui[0];

            for (int i = 0x80000000; i <= nExIds; ++i) {
                __cpuidex(cpui.data(), i, 0);
                extdata.push_back(cpui);
            }

            if (nExIds >= 0x80000001) {
                f_81_ECX_ = extdata[1][2];
                f_81_EDX_ = extdata[1][3];
            }

            if (nExIds >= 0x80000004) {
                char brand[0x40];
                memset(brand, 0, sizeof(brand));

                memcpy(brand, extdata[2].data(), sizeof(cpui));
                memcpy(brand + 16, extdata[3].data(), sizeof(cpui));
                memcpy(brand + 32, extdata[4].data(), sizeof(cpui));
                brand_ = brand;
            }
        }

        std::string_view vendor() { return this->vendor_; }
        std::string_view brand() { return this->brand_; }
        bool sse4_2() { return this->f_1_ECX_[20]; }
        bool avx2() { return this->f_7_EBX_[5]; }
        bool bmi1() { return this->f_7_EBX_[3]; }

        static cpuinfo_impl& get() {
            static cpuinfo_impl info;
            return info;
        }

    private:
        std::string vendor_, brand_;
        bool isIntel_ = false,
                isAMD_ = false;
        std::bitset<32>
                f_1_ECX_ = 0,
                f_1_EDX_ = 0,
                f_7_EBX_ = 0,
                f_7_ECX_ = 0,
                f_81_ECX_ = 0,
                f_81_EDX_ = 0;
    };
}

namespace mnem::internal {
    std::string_view cpuinfo::vendor() {
        return cpuinfo_impl::get().vendor();
    }

    std::string_view cpuinfo::brand() {
        return cpuinfo_impl::get().brand();
    }

    bool cpuinfo::sse4_2() {
        return cpuinfo_impl::get().sse4_2();
    }

    bool cpuinfo::avx2() {
        return cpuinfo_impl::get().avx2();
    }

    bool cpuinfo::bmi1() {
        return cpuinfo_impl::get().bmi1();
    }
}