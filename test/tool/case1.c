/*
 * @Author: Xing Wang (wangx_ddpt@yusur.tech)
 * @date: 2023-06-12 11:39:20
 * @last_author: Xing Wang (wangx_ddpt@yusur.tech)
 * @last_edit_time: 2023-07-07 10:10:07
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "tool.h"
#include "../../lib/include/hados_doe.h"

int array_table_store(int fd, uint8_t tbl_id, uint8_t data_len, int offset)
{
	uint8_t *array_item, *array_data;
	int array_item_len, array_index, i, j, ret = 0;
	
	array_item_len = data_len + array_index_size;
	array_data = (uint8_t *)calloc(batch_num, array_item_len);

	for (i = 0; i < tbl_depth / batch_num; ++i) {
		array_index = i * batch_num;
		array_item = array_data;

		for (j = 0; j < batch_num; ++j) {
			*(int32_t *)array_item = array_index;
			*(array_item + array_index_size) = (array_index + offset) & 0xFF;

			array_index++;
			array_item += array_item_len;
		}

		if (hados_doe_array_store_batch(fd, tbl_id, batch_num, array_data) != batch_num) {
			fprintf(stderr, "store data to array table %d failed. index from %d to %d.\n",
				tbl_id, i * batch_num, (i + 1) * batch_num - 1);
			ret = 1;
			goto error;
		}
	}

error:
	free(array_data);

	return ret;
}

int array_table_check(int fd, uint8_t tbl_id, uint8_t data_len, int offset)
{
	uint8_t *array_data, *array_indexs, *array_tmp;
	int array_item_len, index, i, j, ret = 0;
	uint8_t value[ARRAY_DATA_LEN_MAX] = {0};

	array_item_len = data_len + array_index_size;

	array_indexs = (uint8_t *)calloc(batch_num, array_index_size);
	array_data = (uint8_t *)calloc(batch_num, array_item_len);

	for(int i = 0; i < tbl_depth / batch_num; ++i) {
		array_tmp = array_indexs;
		index = i * batch_num;

		for (int j = 0; j < batch_num; ++j) {
			*(uint32_t *)array_tmp = index + j;
			array_tmp += array_index_size;
			index++;
		}

		memset(array_data, 0, batch_num * array_item_len);

		int ret = hados_doe_array_load_batch(fd, tbl_id, batch_num, array_indexs, array_data);
		if (ret != batch_num) {
			fprintf(stderr, "load data from array table %d failed. index from %d to %d.\n",
				tbl_id, i * batch_num, (i + 1) * batch_num - 1);
			
			goto error;
		}

		array_tmp = array_data;
		for (int j = 0; j < batch_num; ++j) {
			index = *(uint32_t *)array_tmp;

			value[0] = (index + offset) & 0xff;

			if (memcmp(array_tmp + array_index_size, value, data_len) != 0) {
				fprintf(stderr, "check array table %d item failed. index=%d, value=%d, expect value=%d\n",
					tbl_id, index, array_tmp[0], value[0]);
				ret = 1;
				goto error;
			}

			array_tmp += array_item_len;
		}
	}

error:
	free(array_indexs);
	free(array_data);

	return ret;
}

int array_check(int fd, uint8_t tbl_id, uint8_t data_len, int offset)
{
	int item_len = data_len + 4;
	char *in = calloc(batch_num, sizeof(int));
	char *out = calloc(batch_num, item_len);
	char *value = calloc(1, data_len);
	for(int i = 0; i < tbl_depth/batch_num; ++i) {
		for (int j = 0; j < batch_num; ++j) {
			*(int *)(in + j * sizeof(int)) = i * batch_num + j;
		}

		memset(out, 0, batch_num * item_len);

		int ret = hados_doe_array_load_batch(fd, tbl_id, batch_num, in, out);
		if (ret != batch_num) {
			fprintf(stderr, "load data from array table %d failed. index from %d to %d.\n",
				tbl_id, i * batch_num, (i + 1) * batch_num - 1);
			
			goto error;
		}

		for (int j = 0; j < batch_num; ++j) {
			char *index = out + j * item_len;
			char *data = index + 4;
			value[0] = (*(int *)index + offset) % 256;
			if (memcmp(data, value, data_len) != 0) {
				fprintf(stderr, "check array table %d item failed. index=%d, value=%d, expect value=%d\n",
					tbl_id, *(int *)index, data[0], value[0]);
				
				goto error;
			}
		}
	}

	free(in);
	free(out);
	free(value);

	return 0;

error:
	free(in);
	free(out);
	free(value);

	return 1;
}

int array_readclear_check(int fd, uint8_t tbl_id, uint8_t data_len, int offset)
{
	int item_len = data_len + 4;
	char *in = calloc(batch_num, sizeof(int));
	char *out = calloc(batch_num, item_len);
	char *value = calloc(1, data_len);
	for(int i = 0; i < tbl_depth/batch_num; ++i) {
		for (int j = 0; j < batch_num; ++j) {
			*(int *)(in + j * 4) = i * batch_num + j;
		}

		memset(out, 0, batch_num * item_len);
		int ret = hados_doe_array_readclear_batch(fd, tbl_id, batch_num, in, out);
		if (ret != batch_num) {
			fprintf(stderr, "readclear data to array table %d failed. index from %d to %d.\n",
				tbl_id, i * batch_num, (i + 1) * batch_num - 1);
			
			goto error;
		}

		for (int j = 0; j < batch_num; ++j) {
			char *index = out + j * item_len;
			char *data = index + 4;
			value[0] = (*(int *)index + offset) % 256;
			if (memcmp(data, value, data_len) != 0) {
				fprintf(stderr, "check array table %d readclear item failed. index=%d, value=%d, expect value=%d\n",
					tbl_id, i * batch_num + j, data[0], value[0]);
					
				for (int k = 0; k < data_len; ++k) {
					fprintf(stderr, "%02x ", (uint8_t)data[k]);
					if (k % 16 == 15)
						fprintf(stderr, "\n");
				}
				fprintf(stderr, "\n");
				
				goto error;
			}
		}

		memset(out, 0, batch_num * item_len);
		ret = hados_doe_array_load_batch(fd, tbl_id, batch_num, in, out);
		if (ret != batch_num) {
			fprintf(stderr, "load data from array table %d failed. index from %d to %d.\n",
				tbl_id, i * batch_num, (i + 1) * batch_num - 1);
			
			goto error;
		}

		memset(value, 0, data_len);
		for (int j = 0; j < batch_num; ++j) {
			char *index = out + j * item_len;
			char *data = index + 4;
			if (memcmp(data, value, data_len) != 0) {
				fprintf(stderr, "check array table %d item failed. it's not all zero. index=%d\n",
					tbl_id, *(int *)index);
				
				goto error;
			}
		}
	}

	free(in);
	free(out);
	free(value);

	return 0;

error:
	free(in);
	free(out);
	free(value);

	return 1;
}

int hash_insert(int fd, uint8_t tbl_id, int depth, uint8_t key_len, uint8_t value_len, int offset)
{
	char *tmp;
	int item_len = key_len + value_len;
	char *in = calloc(batch_num, item_len);
	for (int i = 0; i < tbl_depth / batch_num; ++i) {
		for (int j = 0; j < batch_num; ++j) {
			tmp = in + j * item_len;
			generate_hash_key(i * batch_num + j, tmp);
			*(tmp + key_len) = (tmp[0] + offset) % 256;
		}

		if (hados_doe_hash_insert_batch(fd, tbl_id, batch_num, in) != batch_num) {
			fprintf(stderr, "insert data to hash table %d failed. index from %d to %d.\n",
				tbl_id, i * batch_num, (i + 1) * batch_num - 1);
			goto err;
		}
	}

	free(in);

	return 0;

err:
	free(in);
	return 1;
}

int hash_query(int fd, uint8_t tbl_id, int depth, uint8_t key_len, uint8_t value_len, int offset)
{
	char *tmp;
	int item_len = key_len + value_len;
	char *in = calloc(batch_num, key_len);
	char *out = calloc(batch_num, item_len);
	char *value = calloc(value_len, 1);
	for(int i = 0; i < tbl_depth/batch_num; ++i) {
		memset(in, 0, batch_num * key_len);
		for (int j = 0; j < batch_num; ++j) {
			tmp = in + j * key_len;
			generate_hash_key(i * batch_num + j, tmp);
		}

		memset(out, 0, batch_num * item_len);
		int ret = hados_doe_hash_query_batch(fd, tbl_id, batch_num, in, out);
		if (ret != batch_num) {
			fprintf(stderr, "query data from table %d failed. index from %d to %d. success: %d\n",
				tbl_id, i * batch_num, (i + 1) * batch_num - 1, ret);
			
			goto error;
		}


		for (int j = 0; j < batch_num; ++j) {
			tmp = out + j * item_len;
			value[0] = (tmp[0] + offset) % 256;
			char *data = out + j * item_len + key_len;
			if (memcmp(data, value, value_len) != 0) {
				fprintf(stderr, "check array table %d item failed. index=%d, value=%d, expect value=%d\n",
					tbl_id, i * batch_num + j, data[0], value[0]);

				for (int k = 0; k < value_len; ++k) {
					fprintf(stderr, "%02x ", (uint8_t)value[k]);
					if (k %16 == 15)
						fprintf(stderr, "\n");
				}
				fprintf(stderr, "\n");
					
				goto error;
			}
		}
	}

	free(in);
	free(out);
	free(value);
	return 0;

error:
	free(in);
	free(out);
	free(value);

	return 1;
}

int hash_del_and_check(int fd, uint8_t tbl_id, int depth, uint8_t key_len, uint8_t value_len, int offset)
{
	char *tmp;
	int item_len = key_len + value_len;
	char *in = calloc(batch_num, key_len);
	char *out = calloc(batch_num, item_len);
	for(int i = 0; i < tbl_depth/batch_num; ++i) {
		for (int j = 0; j < batch_num; ++j) {
			tmp = in + j * key_len;
			generate_hash_key(i * batch_num + j, tmp);
		}

		int ret = hados_doe_hash_delete_batch(fd, tbl_id, batch_num, in);
		if (ret != batch_num) {
			fprintf(stderr, "delete data to hash table %d failed. index from %d to %d. success:%d\n",
				tbl_id, i * batch_num, (i + 1) * batch_num - 1, ret);
			goto error;
		}

		ret = hados_doe_hash_query_batch(fd, tbl_id, batch_num, in, out);
		if (ret != 0) {
			fprintf(stderr, "query success while delete to hash table %d. index=%d\n",
					tbl_id, i * batch_num);
			goto error;
		}
	}

	free(in);
	free(out);

	return 0;

error:
	free(in);
	free(out);

	return 1;
}



int run_case_once(int index)
{
	fprintf(stderr, "into run_case_once\n");
	int ret;
	int fd;
	struct hw_advance_cfg hw_cfg;
	int offset = 0;

	fd = hados_doe_request_fd();
	CHECK(fd == -1, "open doe handle failed.\n");

	// create table
	ret = hados_doe_create_arraytbl(fd, 1, tbl_depth, 1 + index, &tb1_cfg);
	CHECK(ret != DOE_RESULT_SUCCESS, "create table %d failed.\n", 1);

	ret = hados_doe_create_arraytbl(fd, 2, tbl_depth, 1 + index, &tb2_cfg);
	CHECK(ret != DOE_RESULT_SUCCESS, "create table %d failed.\n", 2);

	ret = hados_doe_create_arraytbl(fd, 3, tbl_depth, 16, &tb3_cfg);
	CHECK(ret != DOE_RESULT_SUCCESS, "create table %d failed.\n", 3);

	ret = hados_doe_create_arraytbl(fd, 4, tbl_depth, 16, &tb4_cfg);
	CHECK(ret != DOE_RESULT_SUCCESS, "create table %d failed.\n", 4);

	ret = hados_doe_create_arraytbl(fd, 5, tbl_depth, 16, &tb5_cfg);
	CHECK(ret != DOE_RESULT_SUCCESS, "create table %d failed.\n", 5);

	ret = hados_doe_create_arraytbl(fd, 6, tbl_depth, 16, &tb6_cfg);
	CHECK(ret != DOE_RESULT_SUCCESS, "create table %d failed.\n", 6);

	ret = hados_doe_create_arraytbl(fd, 7, tbl_depth, 64 + index, &tb7_cfg);
	CHECK(ret != DOE_RESULT_SUCCESS, "create table %d failed.\n", 7);

	ret = hados_doe_create_arraytbl(fd, 8, tbl_depth, 64 + index, &tb8_cfg);
	CHECK(ret != DOE_RESULT_SUCCESS, "create table %d failed.\n", 8);

	ret = hados_doe_create_arraytbl(fd, 9, tbl_depth, 128, &tb9_cfg);
	CHECK(ret != DOE_RESULT_SUCCESS, "create table %d failed.\n", 9);

	
	ret = hados_doe_create_hashtbl(fd, 10, tbl_depth, 3 + index, 1 + index, &tb10_cfg);
	CHECK(ret != DOE_RESULT_SUCCESS, "create table %d failed.\n", 10);

	ret = hados_doe_create_hashtbl(fd, 11, tbl_depth, 3 + index, 64 + index, &tb11_cfg);
	CHECK(ret != DOE_RESULT_SUCCESS, "create table %d failed.\n", 11);

	ret = hados_doe_create_hashtbl(fd, 12, tbl_depth, 3 + index, 128, &tb12_cfg);
	CHECK(ret != DOE_RESULT_SUCCESS, "create table %d failed.\n", 12);

	ret = hados_doe_create_hashtbl(fd, 13, tbl_depth, 64 + index, 1 + index, &tb13_cfg);
	CHECK(ret != DOE_RESULT_SUCCESS, "create table %d failed.\n", 13);

	ret = hados_doe_create_hashtbl(fd, 14, tbl_depth, 64 + index, 64 + index, &tb14_cfg);
	CHECK(ret != DOE_RESULT_SUCCESS, "create table %d failed.\n", 14);

	ret = hados_doe_create_hashtbl(fd, 15, tbl_depth, 64 + index, 128, &tb15_cfg);
	CHECK(ret != DOE_RESULT_SUCCESS, "create table %d failed.\n", 15);

	ret = hados_doe_create_hashtbl(fd, 16, tbl_depth, 128, 1 + index, &tb16_cfg);
	CHECK(ret != DOE_RESULT_SUCCESS, "create table %d failed.", 16);

	ret = hados_doe_create_hashtbl(fd, 17, tbl_depth, 128, 64 + index, &tb17_cfg);
	CHECK(ret != DOE_RESULT_SUCCESS, "create table %d failed.\n", 17);

	ret = hados_doe_create_hashtbl(fd, 18, tbl_depth, 128, 128, &tb18_cfg);
	CHECK(ret != DOE_RESULT_SUCCESS, "create table %d failed.\n", 18);

	

	fprintf(stderr, "create table finish.\n");

	// TODO: init table and check data
	for (int i = 0; i < 5; ++i) {
		array_table_store(fd, 1, 1 + index, i);
		array_table_store(fd, 2, 1 + index, i);
		array_table_store(fd, 3, 16, i);
		array_table_store(fd, 4, 16, i);
		array_table_store(fd, 5, 16, i);
		array_table_store(fd, 6, 16, i);
		array_table_store(fd, 7, 64 + index, i);
		array_table_store(fd, 8, 64 + index, i);
		array_table_store(fd, 9, 128, i);

		array_table_check(fd, 1, 1 + index, i);
		array_table_check(fd, 2, 1 + index, i);
		array_table_check(fd, 3, 16, i);
		array_table_check(fd, 4, 16, i);
		array_table_check(fd, 5, 16, i);
		array_table_check(fd, 6, 16, i);
		array_table_check(fd, 7, 64 + index, i);
		array_table_check(fd, 8, 64 + index, i);
		array_table_check(fd, 9, 128, i);
		
		array_readclear_check(fd, 1, 1 + index, i);
		array_readclear_check(fd, 2, 1 + index, i);
		array_readclear_check(fd, 3, 16, i);
		array_readclear_check(fd, 4, 16, i);
		array_readclear_check(fd, 5, 16, i);
		array_readclear_check(fd, 6, 16, i);
		array_readclear_check(fd, 7, 64 + index, i);
		array_readclear_check(fd, 8, 64 + index, i);
		array_readclear_check(fd, 9, 128, i);
		
		hash_insert(fd, 18, tbl_depth, 128, 128, i);
		hash_insert(fd, 10, tbl_depth, 3 + index, 1 + index, i);
		hash_insert(fd, 11, tbl_depth, 3 + index, 64 + index, i);
		hash_insert(fd, 12, tbl_depth, 3 + index, 128, i);
		hash_insert(fd, 13, tbl_depth, 64 + index, 1 + index, i);
		hash_insert(fd, 14, tbl_depth, 64 + index, 64 + index, i);
		hash_insert(fd, 15, tbl_depth, 64 + index, 128, i);
		hash_insert(fd, 16, tbl_depth, 128, 1 + index, i);
		hash_insert(fd, 17, tbl_depth, 128, 64 + index, i);

		hash_query(fd, 10, tbl_depth, 3 + index, 1 + index, i);
		hash_query(fd, 11, tbl_depth, 3 + index, 64 + index, i);
		hash_query(fd, 12, tbl_depth, 3 + index, 128, i);
		hash_query(fd, 13, tbl_depth, 64 + index, 1 + index, i);
		hash_query(fd, 14, tbl_depth, 64 + index, 64 + index, i);
		hash_query(fd, 15, tbl_depth, 64 + index, 128, i);
		hash_query(fd, 16, tbl_depth, 128, 1 + index, i);
		hash_query(fd, 17, tbl_depth, 128, 64 + index, i);
		hash_query(fd, 18, tbl_depth, 128, 128, i);

		hash_del_and_check(fd, 10, tbl_depth, 3 + index, 1 + index, i);
		hash_del_and_check(fd, 11, tbl_depth, 3 + index, 64 + index, i);
		hash_del_and_check(fd, 12, tbl_depth, 3 + index, 128, i);
		hash_del_and_check(fd, 13, tbl_depth, 64 + index, 1 + index, i);
		hash_del_and_check(fd, 14, tbl_depth, 64 + index, 64 + index, i);
		hash_del_and_check(fd, 15, tbl_depth, 64 + index, 128, i);
		hash_del_and_check(fd, 16, tbl_depth, 128, 1 + index, i);
		hash_del_and_check(fd, 17, tbl_depth, 128, 64 + index, i);
		hash_del_and_check(fd, 18, tbl_depth, 128, 128, i);
	}

out:
	hados_doe_delete_arraytbl(fd, 1);
	hados_doe_delete_arraytbl(fd, 2);
	hados_doe_delete_arraytbl(fd, 3);
	hados_doe_delete_arraytbl(fd, 4);
	hados_doe_delete_arraytbl(fd, 5);
	hados_doe_delete_arraytbl(fd, 6);
	hados_doe_delete_arraytbl(fd, 7);
	hados_doe_delete_arraytbl(fd, 8);
	hados_doe_delete_arraytbl(fd, 9);

	hados_doe_delete_hashtbl(fd, 10);
	hados_doe_delete_hashtbl(fd, 11);
	hados_doe_delete_hashtbl(fd, 12);
	hados_doe_delete_hashtbl(fd, 13);
	hados_doe_delete_hashtbl(fd, 14);
	hados_doe_delete_hashtbl(fd, 15);
	hados_doe_delete_hashtbl(fd, 16);
	hados_doe_delete_hashtbl(fd, 17);
	hados_doe_delete_hashtbl(fd, 18);
	
	printf("run case %d times\n", index);

	return 0;
}


int func_case1_main(int argc, char *argv[])
{
	int i;
	// check input param

	// run test case
	for (i = 0; i < 64; ++i) {
		if (run_case_once(i))
			break;
	}

	return 0;
}

