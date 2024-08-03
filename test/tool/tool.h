/**
 * @Author: Xing Wang (wangx_ddpt@yusur.tech)
 * @date: 2023-06-12 11:48:03
 * @last_author: Xing Wang (wangx_ddpt@yusur.tech)
 * @last_edit_time: 2023-07-06 17:20:35
 */

#include "../../lib/include/hados_doe.h"

#include <stdio.h>

static int tbl_depth = 100000;
static int batch_num = 10000;
static int array_index_size = 4;

#define ARRAY_DATA_LEN_MAX 128

static struct hw_advance_cfg tb1_cfg  = { false, {2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 0 };
static struct hw_advance_cfg tb2_cfg  = { true,  {2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 4 };
static struct hw_advance_cfg tb3_cfg  = { false, {2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 0 };
static struct hw_advance_cfg tb4_cfg  = { false, {2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 0 };
static struct hw_advance_cfg tb5_cfg  = { false, {2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 0 };
static struct hw_advance_cfg tb6_cfg  = { false, {2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 0 };
static struct hw_advance_cfg tb7_cfg  = { false, {1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 0 };
static struct hw_advance_cfg tb8_cfg  = { true,  {1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 4 };
static struct hw_advance_cfg tb9_cfg  = { true,  {1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 8 };
static struct hw_advance_cfg tb10_cfg = { true,  {2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 4 };
static struct hw_advance_cfg tb11_cfg = { true,  {2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 6 };
static struct hw_advance_cfg tb12_cfg = { true,  {2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 8 };
static struct hw_advance_cfg tb13_cfg = { true,  {1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 8 };
static struct hw_advance_cfg tb14_cfg = { true,  {1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 8 };
static struct hw_advance_cfg tb15_cfg = { true,  {1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 8 };
static struct hw_advance_cfg tb16_cfg = { true,  {1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 8 };
static struct hw_advance_cfg tb17_cfg = { true,  {1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 8 };
static struct hw_advance_cfg tb18_cfg = { true,  {1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 128 };

#define CHECK(condition, message...)				\
    do {											\
        if (condition) {							\
            fprintf(stderr, ##message);	\
			return 1;								\
        }											\
    } while (0)

int func_case1_main(int argc, char *argv[]);
int func_case2_main(int argc, char *argv[]);

static void generate_hash_key(int ikey, char *key)
{
	key[0] = ikey & 0xff;
	key[1] = (ikey >> 8) & 0xff;
	key[2] = (ikey >> 16) & 0xff;
}