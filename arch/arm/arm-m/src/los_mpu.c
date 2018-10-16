
#include <los_mpu.h>

static struct
{
    volatile UINT32 type;
    volatile UINT32 ctrl;
    volatile UINT32 rnr;
    volatile UINT32 rbar;
    volatile UINT32 rasr;
} * const MPU = (void *) 0xe000ed90;

void mpu_init (void)
    {
    if (((MPU->type & MPU_TYPE_NR_DREG_MASK) >> MPU_TYPE_NR_DREG_SHIFT) != 8)
        {
        return;
        }

    /*
     * the background map region attributes for privileged only:
     *
     * 0x00000000 ~ 0x1fffffff XRW WT       ROM/FLASH
     * 0x20000000 ~ 0x3fffffff XRW WB       SRAM
     * 0x40000000 ~ 0x5fffffff -RW device   on-chip peripheral
     * 0x60000000 ~ 0x7fffffff XRW WB       RAM
     * 0x80000000 ~ 0x9fffffff -RW WT       RAM
     * 0xa0000000 ~ 0xbfffffff -RW device   shared device
     * 0xc0000000 ~ 0xdfffffff -RW device   non-shared device
     * 0xe0000000 ~ 0xe00fffff -RW strongly PPB
     * 0xe0100000 ~ 0xffffffff -RW device   vendor system region
     *
     * refer to ValidateAddress, DefaultMemoryAttributes and DefaultPermissions
     * in DDI0419C and DDI0403E
     *
     * the default background map is just fine for privileged regions so only
     * use MPU entries for unprivileged tasks
     */

    MPU->ctrl = MPU_CTRL_PRIVDEFENA | MPU_CTRL_ENABLE;

    MPU->rbar = 0x00000000 | MPU_RBAR_VALID | MPU_RBAR_REGION (0);
    MPU->rasr = MPU_ATTR_RW_R_ | MPU_ATTR_WT |
                MPU_ATTR_SZ (0x20000000) | MPU_ATTR_EN;

    MPU->rbar = 0x40000000 | MPU_RBAR_VALID | MPU_RBAR_REGION (1);
    MPU->rasr = MPU_ATTR_RW_RW | MPU_ATTR_DEVICE | MPU_ATTR_XN |
                MPU_ATTR_SZ (0x20000000) | MPU_ATTR_EN;
    }

