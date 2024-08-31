
#ifndef _YIB_INC_HW_H
#define _YIB_INC_HW_H



struct yib_hw_mpt_entry {
    __le32 mrtype_pd;   //[2:0]type; [4:3]num_trans_layers; [5:5]zero_based; [7:6]state; [31:8]pd
    __le32 reusecount_qpn;  //[39:32]reuse_count; [59:40]qpn
    __le32 pa0lsb;  //[95:64]pa0_lsb
    __le32 pa0msb;  //[127:96]pa0_msb
    __le32 pa1lsb;  //[159:128]pa1_lsb
    __le32 pa1msb_pgsize;   //[179:160]pa1_msb; [186:182]mrw_pbl_pg_size; [191:187]mrw_pg_size
    __le32 sizelsb;     //[223:192]size_lsb; 
    __le32 sizemsb;     //[255:224]size_msb
    __le32 keylsb_mrinfo;   //[263:256]key_lsb; [287:264]parent_mr_idx_or_child_mw_count
    __le32 valsb;   //[319:288]va_lsb
    __le32 vamsb;   //[351:320]va_msb
    __le32 pblbalsb;    //[383:352]pbl_ba_lsb
    __le32 pblbamsb_access;     //[403:384]pbl_ba_msb; [415:411]access_right
    __le32 byte52_59;
    __le32 byte56_59;
    __le32 byte60_63;
};

#define YIB_FIELD_LOC(field_type, field_h, field_l) field_type, field_h, field_l
#define YIB_MPT_FIELD_LOC(h, l) YIB_FIELD_LOC(struct yib_hw_mpt_entry, h, l)

#define YIB_MPT_TYPE    YIB_MPT_FIELD_LOC(2, 0)     //0:MR  1:PMR  2:MW Type 1  3:MW Type 2A
#define YIB_MPT_NUM_TRANS_LAYERS    YIB_MPT_FIELD_LOC(4, 3)     //0:0级寻址  1:1级寻址  2:2级寻址
//#define YIB_MPT_ZERO_BASED    YIB_MPT_FIELD_LOC(5, 5)
#define YIB_MPT_STATE    YIB_MPT_FIELD_LOC(7, 6)    //0:invalid  1:free  3:valid
#define YIB_MPT_PD    YIB_MPT_FIELD_LOC(31, 8)
#define YIB_MPT_REUSE_COUNT    YIB_MPT_FIELD_LOC(39, 32)    //每次注销或失效时，值都会增加。它被用作PTU中全局缓存的搜索键的一部分。
#define YIB_MPT_QPN    YIB_MPT_FIELD_LOC(59, 40)
#define YIB_MPT_PA0_LSB    YIB_MPT_FIELD_LOC(95, 64)    //物理地址[31:0]
#define YIB_MPT_PA0_MSB    YIB_MPT_FIELD_LOC(127, 96)   //物理地址[63:32]
#define YIB_MPT_PA1_LSB    YIB_MPT_FIELD_LOC(159, 128)  //4K对齐，物理地址[43:12]
#define YIB_MPT_PA1_MSB    YIB_MPT_FIELD_LOC(179, 160)  //4K对齐，物理地址[63:44]
#define YIB_MPT_MRW_PBL_PG_SIZE    YIB_MPT_FIELD_LOC(186, 182)
#define YIB_MPT_MRW_PG_SIZE    YIB_MPT_FIELD_LOC(191, 187)
#define YIB_MPT_SIZE_LSB    YIB_MPT_FIELD_LOC(223, 192)
#define YIB_MPT_SIZE_MSB    YIB_MPT_FIELD_LOC(255, 224)
#define YIB_MPT_KEY_LSB    YIB_MPT_FIELD_LOC(263, 256)
#define YIB_MPT_PARENT_MR_IDX_OR_CHILD_MW_COUNT    YIB_MPT_FIELD_LOC(287, 264)  //如果是MR，软件赋初值0，硬件更新；如果是MW，软件赋值。
#define YIB_MPT_VA_LSB    YIB_MPT_FIELD_LOC(319, 288)   //虚拟地址[31:0]
#define YIB_MPT_VA_MSB    YIB_MPT_FIELD_LOC(351, 320)   //虚拟地址[63:32]
#define YIB_MPT_PBL_BA_LSB    YIB_MPT_FIELD_LOC(383, 352)   //4K对齐
#define YIB_MPT_PBL_BA_MSB    YIB_MPT_FIELD_LOC(403, 384)
#define YIB_MPT_ACCESS_RIGHT    YIB_MPT_FIELD_LOC(415, 411)
//#define YIB_MPT_LIST_PREV    YIB_MPT_FIELD_LOC(467, 448)
//#define YIB_MPT_LIST_NEXT    YIB_MPT_FIELD_LOC(499, 480)



#endif






