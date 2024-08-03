/*
 * @Description: create doe_tool batch store/load need txt.
 * @Company: YUSUR
 * @Author: wangyingfu
 * @LastEditors: wangyingfu
 * @Date: 2022-08-08 13:30:10
 * @LastEditTime: 2024-04-11 17:50:39
 */

#include "stdio.h"
#include "stdint.h"
#include <string.h>

struct np_res_create_param
{
	char name[32];
    uint32_t     depth;
};

enum np_hwres_type
{
	NP_TYPE_MAC_PORT,
	NP_TYPE_QP_REMAP,
	NP_TYPE_QP_STATE,
	NP_TYPE_SQC_CLU4,
	NP_TYPE_SQC_CLU5,
    	NP_TYPE_SQC_CLU6,
    	NP_TYPE_SQC_CLU7,
    	NP_TYPE_SQC_CLU8,
	NP_TYPE_RQC_CLU9,
    	NP_TYPE_RQC_CLU10,
    	NP_TYPE_RQC_CLU11,
    	NP_TYPE_RQC_CLU12,
    	NP_TYPE_RQC_CLU13,
    	NP_TYPE_SQS_CLU4,
	NP_TYPE_SQS_CLU5,
    	NP_TYPE_SQS_CLU6,
   	NP_TYPE_SQS_CLU7,
    	NP_TYPE_SQS_CLU8,
	NP_TYPE_RQS_CLU9,
    	NP_TYPE_RQS_CLU10,
    	NP_TYPE_RQS_CLU11,
    	NP_TYPE_RQS_CLU12,
    	NP_TYPE_RQS_CLU13,
    	NP_TYPE_SQE_CLU4,
	NP_TYPE_SQE_CLU5,
    	NP_TYPE_SQE_CLU6,
    	NP_TYPE_SQE_CLU7,
    	NP_TYPE_SQE_CLU8,
	NP_TYPE_RQE_CLU9,
    	NP_TYPE_RQE_CLU10,
    	NP_TYPE_RQE_CLU11,
    	NP_TYPE_RQE_CLU12,
    	NP_TYPE_RQE_CLU13,
	NP_TYPE_RMSG_CLU9,
    	NP_TYPE_RMSG_CLU10,
    	NP_TYPE_RMSG_CLU11,
    	NP_TYPE_RMSG_CLU12,
    	NP_TYPE_RMSG_CLU13,
    	NP_TYPE_MPT,
    	NP_TYPE_MTT,
    	NP_TYPE_AVT,
	NP_TYPE_CQC,
	NP_TYPE_NQC,
    	NP_TYPE_RES_MAX,
};

struct np_res_create_param np_res_createtbl_info[NP_TYPE_RES_MAX] = {
        [NP_TYPE_MAC_PORT]                              = {
                .name   = "mac_index.txt",
                .depth  = 32,
        },
        [NP_TYPE_QP_REMAP]                              = {
                .name   = "remap_index.txt",
                .depth  = 64,
        },
        [NP_TYPE_QP_STATE]                              = {
                .name   = "state_index.txt",
                .depth  = 64,
        },
        [NP_TYPE_SQC_CLU4]                              = {
                .name   = "sqc4_index.txt",
                .depth  = 8,
        },
        [NP_TYPE_SQC_CLU5]                              = {
                .name   = "sqc5_index.txt",
                .depth  = 8,
        },
        [NP_TYPE_SQC_CLU6]                              = {
                .name   = "sqc6_index.txt",
                .depth  = 8,
        },
        [NP_TYPE_SQC_CLU7]                              = {
                .name   = "sqc7_index.txt",
                .depth  = 8,
        },
        [NP_TYPE_SQC_CLU8]                              = {
                .name   = "sqc8_index.txt",
                .depth  = 8,
        },        
        [NP_TYPE_RQC_CLU9]                              = {
                .name   = "rqc9_index.txt",
                .depth  = 8,
        },        
        [NP_TYPE_RQC_CLU10]                              = {
                .name   = "rqc10_index.txt",
                .depth  = 8,
        },        
        [NP_TYPE_RQC_CLU11]                              = {
                .name   = "rqc11_index.txt",
                .depth  = 8,
        },        
        [NP_TYPE_RQC_CLU12]                              = {
                .name   = "rqc12_index.txt",
                .depth  = 8,
        },        
        [NP_TYPE_RQC_CLU13]                              = {
                .name   = "rqc13_index.txt",
                .depth  = 8,
        },        
        [NP_TYPE_SQS_CLU4]                              = {
                .name   = "sqs4_index.txt",
                .depth  = 8,
        },        
        [NP_TYPE_SQS_CLU5]                              = {
                .name   = "sqs5_index.txt",
                .depth  = 8,
        },
        [NP_TYPE_SQS_CLU6]                              = {
                .name   = "sqs6_index.txt",
                .depth  = 8,
        },
        [NP_TYPE_SQS_CLU7]                              = {
                .name   = "sqs7_index.txt",
                .depth  = 8,
        },
        [NP_TYPE_SQS_CLU8]                              = {
                .name   = "sqs8_index.txt",
                .depth  = 8,
        },        
        [NP_TYPE_RQS_CLU9]                              = {
                .name   = "rqs9_index.txt",
                .depth  = 8,
        },        
        [NP_TYPE_RQS_CLU10]                              = {
                .name   = "rqs10_index.txt",
                .depth  = 8,
        },        
        [NP_TYPE_RQS_CLU11]                              = {
                .name   = "rqs11_index.txt",
                .depth  = 8,
        },        
        [NP_TYPE_RQS_CLU12]                              = {
                .name   = "rqs12_index.txt",
                .depth  = 8,
        },        
        [NP_TYPE_RQS_CLU13]                              = {
                .name   = "rqs13_index.txt",
                .depth  = 8,
        },        
        [NP_TYPE_SQE_CLU4]                              = {
                .name   = "sqe4_index.txt",
                .depth  = 256,
        },        
        [NP_TYPE_SQE_CLU5]                              = {
                .name   = "sqe5_index.txt",
                .depth  = 256,
        },
        [NP_TYPE_SQE_CLU6]                              = {
                .name   = "sqe6_index.txt",
                .depth  = 256,
        },
        [NP_TYPE_SQE_CLU7]                              = {
                .name   = "sqe7_index.txt",
                .depth  = 256,
        },
        [NP_TYPE_SQE_CLU8]                              = {
                .name   = "sqe8_index.txt",
                .depth  = 256,
        },        
        [NP_TYPE_RQE_CLU9]                              = {
                .name   = "rqe9_index.txt",
                .depth  = 256,
        },        
        [NP_TYPE_RQE_CLU10]                              = {
                .name   = "rqe10_index.txt",
                .depth  = 256,
        },        
        [NP_TYPE_RQE_CLU11]                              = {
                .name   = "rqe11_index.txt",
                .depth  = 256,
        },        
        [NP_TYPE_RQE_CLU12]                              = {
                .name   = "rqe12_index.txt",
                .depth  = 256,
        },        
        [NP_TYPE_RQE_CLU13]                              = {
                .name   = "rqe13_index.txt",
                .depth  = 256,
        },        
        [NP_TYPE_RMSG_CLU9]                              = {
                .name   = "rmsg9_index.txt",
                .depth  = 256,
        },        
        [NP_TYPE_RMSG_CLU10]                              = {
                .name   = "rmsg10_index.txt",
                .depth  = 256,
        },        
        [NP_TYPE_RMSG_CLU11]                              = {
                .name   = "rmsg11_index.txt",
                .depth  = 256,
        },        
        [NP_TYPE_RMSG_CLU12]                              = {
                .name   = "rmsg12_index.txt",
                .depth  = 256,
        },        
        [NP_TYPE_RMSG_CLU13]                              = {
                .name   = "rmsg13_index.txt",
                .depth  = 256,
        },        
        [NP_TYPE_MPT]                              = {
                .name   = "mpt_index.txt",
                .depth  = 1024,
        },
        [NP_TYPE_MTT]                              = {
                .name   = "mtt_index.txt",
                .depth  = 256,
        },
        [NP_TYPE_AVT]                              = {
                .name   = "avt_index.txt",
                .depth  = 1024,
        },
        [NP_TYPE_CQC]                              = {
                .name   = "cqc_index.txt",
                .depth  = 128,
        },        
        [NP_TYPE_NQC]                              = {
                .name   = "nqc_index.txt",
                .depth  = 1,
        },        
};

int main(int argc, char *argv[])
{
    FILE * fp1;
	uint32_t value;	
	size_t i, j, ret;
	uint32_t count;

	for (j = 0; j < NP_TYPE_RES_MAX; j++)
	{
		if((fp1 = fopen(np_res_createtbl_info[j].name,"w+")) < 0)
		{
			printf("open the file is error!\n");
			return -1;
		}
		fseek(fp1,0,SEEK_SET);


		for (i = 0; i < np_res_createtbl_info[j].depth; i++)
		{
			value = i;
			ret = fwrite(&value, sizeof(uint32_t), 1, fp1);
			if(ret != 1){
				printf("write the file is failed %ld!\n", i);
				break;
			}
		}

		fclose(fp1);
	}

    return 0;
}
