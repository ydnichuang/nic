/**
 * @Author: Xing Wang (wangx_ddpt@yusur.tech)
 * @date: 2023-03-25 22:41:48
 * @last_author: Xing Wang (wangx_ddpt@yusur.tech)
 * @last_edit_time: 2024-03-04 17:35:21
 */

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "hados_doe.h"

static char value[] = "hello,world";

TEST_CASE("创建数组表")
{
	doe_result_t ret;
	uint8_t tbl1_id = 1;
	uint8_t tbl2_id = 2;
	uint8_t tbl3_id = 3;

	int doe_fd = hados_doe_request_fd();
	REQUIRE_MESSAGE(doe_fd != -1, "打开DOE文件描述符失败");

	ret = hados_doe_create_arraytbl(doe_fd, tbl2_id, 8000000, 32, NULL);
	CHECK_MESSAGE(ret == DOE_RESULT_SUCCESS, "创建32字节数组表失败");

	ret = hados_doe_create_arraytbl(doe_fd, tbl2_id, 8000000, 64, NULL);
	CHECK_MESSAGE(ret == DOE_RESULT_SUCCESS, "创建64字节数组表失败");

	ret = hados_doe_create_arraytbl(doe_fd, tbl3_id, 2000000, 128, NULL);
	CHECK_MESSAGE(ret == DOE_RESULT_SUCCESS, "创建128字节数组表失败");

	hados_doe_delete_arraytbl(doe_fd, tbl1_id);
	hados_doe_delete_arraytbl(doe_fd, tbl2_id);
	hados_doe_delete_arraytbl(doe_fd, tbl3_id);
}

void store_test(int fd, uint8_t tbl_id, int depth, int data_len)
{
	doe_result_t ret;

	ret = hados_doe_create_arraytbl(fd, tbl_id, depth, data_len, NULL);
	REQUIRE_MESSAGE(ret == DOE_RESULT_SUCCESS, "创建%d字节数组表失败", data_len);

	for(auto i = 0; i < depth; ++i) {
		ret = hados_doe_array_store(fd, tbl_id, i, value, 12);
		REQUIRE_MESSAGE(ret == DOE_RESULT_SUCCESS, "插入数据失败。index=%d", i);
	}

	hados_doe_delete_arraytbl(fd, tbl_id);
}

TEST_CASE("数据表store指令")
{
	uint8_t tbl1_id = 1;
	uint8_t tbl2_id = 2;
	uint8_t tbl3_id = 3;

	int doe_fd = hados_doe_request_fd();
	REQUIRE_MESSAGE(doe_fd != -1, "打开DOE文件描述符失败");

	store_test(doe_fd, tbl1_id, 8000000, 32);
	store_test(doe_fd, tbl2_id, 8000000, 64);
	store_test(doe_fd, tbl3_id, 2000000, 128);

	hados_doe_release_fd(doe_fd);
}

void load_test(int fd, uint8_t tbl_id, int depth, int data_len)
{
	doe_result_t ret;

	ret = hados_doe_create_arraytbl(fd, tbl_id, depth, data_len, NULL);
	REQUIRE_MESSAGE(ret == DOE_RESULT_SUCCESS, "创建%d字节数组表失败", data_len);

	for(auto i = 0; i < depth; ++i) {
		ret = hados_doe_array_store(fd, tbl_id, i, value, 11);
		REQUIRE_MESSAGE(ret == DOE_RESULT_SUCCESS, "插入数据失败。index=%d", i);
	}

	char buf[16] = {0};
	for(auto i = 0; i < depth; ++i) {
		ret = hados_doe_array_load(fd, tbl_id, i, buf, 11);
		REQUIRE_MESSAGE(ret == DOE_RESULT_SUCCESS, "load数据失败。%d", i);
		REQUIRE_MESSAGE(memcmp(value, buf, 11) == 0, "load数据失败。%d", i);
	}

	hados_doe_delete_arraytbl(fd, tbl_id);
}

TEST_CASE("数组表load指令")
{
	uint8_t tbl1_id = 1;
	uint8_t tbl2_id = 2;
	uint8_t tbl3_id = 3;

	int doe_fd = hados_doe_request_fd();
	REQUIRE_MESSAGE(doe_fd != -1, "打开DOE文件描述符失败");

	load_test(doe_fd, tbl1_id, 8000000, 32);
	load_test(doe_fd, tbl2_id, 8000000, 64);
	load_test(doe_fd, tbl3_id, 2000000, 128);

	hados_doe_release_fd(doe_fd);
}

void readclear_test(int fd, uint8_t tbl_id, int depth, int data_len)
{
	doe_result_t ret;

	ret = hados_doe_create_arraytbl(fd, tbl_id, depth, data_len, NULL);
	REQUIRE_MESSAGE(ret == DOE_RESULT_SUCCESS, "创建%d字节数组表失败", data_len);

	for(auto i = 0; i < depth; ++i) {
		ret = hados_doe_array_store(fd, tbl_id, i, value, 11);
		REQUIRE_MESSAGE(ret == DOE_RESULT_SUCCESS, "插入数据失败。index=%d", i);
	}

	char buf[16] = {0};
	for(auto i = 0; i < depth; ++i) {
		ret = hados_doe_array_read_clear(fd, tbl_id, i, buf, 11);
		REQUIRE_MESSAGE(ret == DOE_RESULT_SUCCESS, "readclear数据失败。%d", i);
		REQUIRE_MESSAGE(memcmp(value, buf, 11) == 0, "readclear数据失败。%d", i);
	}

	for(auto i = 0; i < depth; ++i) {
		ret = hados_doe_array_load(fd, tbl_id, i, buf, 11);
		REQUIRE_MESSAGE(ret == DOE_RESULT_SUCCESS, "load数据失败。%d", i);
		REQUIRE_MESSAGE(strlen(buf) == 0, "load数据失败。%d", i);
	}

	hados_doe_delete_arraytbl(fd, tbl_id);
}

TEST_CASE("数据表readclear指令")
{
	uint8_t tbl1_id = 1;
	uint8_t tbl2_id = 2;
	uint8_t tbl3_id = 3;

	int doe_fd = hados_doe_request_fd();
	REQUIRE_MESSAGE(doe_fd != -1, "打开DOE文件描述符失败");

	readclear_test(doe_fd, tbl1_id, 8000000, 32);
	readclear_test(doe_fd, tbl2_id, 8000000, 64);
	readclear_test(doe_fd, tbl3_id, 2000000, 128);

	hados_doe_release_fd(doe_fd);
}

TEST_CASE("哈希表创建")
{
	doe_result_t ret;
	uint8_t tbl1_id = 1;
	uint8_t tbl2_id = 2;
	uint8_t tbl3_id = 3;

	int doe_fd = hados_doe_request_fd();
	REQUIRE_MESSAGE(doe_fd != -1, "打开DOE文件描述符失败");

	ret = hados_doe_create_hashtbl(doe_fd, tbl2_id, 8000000, 32, 32, NULL);
	CHECK_MESSAGE(ret == DOE_RESULT_SUCCESS, "创建%d-%d字节数组表失败", 32, 32);

	ret = hados_doe_create_hashtbl(doe_fd, tbl2_id, 4000000, 64, 64, NULL);
	CHECK_MESSAGE(ret == DOE_RESULT_SUCCESS, "创建%d-%d字节数组表失败", 64, 64);

	ret = hados_doe_create_hashtbl(doe_fd, tbl3_id, 2000000, 128, 128, NULL);
	CHECK_MESSAGE(ret == DOE_RESULT_SUCCESS, "创建%d-%d字节数组表失败", 128, 128);

	hados_doe_delete_arraytbl(doe_fd, tbl1_id);
	hados_doe_delete_arraytbl(doe_fd, tbl2_id);
	hados_doe_delete_arraytbl(doe_fd, tbl3_id);
}

void insert_test(int fd, uint8_t tbl_id, int depth, int key_len, int val_len)
{
	doe_result_t ret;

	ret = hados_doe_create_hashtbl(fd, tbl_id, depth, key_len, val_len, NULL);
	REQUIRE_MESSAGE(ret == DOE_RESULT_SUCCESS, "创建%d-%d哈希表失败", key_len, val_len);

	for(auto i = 0; i < depth; ++i) {
		std::string key("hello");
		key += std::to_string(i);
		ret = hados_doe_hash_insert(fd, tbl_id, key.c_str(), key.length(), value, 11);
		REQUIRE_MESSAGE(ret == DOE_RESULT_SUCCESS, "插入数据失败。index=%d", i);
	}

	hados_doe_delete_hashtbl(fd, tbl_id);
}

TEST_CASE("哈希表insert指令")
{
	uint8_t tbl1_id = 1;
	uint8_t tbl2_id = 2;
	uint8_t tbl3_id = 3;

	int doe_fd = hados_doe_request_fd();
	REQUIRE_MESSAGE(doe_fd != -1, "打开DOE文件描述符失败");

	insert_test(doe_fd, tbl1_id, 8000000, 32, 32);
	insert_test(doe_fd, tbl2_id, 4000000, 64, 64);
	insert_test(doe_fd, tbl3_id, 2000000, 128, 128);

	hados_doe_release_fd(doe_fd);
}

void query_test(int fd, uint8_t tbl_id, int depth, int key_len, int val_len)
{
	doe_result_t ret;

	ret = hados_doe_create_hashtbl(fd, tbl_id, depth, key_len, val_len, NULL);
	REQUIRE_MESSAGE(ret == DOE_RESULT_SUCCESS, "创建%d-%d哈希表失败", key_len, val_len);

	for(auto i = 0; i < depth; ++i) {
		std::string key("hello");
		key += std::to_string(i);
		ret = hados_doe_hash_insert(fd, tbl_id, key.c_str(), key.length(), value, 11);
		REQUIRE_MESSAGE(ret == DOE_RESULT_SUCCESS, "插入数据失败。index=%d", i);
	}

	char buf[16] = {0};
	for(auto i = 0; i < depth; ++i) {
		std::string key("hello");
		key += std::to_string(i);
		ret = hados_doe_hash_query(fd, tbl_id, key.c_str(), key.length(), buf, sizeof(buf));
		REQUIRE_MESSAGE(ret == DOE_RESULT_SUCCESS, "readclear数据失败。%d", i);
		REQUIRE_MESSAGE(memcmp(value, buf, 11) == 0, "readclear数据失败。%d", i);
	}

	hados_doe_delete_hashtbl(fd, tbl_id);
}

TEST_CASE("哈希表query指令")
{
	uint8_t tbl1_id = 1;
	uint8_t tbl2_id = 2;
	uint8_t tbl3_id = 3;

	int doe_fd = hados_doe_request_fd();
	REQUIRE_MESSAGE(doe_fd != -1, "打开DOE文件描述符失败");

	query_test(doe_fd, tbl1_id, 8000000, 32, 32);
	query_test(doe_fd, tbl2_id, 4000000, 64, 64);
	query_test(doe_fd, tbl3_id, 2000000, 128, 128);

	hados_doe_release_fd(doe_fd);
}

void delete_test(int fd, uint8_t tbl_id, int depth, int key_len, int val_len)
{
	doe_result_t ret;

	ret = hados_doe_create_hashtbl(fd, tbl_id, depth, key_len, val_len, NULL);
	REQUIRE_MESSAGE(ret == DOE_RESULT_SUCCESS, "创建%d-%d哈希表失败", key_len, val_len);

	for(auto i = 0; i < depth; ++i) {
		std::string key("hello");
		key += std::to_string(i);
		ret = hados_doe_hash_insert(fd, tbl_id, key.c_str(), key.length(), value, 11);
		REQUIRE_MESSAGE(ret == DOE_RESULT_SUCCESS, "插入数据失败。index=%d", i);
	}

	for(auto i = 0; i < depth; ++i) {
		std::string key("hello");
		key += std::to_string(i);
		ret = hados_doe_hash_delete(fd, tbl_id, key.c_str(), key.length());
		REQUIRE_MESSAGE(ret == DOE_RESULT_SUCCESS, "删除数据失败。index=%d", i);
	}

	char buf[16] = {0};
	for(auto i = 0; i < depth; ++i) {
		std::string key("hello");
		key += std::to_string(i);
		ret = hados_doe_hash_query(fd, tbl_id, key.c_str(), key.length(), buf, sizeof(buf));
		REQUIRE_MESSAGE(ret != DOE_RESULT_SUCCESS, "readclear数据失败。%d", i);
	}

	hados_doe_delete_hashtbl(fd, tbl_id);
}

TEST_CASE("哈希表delete指令")
{
	uint8_t tbl1_id = 1;
	uint8_t tbl2_id = 2;
	uint8_t tbl3_id = 3;

	int doe_fd = hados_doe_request_fd();
	REQUIRE_MESSAGE(doe_fd != -1, "打开DOE文件描述符失败");

	delete_test(doe_fd, tbl1_id, 8000000, 32, 32);
	delete_test(doe_fd, tbl2_id, 4000000, 64, 64);
	delete_test(doe_fd, tbl3_id, 2000000, 128, 128);

	hados_doe_release_fd(doe_fd);
}




