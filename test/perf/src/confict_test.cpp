/**
 * @file hash_insert_random.c
 * @brief 测试hash表存储的随机性。插入大量数据后，查看主表命中率
 * @copyright Copyright (c) 2020 YUSUR Technology Co., Ltd. All Rights Reserved. Learn more at www.yusur.tech.
 * @author Xing Wang (wangx_ddpt@yusur.tech)
 * @date 2023-01-04 10:08:02
 * @last_author: Xing Wang (wangx_ddpt@yusur.tech)
 * @last_edit_time: 2024-02-28 15:45:25
 */

#include "../../../lib/include/hados_doe.h"
#include "../../../modules/include/uapi_yusdoe.h"
#include <stdio.h>
#include <cstring>
#include <string>
#include <sys/ioctl.h>

struct item_t {
	char key[32];
	char val[32];
} __attribute__((packed));

struct array_load_t {
	char			op;
	char			tbl_id;
	uint32_t		index;
} __attribute__((packed));


static uint32_t find_next_power_of_2(uint32_t depth)
{
	uint32_t target = 2;

	while (target < depth)
		target <<= 1;

	return target;
}

static uint8_t tbl_id = 88;

int doe_fd = -1;

uint32_t hash_conflict_list_len(int doe_fd, uint32_t index)
{
	int ret;

	array_load_t raw_data = {
		(char)0x28,
		(char)0xee,
		index,
	};

	struct yusdoe_sw_cmd cmd = {
		YUSDOE_SW_RAW_CMD,
		tbl_id,
		1,
	};

	cmd.cmd = &raw_data;
	cmd.cmd_size = 6;

	ret = ioctl(doe_fd, YUSDOE_SEND_CMD, &cmd);
	if (ret != 0)
		return 0;
	else if (cmd.data[12] == 0)
		return 0;
	else if (cmd.data[11] == 1)
		return hash_conflict_list_len(doe_fd, *(uint32_t *)(cmd.data + 6)) + 1;
	else
		return 1;
}

void hash_conflict(uint32_t depth, uint32_t count, uint8_t key_len, uint8_t val_len)
{
	item_t* pair_list;
	uint32_t succ_cnt, i;
	uint32_t max_confict_len = 0;
	uint32_t total_confict_len = 0;
	uint32_t confict_num = 0;
	uint32_t confict_len_num[16] = {0};

	fprintf(stderr, "********************************\n");
	fprintf(stderr, "Table Depth   : %d\n", depth);
	fprintf(stderr, "Insert Count  : %d\n", count);

	/* create hash table */
	doe_result_t ret = hados_doe_create_hashtbl(doe_fd, tbl_id, depth, val_len, val_len, NULL);
	if (ret != DOE_RESULT_SUCCESS) {
		fprintf(stderr, "create table failed. exit.\n");
		return;
	}

	/* generate data for batch insert */
	pair_list = new item_t[count];
	std::memset(pair_list, 0, sizeof(item_t) * count);

	for (i = 0; i < count; ++i) {
		std::string tmp("hello");
		tmp.append(std::to_string(i));

		memcpy(pair_list[i].key, tmp.c_str(), tmp.length());
		memcpy(pair_list[i].val, "world", sizeof("world"));
	}

	/* batch insert data to hash table */
	succ_cnt = hados_doe_hash_insert_batch(doe_fd, tbl_id, count, pair_list);
	if (succ_cnt != count) {
		fprintf(stderr, "batch insert falied. success count = %d\n", succ_cnt);
		goto del_table;
	}

	/* query main table for calculated */
	for (i = 0; i < depth; ++i) {
		uint32_t len = hash_conflict_list_len(doe_fd, i);
		if (len > max_confict_len)
			max_confict_len = len;

		if (len > 0) {
			confict_num++;
			total_confict_len += len;
			confict_len_num[len]++;
		}
	}

	/* printf result */
	fprintf(stderr, "Result        : %d, %.2f%%, %.2f\n",
		max_confict_len, (float)confict_num*100/depth, (float)total_confict_len/confict_num);
	fprintf(stderr, "              : ");
	for (uint32_t i = 0; i <= max_confict_len; ++i)
		fprintf(stderr, "%d-", confict_len_num[i]);
	fprintf(stderr, "\n\n");

del_table:
	hados_doe_delete_hashtbl(doe_fd, tbl_id);
}

int main(int argc, char* argv[])
{
	// create hash table with depth = 10M
	doe_fd = hados_doe_request_fd();
	if (doe_fd == -1) {
		fprintf(stderr, "call yusdoe_request_fd failed.\n");
		return -1;
	}

	hash_conflict(find_next_power_of_2(1024), 128, 32, 32);
	hash_conflict(find_next_power_of_2(16384), 1024, 32, 32);
	hash_conflict(find_next_power_of_2(131072), 10000, 32, 32);
	hash_conflict(find_next_power_of_2(524288), 100000, 32, 32);
	hash_conflict(find_next_power_of_2(4194304), 1000000, 32, 32);
	hash_conflict(find_next_power_of_2(8388608), 5000000, 32, 32);

	hash_conflict(find_next_power_of_2(128), 128, 32, 32);
	hash_conflict(find_next_power_of_2(1024), 1024, 32, 32);
	hash_conflict(find_next_power_of_2(16384), 16384, 32, 32);
	hash_conflict(find_next_power_of_2(131072), 131072, 32, 32);
	hash_conflict(find_next_power_of_2(1048576), 1048576, 32, 32);
	hash_conflict(find_next_power_of_2(8388608), 8388608, 32, 32);

	hados_doe_release_fd(doe_fd);
}

