/*
 * @Author: Xing Wang (wangx_ddpt@yusur.tech)
 * @date: 2023-07-04 20:33:47
 * @last_author: Xing Wang (wangx_ddpt@yusur.tech)
 * @last_edit_time: 2023-07-31 11:43:00
 */

#include "tool.h"
#include <argp.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

struct input_arg {
	uint32_t offset;
	uint32_t duration;
};

int create_tables(int fd, struct input_arg *arg);
void delete_tables(int fd);
void run_case(int fd, struct input_arg *arg);
void share_array_store(int fd, struct input_arg *arg, int shift);
void unshare_array_query(int fd, struct input_arg *arg, int shift);
void unshare_array_readclear(int fd, struct input_arg *arg, int shift);
void hash_operator(int fd, struct input_arg *arg, int shift);
int case2_array_store(int fd, uint8_t tbl_id, uint8_t data_len, int shift);
int case2_array_check(int fd, uint8_t tbl_id, uint8_t data_len, int shift);
int case2_array_readclear(int fd, uint8_t tbl_id, uint8_t data_len, int shift);
int case2_hash_insert(int fd, uint8_t tbl_id, uint8_t key_len, uint8_t value_len, int shift);
int case2_hash_delete(int fd, uint8_t tbl_id, uint8_t key_len, uint8_t value_len, int shift);

static struct argp_option options_case2[] = {
	{"offset",	'o',	"Dec",		0,	"table spec offset"},
	{"time",	't',	"Dec",		0,	"execution durtion"},
	{ 0 },
};

static error_t parse_opt_case2(int key, char *arg, struct argp_state *state)
{
	struct input_arg *argu = (struct input_arg *)(state->input);

	switch (key) {
	case 'o':
		argu->offset = atoi(arg);
		break;
	case 't':
		argu->duration = atoi(arg);
		break;
	default:
		break;
	}

	return 0;
}
/* argp parser. */
static struct argp argp_case2 = {
	.options = options_case2,
	.parser = parse_opt_case2,
	.args_doc = NULL,
	.doc = NULL,
	.children = NULL,
	.help_filter = NULL,
	.argp_domain = NULL,
};


int func_case2_main(int argc, char *argv[])
{
	struct input_arg param = {0};
	int fd;
	argp_parse(&argp_case2, argc, argv, 0, 0, &param);
	
	fd = hados_doe_request_fd();
	CHECK(fd == -1, "open doe handle failed.\n");

	if (create_tables(fd, &param) != 0)
		return 1;

	run_case(fd, &param);

	delete_tables(fd);

	hados_doe_release_fd(fd);

	return 0;
}

int create_tables(int fd, struct input_arg *arg)
{
	int ret, i;

	i = arg->offset;

	// create table
	ret = hados_doe_create_arraytbl(fd, 1, tbl_depth, 1 + i, &tb1_cfg);
	CHECK(ret != DOE_RESULT_SUCCESS, "create table %d failed.\n", 1);

	ret = hados_doe_create_arraytbl(fd, 2, tbl_depth, 1 + i, &tb2_cfg);
	CHECK(ret != DOE_RESULT_SUCCESS, "create table %d failed.\n", 2);

	ret = hados_doe_create_arraytbl(fd, 3, tbl_depth, 16, &tb3_cfg);
	CHECK(ret != DOE_RESULT_SUCCESS, "create table %d failed.\n", 3);

	ret = hados_doe_create_arraytbl(fd, 4, tbl_depth, 16, &tb4_cfg);
	CHECK(ret != DOE_RESULT_SUCCESS, "create table %d failed.\n", 4);

	ret = hados_doe_create_arraytbl(fd, 5, tbl_depth, 16, &tb5_cfg);
	CHECK(ret != DOE_RESULT_SUCCESS, "create table %d failed.\n", 5);

	ret = hados_doe_create_arraytbl(fd, 6, tbl_depth, 16, &tb6_cfg);
	CHECK(ret != DOE_RESULT_SUCCESS, "create table %d failed.\n", 6);

	ret = hados_doe_create_arraytbl(fd, 7, tbl_depth, 64 + i, &tb7_cfg);
	CHECK(ret != DOE_RESULT_SUCCESS, "create table %d failed.\n", 7);

	ret = hados_doe_create_arraytbl(fd, 8, tbl_depth, 64 + i, &tb8_cfg);
	CHECK(ret != DOE_RESULT_SUCCESS, "create table %d failed.\n", 8);

	ret = hados_doe_create_arraytbl(fd, 9, tbl_depth, 128, &tb9_cfg);
	CHECK(ret != DOE_RESULT_SUCCESS, "create table %d failed.\n", 9);

	
	ret = hados_doe_create_hashtbl(fd, 10, tbl_depth, 3 + i, 1 + i, &tb10_cfg);
	CHECK(ret != DOE_RESULT_SUCCESS, "create table %d failed.\n", 10);

	ret = hados_doe_create_hashtbl(fd, 11, tbl_depth, 3 + i, 64 + i, &tb11_cfg);
	CHECK(ret != DOE_RESULT_SUCCESS, "create table %d failed.\n", 11);

	ret = hados_doe_create_hashtbl(fd, 12, tbl_depth, 3 + i, 128, &tb12_cfg);
	CHECK(ret != DOE_RESULT_SUCCESS, "create table %d failed.\n", 12);

	ret = hados_doe_create_hashtbl(fd, 13, tbl_depth, 64 + i, 1 + i, &tb13_cfg);
	CHECK(ret != DOE_RESULT_SUCCESS, "create table %d failed.\n", 13);

	ret = hados_doe_create_hashtbl(fd, 14, tbl_depth, 64 + i, 64 + i, &tb14_cfg);
	CHECK(ret != DOE_RESULT_SUCCESS, "create table %d failed.\n", 14);

	ret = hados_doe_create_hashtbl(fd, 15, tbl_depth, 64 + i, 128, &tb15_cfg);
	CHECK(ret != DOE_RESULT_SUCCESS, "create table %d failed.\n", 15);

	ret = hados_doe_create_hashtbl(fd, 16, tbl_depth, 128, 1 + i, &tb16_cfg);
	CHECK(ret != DOE_RESULT_SUCCESS, "create table %d failed.", 16);

	ret = hados_doe_create_hashtbl(fd, 17, tbl_depth, 128, 64 + i, &tb17_cfg);
	CHECK(ret != DOE_RESULT_SUCCESS, "create table %d failed.\n", 17);

	ret = hados_doe_create_hashtbl(fd, 18, tbl_depth, 128, 128, &tb18_cfg);
	CHECK(ret != DOE_RESULT_SUCCESS, "create table %d failed.\n", 18);

	return ret;
}

void delete_tables(int fd)
{
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
}

int get_random_number()
{
	srand(time(NULL));
	return random() % 5;
}

void run_case(int fd, struct input_arg *arg)
{
	int random, index;
	time_t now, prev = time(NULL);
	do {
		random = get_random_number();
		fprintf(stdout, "Random operator is %d\n", random);

		if (random == 0)
			share_array_store(fd, arg, index);
		else if (random == 1)
			unshare_array_query(fd, arg, index);
		else if (random == 2)
			unshare_array_readclear(fd, arg, index);
		else if (random == 3 || random == 4)
			hash_operator(fd, arg, index);

		index++;

		now = time(NULL);
	} while (now - prev < arg->duration);

	fprintf(stderr, "The case is complete. prev=%ld, now=%ld\n",
			 prev, now);
}

void share_array_store(int fd, struct input_arg *arg, int shift)
{
	case2_array_store(fd, 2, 1 + arg->offset, shift);
	case2_array_store(fd, 8, 64 + arg->offset, shift);
	case2_array_store(fd, 9, 128, shift);
}

void unshare_array_query(int fd, struct input_arg *arg, int shift)
{
	case2_array_check(fd, 1, 1 + arg->offset, shift);
	case2_array_check(fd, 3, 16, shift);
	case2_array_check(fd, 4, 16, shift);
	case2_array_check(fd, 5, 16, shift);
	case2_array_check(fd, 6, 16, shift);
	case2_array_check(fd, 7, 64 + arg->offset, shift);
}

void unshare_array_readclear(int fd, struct input_arg *arg, int shift)
{
	case2_array_readclear(fd, 1, 1 + arg->offset, shift);
	case2_array_readclear(fd, 3, 16, shift);
	case2_array_readclear(fd, 4, 16, shift);
	case2_array_readclear(fd, 5, 16, shift);
	case2_array_readclear(fd, 6, 16, shift);
	case2_array_readclear(fd, 7, 64 + arg->offset, shift);
}

void hash_tables_insert(int fd, struct input_arg *arg, int shift)
{
	int index = arg->offset;
	
	case2_hash_insert(fd, 10, 3 + index, 1 + index, shift);
	case2_hash_insert(fd, 11, 3 + index, 64 + index, shift);
	case2_hash_insert(fd, 12, 3 + index, 128, shift);
	case2_hash_insert(fd, 13, 64 + index, 1 + index, shift);
	case2_hash_insert(fd, 14, 64 + index, 64 + index, shift);
	case2_hash_insert(fd, 15, 64 + index, 128, shift);
	case2_hash_insert(fd, 16, 128, 1 + index, shift);
	case2_hash_insert(fd, 17, 128, 64 + index, shift);
	case2_hash_insert(fd, 18, 128, 128, shift);
}

void hash_tables_delete(int fd, struct input_arg *arg, int shift)
{
	int index = arg->offset;
	
	case2_hash_delete(fd, 10, 3 + index, 1 + index, shift);
	case2_hash_delete(fd, 11, 3 + index, 64 + index, shift);
	case2_hash_delete(fd, 12, 3 + index, 128, shift);
	case2_hash_delete(fd, 13, 64 + index, 1 + index, shift);
	case2_hash_delete(fd, 14, 64 + index, 64 + index, shift);
	case2_hash_delete(fd, 15, 64 + index, 128, shift);
	case2_hash_delete(fd, 16, 128, 1 + index, shift);
	case2_hash_delete(fd, 17, 128, 64 + index, shift);
	case2_hash_delete(fd, 18, 128, 128, shift);
}

bool hash_inserted = false;
void hash_operator(int fd, struct input_arg *arg, int shift)
{
	if (hash_inserted)
		hash_tables_delete(fd, arg, shift);
	else
		hash_tables_insert(fd, arg, shift);
	
	hash_inserted = !hash_inserted;
}

int case2_array_store(int fd, uint8_t tbl_id, uint8_t data_len, int shift)
{
	char *item, *in;
	int item_len, ret;

	item_len = data_len + 4;
	in = calloc(batch_num, item_len);

	for (int i = 0; i < tbl_depth / batch_num; ++i) {
		for (int j = 0; j < batch_num; ++j) {
			item = in + j * item_len;
			int index = i * batch_num + j;
			*(int *)item = index;
			*(item + 4) = (index + shift) % 256;
		}

		if (hados_doe_array_store_batch(fd, tbl_id, batch_num, in) != batch_num) {
			fprintf(stderr, "store data to array table %d failed. index from %d to %d.\n",
				tbl_id, i * batch_num, (i + 1) * batch_num - 1);
			ret = 1;
			goto out;
		}
	}

	ret = 0;
out:
	free(in);
	return 1;
}

int case2_array_check(int fd, uint8_t tbl_id, uint8_t data_len, int shift)
{
	int ret = 0;
	int item_len = data_len + 4;
	char *in = calloc(batch_num, sizeof(int));
	char *out = calloc(batch_num, item_len);
	for(int i = 0; i < tbl_depth/batch_num; ++i) {
		for (int j = 0; j < batch_num; ++j) {
			*(int *)(in + j * sizeof(int)) = i * batch_num + j;
		}

		memset(out, 0, batch_num * item_len);

		int ret = hados_doe_array_load_batch(fd, tbl_id, batch_num, in, out);
		if (ret != batch_num) {
			fprintf(stderr, "load data from array table %d failed. index from %d to %d.\n",
				tbl_id, i * batch_num, (i + 1) * batch_num - 1);
			ret = 1;
			goto out;
		}
	}

out:
	free(in);
	free(out);

	return ret;
}

int case2_array_readclear(int fd, uint8_t tbl_id, uint8_t data_len, int shift)
{
	int ret = 0;
	int item_len = data_len + 4;
	char *in = calloc(batch_num, sizeof(int));
	char *out = calloc(batch_num, item_len);
	for(int i = 0; i < tbl_depth/batch_num; ++i) {
		for (int j = 0; j < batch_num; ++j) {
			*(int *)(in + j * 4) = i * batch_num + j;
		}

		memset(out, 0, batch_num * item_len);
		int ret = hados_doe_array_readclear_batch(fd, tbl_id, batch_num, in, out);
		if (ret != batch_num) {
			fprintf(stderr, "readclear data to array table %d failed. index from %d to %d.\n",
				tbl_id, i * batch_num, (i + 1) * batch_num - 1);
			ret = 1;
			goto out;
		}
	}

out:
	free(in);
	free(out);

	return ret;
}

int case2_hash_insert(int fd, uint8_t tbl_id, uint8_t key_len, uint8_t value_len, int shift)
{
	int ret = 0;
	char *tmp;
	int item_len = key_len + value_len;
	char *in = calloc(batch_num, item_len);
	for (int i = 0; i < tbl_depth / batch_num; ++i) {
		for (int j = 0; j < batch_num; ++j) {
			tmp = in + j * item_len;
			generate_hash_key(i * batch_num + j, tmp);
			*(tmp + key_len) = (tmp[0] + shift) % 256;
		}

		if (hados_doe_hash_insert_batch(fd, tbl_id, batch_num, in) != batch_num) {
			fprintf(stderr, "insert data to hash table %d failed. index from %d to %d.\n",
				tbl_id, i * batch_num, (i + 1) * batch_num - 1);
			ret = 1;
			goto err;
		}
	}
err:
	free(in);
	return ret;
}

int case2_hash_delete(int fd, uint8_t tbl_id, uint8_t key_len, uint8_t value_len, int shift)
{
	int ret = 0;
	char *tmp;
	int item_len = key_len + value_len;
	char *in = calloc(batch_num, key_len);
	for(int i = 0; i < tbl_depth/batch_num; ++i) {
		for (int j = 0; j < batch_num; ++j) {
			tmp = in + j * key_len;
			generate_hash_key(i * batch_num + j, tmp);
		}

		int ret = hados_doe_hash_delete_batch(fd, tbl_id, batch_num, in);
		if (ret != batch_num) {
			fprintf(stderr, "delete data to hash table %d failed. index from %d to %d. success:%d\n",
				tbl_id, i * batch_num, (i + 1) * batch_num - 1, ret);
			ret = 1;
			goto error;
		}
	}
error:
	free(in);

	return ret;
}






