#ifndef _YIB_INC_BASE_H
#define _YIB_INC_BASE_H

#define BIG_ENDIAN    0
#define LITTLE_ENDIAN 1
#define ENDIANNESS    LITTLE_ENDIAN

#if ENDIANNESS == BIG_ENDIAN
#define yib_cpu_to_le32(x) \
    ((((x) & 0xFF000000) >> 24) | \
     (((x) & 0x00FF0000) >>  8) | \
     (((x) & 0x0000FF00) <<  8) | \
     (((x) & 0x000000FF) << 24))
#define yib_le32_to_cpu(x) yib_cpu_to_le32(x)
#elif ENDIANNESS == LITTLE_ENDIAN
#define yib_cpu_to_le32(x) (x)
#define yib_le32_to_cpu(x) (x)
#endif

#define RUNNING_ON_LINUX    0
#define RUNNING_ON_FW       1
#define RUNNING_ON_QEMU     2
#define RUNNING_ON_ENV      RUNNING_ON_FW

#if RUNNING_ON_ENV == RUNNING_ON_LINUX
#include "os/linux.h"
#elif RUNNING_ON_ENV == RUNNING_ON_FW
#include "os/fw.h"
#elif RUNNING_ON_ENV == RUNNING_ON_QEMU
#include "os/qemu.h"
#endif

#define YIB_FIELD_LOC(field_type, field_h, field_l) field_type, field_h, field_l
#define YIB_GENMASK(high, low) (((~0U) >> (31 - (high) + (low))) << (low))
#define YIB_BUILD_BUG_ON(condition) ((void)sizeof(char[1 - 2*!!(condition)]))

static inline __le32 YIB_FIELD_GET(__le32 mask, __le32 reg) {
    return (reg & mask) >> (__builtin_ctz(mask));
}

static inline __le32 YIB_FIELD_PREP(__le32 mask, __le32 val) {
    return (val << __builtin_ctz(mask)) & mask;
}

#define _yib_hwres_clear(ptr, field_type, field_h, field_l)                                   \
	({                                                                                        \
		const field_type *_ptr = ptr;                                                         \
		YIB_BUILD_BUG_ON(((field_h) / 32) != ((field_l) / 32));                               \
        YIB_BUILD_BUG_ON((field_h) < (field_l));                                              \
        __le32 _val = yib_cpu_to_le32(yib_readl((const volatile void __iomem *)                   \
            ((__le32 *)_ptr + ((field_h) / 32))));                                            \
        _val &= ~yib_cpu_to_le32(YIB_GENMASK((field_h) % 32, (field_l) % 32));                \
        yib_writel(yib_le32_to_cpu(_val), (void __iomem *)((__le32 *)_ptr + ((field_h) / 32)));   \
	})

#define yib_hwres_clear(ptr, field) _yib_hwres_clear(ptr, field)

#define _yib_hwres_write(ptr, field_type, field_h, field_l, val)                                   \
	({                                                                                             \
        const field_type *_ptr = ptr;                                                              \
		YIB_BUILD_BUG_ON(((field_h) / 32) != ((field_l) / 32));                                    \
        YIB_BUILD_BUG_ON((field_h) < (field_l));                                                   \
        __le32 _val = yib_cpu_to_le32(yib_readl((const volatile void __iomem *)                        \
            ((__le32 *)_ptr + ((field_h) / 32))));                                                 \
        _val &= ~yib_cpu_to_le32(YIB_GENMASK((field_h) % 32, (field_l) % 32));                     \
        _val |= yib_cpu_to_le32(YIB_FIELD_PREP(YIB_GENMASK((field_h) % 32, (field_l) % 32), val)); \
        yib_writel(yib_le32_to_cpu(_val), (void __iomem *)((__le32 *)_ptr + ((field_h) / 32)));        \
	})

#define yib_hwres_write(ptr, field, val) _yib_hwres_write(ptr, field, val)

#define _yib_hwres_read(ptr, field_type, field_h, field_l)                      \
	({                                                                          \
		const field_type *_ptr = ptr;                                           \
		YIB_BUILD_BUG_ON(((field_h) / 32) != ((field_l) / 32));                 \
        YIB_BUILD_BUG_ON((field_h) < (field_l));                                \
        __le32 _val = yib_cpu_to_le32(yib_readl((const volatile void __iomem *)     \
            ((__le32 *)_ptr + ((field_h) / 32))));                              \
		YIB_FIELD_GET(YIB_GENMASK((field_h) % 32, (field_l) % 32),              \
              yib_le32_to_cpu(_val)); \
	})

#define yib_hwres_read(ptr, field) _yib_hwres_read(ptr, field)

#endif

