#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include "yib_inc_base.h"
#include "yib_inc_hw.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("yusur");
MODULE_DESCRIPTION("A test module for yib_hw_mpt_entry struct operations.");
MODULE_VERSION("0.1");

// #define YIB_MPT_TEST    YIB_MPT_FIELD_LOC(94, 95)
// #define YIB_MPT_TEST    YIB_MPT_FIELD_LOC(96, 95)
// #define YIB_MPT_TEST    YIB_MPT_FIELD_LOC(128, 96)

static int __init yib_hw_test_init(void) {
    struct yib_hw_mpt_entry entry;
    uint32_t test_val;

    memset(&entry, 0, sizeof(entry));

    #if(0)
    printk(KERN_INFO "Testing YIB_MPT_TEST...\n");
    yib_hwres_write(&entry, YIB_MPT_TEST, 0xFFFFFFFF);
    test_val = yib_hwres_read(&entry, YIB_MPT_TEST);
    printk(KERN_INFO "Written 0xFFFFFFFF, read back 0x%X\n", test_val);
    yib_hwres_clear(&entry, YIB_MPT_TEST);
    test_val = yib_hwres_read(&entry, YIB_MPT_TEST);
    printk(KERN_INFO "After clear, YIB_MPT_TEST: 0x%X\n", test_val);
    #endif

    printk(KERN_INFO "Testing YIB_MPT_TYPE...3bit\n");
    yib_hwres_write(&entry, YIB_MPT_TYPE, 0x07);
    test_val = yib_hwres_read(&entry, YIB_MPT_TYPE);
    printk(KERN_INFO "Written 0x07, read back 0x%X\n", test_val);

    printk(KERN_INFO "Testing YIB_MPT_NUM_TRANS_LAYERS...2bit\n");
    yib_hwres_write(&entry, YIB_MPT_NUM_TRANS_LAYERS, 0x01);
    test_val = yib_hwres_read(&entry, YIB_MPT_NUM_TRANS_LAYERS);
    printk(KERN_INFO "Written 0x01, read back 0x%X\n", test_val);

    printk(KERN_INFO "Testing YIB_MPT_STATE...2bit\n");
    yib_hwres_write(&entry, YIB_MPT_STATE, 0x03);
    test_val = yib_hwres_read(&entry, YIB_MPT_STATE);
    printk(KERN_INFO "Written 0x03, read back 0x%X\n", test_val);

    printk(KERN_INFO "Testing YIB_MPT_PD...24bit\n");
    yib_hwres_write(&entry, YIB_MPT_PD, 0x89ABCD);
    test_val = yib_hwres_read(&entry, YIB_MPT_PD);
    printk(KERN_INFO "Written 0x89ABCD, read back 0x%X\n", test_val);

    printk(KERN_INFO "mrtype_pd = 0x%X\n", entry.mrtype_pd);

    printk(KERN_INFO "Testing YIB_MPT_REUSE_COUNT...8bit\n");
    yib_hwres_write(&entry, YIB_MPT_REUSE_COUNT, 0x78);
    test_val = yib_hwres_read(&entry, YIB_MPT_REUSE_COUNT);
    printk(KERN_INFO "Written 0x78, read back 0x%X\n", test_val);

    printk(KERN_INFO "Testing YIB_MPT_QPN...20bit\n");
    yib_hwres_write(&entry, YIB_MPT_QPN, 0x23456);
    test_val = yib_hwres_read(&entry, YIB_MPT_QPN);
    printk(KERN_INFO "Written 0x23456, read back 0x%X\n", test_val);

    printk(KERN_INFO "reusecount_qpn = 0x%X\n", entry.reusecount_qpn);

    printk(KERN_INFO "Testing YIB_MPT_PA0_LSB...32bit\n");
    yib_hwres_write(&entry, YIB_MPT_PA0_LSB, 0x89ABCDEF);
    test_val = yib_hwres_read(&entry, YIB_MPT_PA0_LSB);
    printk(KERN_INFO "Written 0x89ABCDEF, read back 0x%X\n", test_val);

    printk(KERN_INFO "Testing YIB_MPT_PA0_MSB...32bit\n");
    yib_hwres_write(&entry, YIB_MPT_PA0_MSB, 0x89ABCDEF);
    test_val = yib_hwres_read(&entry, YIB_MPT_PA0_MSB);
    printk(KERN_INFO "Written 0x89ABCDEF, read back 0x%X\n", test_val);

    printk(KERN_INFO "Testing YIB_MPT_PA1_LSB...32bit\n");
    yib_hwres_write(&entry, YIB_MPT_PA1_LSB, 0x89ABCDEF);
    test_val = yib_hwres_read(&entry, YIB_MPT_PA1_LSB);
    printk(KERN_INFO "Written 0x89ABCDEF, read back 0x%X\n", test_val);

    printk(KERN_INFO "Testing YIB_MPT_PA1_MSB...20bit\n");
    yib_hwres_write(&entry, YIB_MPT_PA1_MSB, 0x89ABCDEF);
    test_val = yib_hwres_read(&entry, YIB_MPT_PA1_MSB);
    printk(KERN_INFO "Written 0x89ABCDEF, read back 0x%X\n", test_val);

    printk(KERN_INFO "Testing YIB_MPT_MRW_PBL_PG_SIZE...5bit\n");
    yib_hwres_write(&entry, YIB_MPT_MRW_PBL_PG_SIZE, 0x89ABCDEF);
    test_val = yib_hwres_read(&entry, YIB_MPT_MRW_PBL_PG_SIZE);
    printk(KERN_INFO "Written 0x89ABCDEF, read back 0x%X\n", test_val);

    printk(KERN_INFO "Testing YIB_MPT_MRW_PG_SIZE...5bit\n");
    yib_hwres_write(&entry, YIB_MPT_MRW_PG_SIZE, 0x89ABCDEF);
    test_val = yib_hwres_read(&entry, YIB_MPT_MRW_PG_SIZE);
    printk(KERN_INFO "Written 0x89ABCDEF, read back 0x%X\n", test_val);

    printk(KERN_INFO "Testing YIB_MPT_SIZE_LSB...32bit\n");
    yib_hwres_write(&entry, YIB_MPT_SIZE_LSB, 0x89ABCDEF);
    test_val = yib_hwres_read(&entry, YIB_MPT_SIZE_LSB);
    printk(KERN_INFO "Written 0x89ABCDEF, read back 0x%X\n", test_val);

    printk(KERN_INFO "Testing YIB_MPT_SIZE_MSB...32bit\n");
    yib_hwres_write(&entry, YIB_MPT_SIZE_MSB, 0x89ABCDEF);
    test_val = yib_hwres_read(&entry, YIB_MPT_SIZE_MSB);
    printk(KERN_INFO "Written 0x89ABCDEF, read back 0x%X\n", test_val);

    printk(KERN_INFO "Testing YIB_MPT_KEY_LSB...8bit\n");
    yib_hwres_write(&entry, YIB_MPT_KEY_LSB, 0x89ABCDEF);
    test_val = yib_hwres_read(&entry, YIB_MPT_KEY_LSB);
    printk(KERN_INFO "Written 0x89ABCDEF, read back 0x%X\n", test_val);

    printk(KERN_INFO "Testing YIB_MPT_PARENT_MR_IDX_OR_CHILD_MW_COUNT...24bit\n");
    yib_hwres_write(&entry, YIB_MPT_PARENT_MR_IDX_OR_CHILD_MW_COUNT, 0x89ABCDEF);
    test_val = yib_hwres_read(&entry, YIB_MPT_PARENT_MR_IDX_OR_CHILD_MW_COUNT);
    printk(KERN_INFO "Written 0x89ABCDEF, read back 0x%X\n", test_val);

    printk(KERN_INFO "Testing YIB_MPT_VA_LSB...32bit\n");
    yib_hwres_write(&entry, YIB_MPT_VA_LSB, 0x89ABCDEF);
    test_val = yib_hwres_read(&entry, YIB_MPT_VA_LSB);
    printk(KERN_INFO "Written 0x89ABCDEF, read back 0x%X\n", test_val);

    printk(KERN_INFO "Testing YIB_MPT_VA_MSB...32bit\n");
    yib_hwres_write(&entry, YIB_MPT_VA_MSB, 0x89ABCDEF);
    test_val = yib_hwres_read(&entry, YIB_MPT_VA_MSB);
    printk(KERN_INFO "Written 0x89ABCDEF, read back 0x%X\n", test_val);

    printk(KERN_INFO "Testing YIB_MPT_PBL_BA_LSB...32bit\n");
    yib_hwres_write(&entry, YIB_MPT_PBL_BA_LSB, 0x89ABCDEF);
    test_val = yib_hwres_read(&entry, YIB_MPT_PBL_BA_LSB);
    printk(KERN_INFO "Written 0x89ABCDEF, read back 0x%X\n", test_val);

    printk(KERN_INFO "Testing YIB_MPT_PBL_BA_MSB...20bit\n");
    yib_hwres_write(&entry, YIB_MPT_PBL_BA_MSB, 0x89ABCDEF);
    test_val = yib_hwres_read(&entry, YIB_MPT_PBL_BA_MSB);
    printk(KERN_INFO "Written 0x89ABCDEF, read back 0x%X\n", test_val);

    printk(KERN_INFO "Testing YIB_MPT_ACCESS_RIGHT...5bit\n");
    yib_hwres_write(&entry, YIB_MPT_ACCESS_RIGHT, 0x89ABCDEF);
    test_val = yib_hwres_read(&entry, YIB_MPT_ACCESS_RIGHT);
    printk(KERN_INFO "Written 0x89ABCDEF, read back 0x%X\n", test_val);

    yib_hwres_clear(&entry, YIB_MPT_PBL_BA_MSB);
    test_val = yib_hwres_read(&entry, YIB_MPT_PBL_BA_MSB);
    printk(KERN_INFO "After clear, YIB_MPT_PBL_BA_MSB: 0x%X\n", test_val);

    yib_hwres_clear(&entry, YIB_MPT_ACCESS_RIGHT);
    test_val = yib_hwres_read(&entry, YIB_MPT_ACCESS_RIGHT);
    printk(KERN_INFO "After clear, YIB_MPT_ACCESS_RIGHT: 0x%X\n", test_val);

    return 0;
}

static void __exit yib_hw_test_exit(void) {
    printk(KERN_INFO KERN_INFO "YIB HW test module unloaded.\n");
}

module_init(yib_hw_test_init);
module_exit(yib_hw_test_exit);
