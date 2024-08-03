/*
 * @Author: Xing Wang (wangx_ddpt@yusur.tech)
 * @date: 2022-12-09 17:07:53
 * @last_author: Xing Wang (wangx_ddpt@yusur.tech)
 * @last_edit_time: 2022-12-12 11:42:48
 */
extern "C" {
#include "hados_doe.h"
}

#include <iostream>
#include <string.h>
#include <memory>

#include <pthread.h>
#include <benchmark/benchmark.h>

pthread_mutex_t mutex;

static char value[] = "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz";

template <uint8_t DATA_LEN>
class array_batch_test
{
public:
	struct item_t {
		uint32_t    index;
		char        data[DATA_LEN];
	};

	struct index_t {
		int index;
	};

public:
	array_batch_test(int fd, uint8_t tbl_id, uint32_t depth)
		: _fd(fd), _tbl_id(tbl_id), _depth(depth)
	{}

	int init_env()
	{
		/* malloc data for batch insert */
		_data = (item_t *)malloc(sizeof(item_t) * _depth);
		memset(_data, 0, sizeof(item_t) * _depth);

		/* malloc index for batch load */
		_indexs = (uint32_t*)malloc(sizeof(uint32_t) * _depth);

		/* init data and index */
		for (uint32_t i = 0; i < _depth; ++i) {
			_indexs[i] = i;
			_data[i].index = i;
			memcpy(_data[i].data, "hello", sizeof("hello"));
		}

		/* create table */
		pthread_mutex_lock(&mutex);
		auto ret = hados_doe_create_arraytbl(_fd, _tbl_id, _depth, DATA_LEN, NULL);
		if (ret != DOE_RESULT_SUCCESS)
			std::cout << "create array table failed. ret=" << ret << std::endl;
		pthread_mutex_unlock(&mutex);

		return ret;
	}

void clear_env()
{
	pthread_mutex_lock(&mutex);
	hados_doe_delete_arraytbl(_fd, _tbl_id);
	pthread_mutex_unlock(&mutex);

	free(_data);
	free(_indexs);
}

bool store(uint32_t index)
{
	return hados_doe_array_store(_fd, _tbl_id, _indexs[index], _data, DATA_LEN) == DOE_RESULT_SUCCESS;
}

bool batch_store(uint32_t count, uint32_t index)
{
	int err = 0;

	if (hados_doe_array_store_batch(_fd, _tbl_id, count, _data + index) != count) {
		err++;
	}

	return !err;
}

bool load(uint32_t index, void *out_data)
{
	return hados_doe_array_load(_fd, _tbl_id, _indexs[index], out_data, DATA_LEN) == DOE_RESULT_SUCCESS;
}

bool read_clear(uint32_t index, void *out_data)
{
	return hados_doe_array_read_clear(_fd, _tbl_id, _indexs[index], out_data, DATA_LEN) == DOE_RESULT_SUCCESS;
}

bool batch_load(uint32_t count, uint32_t index, void *out_data)
{
	int err = 0;

	if (hados_doe_array_load_batch(_fd, _tbl_id, count, _indexs + index, out_data) != count) {
		err++;
	}

	return !err;
}

bool batch_readclear(uint32_t count, uint32_t index, void *out_data)
{
	int total = 0, err = 0;

	if (hados_doe_array_readclear_batch(_fd, _tbl_id, count, _indexs + index, out_data) != count) {
		err++;
	}
	total++;

	return !err;
}

private:
	item_t*     _data;
	uint32_t*   _indexs;
	int         _fd;
	uint8_t     _tbl_id;
	uint32_t    _depth;
};

template <uint8_t KEY_LEN, uint8_t VAL_LEN>
class hash_batch_test
{
	struct item_t
	{
		char key[KEY_LEN];
		char val[VAL_LEN];
	} __attribute__((packed));

	struct key_t {
	char key[KEY_LEN];
	} __attribute__((packed));

private:
	item_t*     _data;
	char*      _keys;
	int         _fd;
	uint8_t     _tbl_id;
	uint32_t    _depth;
	bool        _init = true;

public:
	hash_batch_test(int fd, uint8_t tbl_id, uint32_t depth)
	: _fd(fd), _tbl_id(tbl_id), _depth(depth)
	{
		_data = new item_t[_depth];
		memset(_data, 0, sizeof(item_t) * _depth);

		// _keys = new key_t[_depth];
		_keys = (char *)malloc(KEY_LEN * _depth);
		memset(_keys, 0, KEY_LEN * _depth);


		for (uint32_t i = 0; i < _depth; ++i) {
			std::string tmp("hello");
			tmp += std::to_string(i);

			memcpy(_data[i].key, tmp.c_str(), tmp.length());
			memcpy(_keys + KEY_LEN * i, tmp.c_str(), tmp.length());

			strcpy(_data[i].val, "world");
		}
	}

	~hash_batch_test()
	{
		delete[] _data;
		free(_keys);
	}

	int init_env(bool init_data = false)
	{
		pthread_mutex_lock(&mutex);
		doe_result_t ret = hados_doe_create_hashtbl(_fd, _tbl_id, _depth, KEY_LEN, VAL_LEN, NULL);
		if (ret != DOE_RESULT_SUCCESS) {
			pthread_mutex_unlock(&mutex);
			fprintf(stderr, "call yusdoe_create_hashtbl failed. _fd=%d, _tbl_id=%d, _depth=%d, key_len=%d, val_len=%d\n",
				_fd, _tbl_id, _depth, KEY_LEN, VAL_LEN);
			return ret;
		}
		pthread_mutex_unlock(&mutex);

		int32_t count = 10000;
		if (init_data) {
			int success = hados_doe_hash_insert_batch(_fd, _tbl_id, count, _data);
			if (success != count) {
				std::cout << "insert data falied. success=" << success << ", count=" << count << std::endl;
				ret = DOE_RESULT_TBL_ERR;
			}
		}

		return ret;
	}

	void clear_env()
	{
		pthread_mutex_lock(&mutex);
		hados_doe_delete_hashtbl(_fd, _tbl_id);
		pthread_mutex_unlock(&mutex);
	}

	bool insert(int index)
	{
		int err = 0;

		if (hados_doe_hash_insert(_fd, _tbl_id, _keys + index * KEY_LEN, KEY_LEN, value, VAL_LEN) != DOE_RESULT_SUCCESS) {
			err++;
		}

		return !err;
	}

	bool batch_insert(uint32_t count, uint32_t index)
	{
		int err = 0;

		if (hados_doe_hash_insert_batch(_fd, _tbl_id, count, _data + index) != count) {
			err++;
		}

		return !err;
	}

	bool query(int index)
	{
		int total = 0, err = 0;

		char buf[256];
		if (hados_doe_hash_query(_fd, _tbl_id, _keys + index * KEY_LEN, KEY_LEN, buf, 255) != DOE_RESULT_SUCCESS) {
			err++;
		}
		total++;

		return !err;
	}

	bool batch_query(uint32_t count, uint32_t index)
	{
		uint32_t err = 0, ret;

		auto tmp = std::make_unique<item_t[]>(_depth);
		ret = hados_doe_hash_query_batch(_fd, _tbl_id, count, _keys + index * KEY_LEN, tmp.get());
		if (ret != count) {
			err++;
		}

		return !err;
	}

	bool del(int index)
	{
		int err = 0, ret;

		ret = hados_doe_hash_delete(_fd, _tbl_id, _keys + index * KEY_LEN, KEY_LEN);
		if (ret != DOE_RESULT_SUCCESS) {
			err++;
		}

		return !err;
	}

	bool batch_del(int32_t count, uint32_t index)
	{
		int err = 0, ret;

		ret = hados_doe_hash_delete_batch(_fd, _tbl_id, count, _keys + index * KEY_LEN);
		if (ret != count) {
			err++;
		}

		return !err;
	}
};

/* hash insert */
template <class Q>
void BM_HashInsert(benchmark::State& state) {
	uint8_t tbl_id;
	uint32_t index, tbl_deepth = 100000;
	int fd;

	fd = hados_doe_request_fd();
	tbl_id = state.thread_index();
	index = 0;

	auto hashtbl = Q(fd, tbl_id, tbl_deepth);
	hashtbl.init_env();
	
	for (auto _ : state) {
		hashtbl.insert(index);
		index++;
	}

	hashtbl.clear_env();

	hados_doe_release_fd(fd);
}

/* hash batch insert */
template <class Q>
void BM_HashBatchInsert(benchmark::State& state) {
	uint8_t tbl_id;
	uint32_t index, count, tbl_deepth = 1000000;
	int fd;

	fd = hados_doe_request_fd();
	tbl_id = state.thread_index();
	index = 0;
	count = state.range(0);

	auto hashtbl = Q(fd, tbl_id, tbl_deepth);
	hashtbl.init_env();
	
	for (auto _ : state) {
		hashtbl.batch_insert(count, index);
		index += count;
	}

	hashtbl.clear_env();

	hados_doe_release_fd(fd);
}

/* hash query */
template <class Q>
void BM_HashQuery(benchmark::State& state) {
	uint8_t tbl_id;
	uint32_t index, tbl_deepth = 100000;
	int fd;

	fd = hados_doe_request_fd();
	tbl_id = state.thread_index();
	index = 0;

	auto hashtbl = Q(fd, tbl_id, tbl_deepth);
	hashtbl.init_env(true);
	
	for (auto _ : state) {
		hashtbl.query(index);
		index++;
	}

	hashtbl.clear_env();

	hados_doe_release_fd(fd);
}

/* hash batch insert */
template <class Q>
void BM_HashBatchQuery(benchmark::State& state) {
	uint8_t tbl_id;
	uint32_t index, count, tbl_deepth = 1000000;
	int fd;

	fd = hados_doe_request_fd();
	tbl_id = state.thread_index();
	index = 0;
	count = state.range(0);

	auto hashtbl = Q(fd, tbl_id, tbl_deepth);
	hashtbl.init_env(true);
	
	for (auto _ : state) {
		hashtbl.batch_query(count, index);
		index += count;
	}

	hashtbl.clear_env();

	hados_doe_release_fd(fd);
}

/* hash delete */
template <class Q>
void BM_HashDelete(benchmark::State& state) {
	uint8_t tbl_id;
	uint32_t index, tbl_deepth = 100000;
	int fd;

	fd = hados_doe_request_fd();
	tbl_id = state.thread_index();
	index = 0;

	auto hashtbl = Q(fd, tbl_id, tbl_deepth);
	hashtbl.init_env(true);
	
	for (auto _ : state) {
		hashtbl.del(index);
		index++;
	}

	hashtbl.clear_env();

	hados_doe_release_fd(fd);
}

/* batch delete */
template <class Q>
void BM_HashBatchDelete(benchmark::State& state) {
	uint8_t tbl_id;
	uint32_t index, count, tbl_deepth = 1000000;
	int fd;

	fd = hados_doe_request_fd();
	tbl_id = state.thread_index();
	index = 0;
	count = state.range(0);

	auto hashtbl = Q(fd, tbl_id, tbl_deepth);
	hashtbl.init_env(true);
	
	for (auto _ : state) {
		hashtbl.batch_del(count, index);
		index += count;
	}

	hashtbl.clear_env();

	hados_doe_release_fd(fd);
}

BENCHMARK_TEMPLATE(BM_HashInsert, hash_batch_test<36, 128>)->Iterations(100000)->ThreadRange(1, 16);
BENCHMARK_TEMPLATE(BM_HashInsert, hash_batch_test<60, 128>)->Iterations(100000)->ThreadRange(1, 16);
BENCHMARK_TEMPLATE(BM_HashInsert, hash_batch_test<96, 128>)->Iterations(100000)->ThreadRange(1, 16);

/* batch test needn't use multi-thread */
BENCHMARK_TEMPLATE(BM_HashBatchInsert, hash_batch_test<36, 128>)
	->Iterations(10)->Arg(100)->Arg(1000)->Arg(10000)->Arg(20000);
BENCHMARK_TEMPLATE(BM_HashBatchInsert, hash_batch_test<60, 128>)
	->Iterations(10)->Arg(100)->Arg(1000)->Arg(10000)->Arg(20000);
BENCHMARK_TEMPLATE(BM_HashBatchInsert, hash_batch_test<96, 128>)
	->Iterations(10)->Arg(100)->Arg(1000)->Arg(10000)->Arg(20000);

BENCHMARK_TEMPLATE(BM_HashQuery, hash_batch_test<36, 128>)->Iterations(100000)->ThreadRange(1, 16);
BENCHMARK_TEMPLATE(BM_HashQuery, hash_batch_test<60, 128>)->Iterations(100000)->ThreadRange(1, 16);
BENCHMARK_TEMPLATE(BM_HashQuery, hash_batch_test<96, 128>)->Iterations(100000)->ThreadRange(1, 16);

/* batch test needn't use multi-thread */
BENCHMARK_TEMPLATE(BM_HashBatchQuery, hash_batch_test<36, 128>)
	->Iterations(10)->Arg(100)->Arg(1000)->Arg(10000)->Arg(20000);
BENCHMARK_TEMPLATE(BM_HashBatchQuery, hash_batch_test<60, 128>)
	->Iterations(10)->Arg(100)->Arg(1000)->Arg(10000)->Arg(20000);
BENCHMARK_TEMPLATE(BM_HashBatchQuery, hash_batch_test<96, 128>)
	->Iterations(10)->Arg(100)->Arg(1000)->Arg(10000)->Arg(20000);

BENCHMARK_TEMPLATE(BM_HashDelete, hash_batch_test<36, 128>)->Iterations(100000)->ThreadRange(1, 16);
BENCHMARK_TEMPLATE(BM_HashDelete, hash_batch_test<60, 128>)->Iterations(100000)->ThreadRange(1, 16);
BENCHMARK_TEMPLATE(BM_HashDelete, hash_batch_test<96, 128>)->Iterations(100000)->ThreadRange(1, 16);

/* batch test needn't use multi-thread */
BENCHMARK_TEMPLATE(BM_HashBatchDelete, hash_batch_test<36, 128>)
	->Iterations(10)->Arg(100)->Arg(1000)->Arg(10000)->Arg(20000);
BENCHMARK_TEMPLATE(BM_HashBatchDelete, hash_batch_test<60, 128>)
	->Iterations(10)->Arg(100)->Arg(1000)->Arg(10000)->Arg(20000);
BENCHMARK_TEMPLATE(BM_HashBatchDelete, hash_batch_test<96, 128>)
	->Iterations(10)->Arg(100)->Arg(1000)->Arg(10000)->Arg(20000);


/* array insert */
template <class Q>
void BM_ArrayStore(benchmark::State& state) {
	uint8_t tbl_id;
	uint32_t index, tbl_deepth = 100000;
	int fd;

	fd = hados_doe_request_fd();
	tbl_id = state.thread_index();
	index = 0;

	auto arraytbl = Q(fd, tbl_id, tbl_deepth);
	arraytbl.init_env();
	
	for (auto _ : state) {
		arraytbl.store(index);
		index++;
	}

	arraytbl.clear_env();

	hados_doe_release_fd(fd);
}

/* array batch store */
template <class Q>
void BM_ArrayBatchStore(benchmark::State& state) {
	uint8_t tbl_id;
	uint32_t index, count, tbl_deepth = 1000000;
	int fd;

	fd = hados_doe_request_fd();
	tbl_id = state.thread_index();
	index = 0;
	count = state.range(0);

	auto arraytbl = Q(fd, tbl_id, tbl_deepth);
	arraytbl.init_env();
	
	for (auto _ : state) {
		arraytbl.batch_store(count, index);
		index += count;
	}

	arraytbl.clear_env();

	hados_doe_release_fd(fd);
}

/* array readclear */
template <class Q>
void BM_ArrayReadclear(benchmark::State& state) {
	uint8_t tbl_id;
	uint32_t index, tbl_deepth = 100000;
	int fd;
	struct Q::item_t out_data;

	fd = hados_doe_request_fd();
	tbl_id = state.thread_index();
	index = 0;

	auto arraytbl = Q(fd, tbl_id, tbl_deepth);
	arraytbl.init_env();
	
	for (auto _ : state) {
		arraytbl.read_clear(index, &out_data);
		index++;
	}

	arraytbl.clear_env();

	hados_doe_release_fd(fd);
}

/* array batch readclear */
template <class Q>
void BM_ArrayBatchReadclear(benchmark::State& state) {
	uint8_t tbl_id;
	uint32_t index, count, tbl_deepth = 1000000;
	int fd;
	void *out_data;

	fd = hados_doe_request_fd();
	tbl_id = state.thread_index();
	index = 0;
	count = state.range(0);

	out_data = malloc(sizeof(struct Q::item_t) * count);

	auto arraytbl = Q(fd, tbl_id, tbl_deepth);
	arraytbl.init_env();
	
	for (auto _ : state) {
		arraytbl.batch_readclear(count, index, out_data);
		index += count;
	}

	free(out_data);

	arraytbl.clear_env();

	hados_doe_release_fd(fd);
}

/* array load */
template <class Q>
void BM_ArrayLoad(benchmark::State& state) {
	uint8_t tbl_id;
	uint32_t index, tbl_deepth = 100000;
	int fd;
	struct Q::item_t out_data;

	fd = hados_doe_request_fd();
	tbl_id = state.thread_index();
	index = 0;

	auto arraytbl = Q(fd, tbl_id, tbl_deepth);
	arraytbl.init_env();
	
	for (auto _ : state) {
		arraytbl.load(index, (void*)&out_data);
		index++;
	}

	arraytbl.clear_env();

	hados_doe_release_fd(fd);
}

/* array batch load */
template <class Q>
void BM_ArrayBatchLoad(benchmark::State& state) {
	uint8_t tbl_id;
	uint32_t index, count, tbl_deepth = 1000000;
	int fd;
	void *out_data;

	fd = hados_doe_request_fd();
	tbl_id = state.thread_index();
	index = 0;
	count = state.range(0);

	out_data = malloc(sizeof(struct Q::item_t) * count);

	auto arraytbl = Q(fd, tbl_id, tbl_deepth);
	arraytbl.init_env();
	
	for (auto _ : state) {
		arraytbl.batch_load(count, index, out_data);
		index += count;
	}

	free(out_data);

	arraytbl.clear_env();

	hados_doe_release_fd(fd);
}

BENCHMARK_TEMPLATE(BM_ArrayStore, array_batch_test<16>)->Iterations(10000)->ThreadRange(1, 16);
BENCHMARK_TEMPLATE(BM_ArrayStore, array_batch_test<32>)->Iterations(10000)->ThreadRange(1, 16);
BENCHMARK_TEMPLATE(BM_ArrayStore, array_batch_test<76>)->Iterations(10000)->ThreadRange(1, 16);
BENCHMARK_TEMPLATE(BM_ArrayStore, array_batch_test<128>)->Iterations(10000)->ThreadRange(1, 16);

/* batch test needn't use multi-thread */
BENCHMARK_TEMPLATE(BM_ArrayBatchStore, array_batch_test<16>)
	->Iterations(10)->Arg(100)->Arg(1000)->Arg(10000)->Arg(20000);
BENCHMARK_TEMPLATE(BM_ArrayBatchStore, array_batch_test<32>)
	->Iterations(10)->Arg(100)->Arg(1000)->Arg(10000)->Arg(20000);
BENCHMARK_TEMPLATE(BM_ArrayBatchStore, array_batch_test<76>)
	->Iterations(10)->Arg(100)->Arg(1000)->Arg(10000)->Arg(20000);
BENCHMARK_TEMPLATE(BM_ArrayBatchStore, array_batch_test<128>)
	->Iterations(10)->Arg(100)->Arg(1000)->Arg(10000)->Arg(20000);

BENCHMARK_TEMPLATE(BM_ArrayReadclear, array_batch_test<16>)->Iterations(10000)->ThreadRange(1, 16);
BENCHMARK_TEMPLATE(BM_ArrayReadclear, array_batch_test<32>)->Iterations(10000)->ThreadRange(1, 16);
BENCHMARK_TEMPLATE(BM_ArrayReadclear, array_batch_test<76>)->Iterations(10000)->ThreadRange(1, 16);
BENCHMARK_TEMPLATE(BM_ArrayReadclear, array_batch_test<128>)->Iterations(10000)->ThreadRange(1, 16);

/* batch test needn't use multi-thread */
BENCHMARK_TEMPLATE(BM_ArrayBatchReadclear, array_batch_test<16>)
	->Iterations(10)->Arg(100)->Arg(1000)->Arg(10000)->Arg(20000);
BENCHMARK_TEMPLATE(BM_ArrayBatchReadclear, array_batch_test<32>)
	->Iterations(10)->Arg(100)->Arg(1000)->Arg(10000)->Arg(20000);
BENCHMARK_TEMPLATE(BM_ArrayBatchReadclear, array_batch_test<76>)
	->Iterations(10)->Arg(100)->Arg(1000)->Arg(10000)->Arg(20000);
BENCHMARK_TEMPLATE(BM_ArrayBatchReadclear, array_batch_test<128>)
	->Iterations(10)->Arg(100)->Arg(1000)->Arg(10000)->Arg(20000);

BENCHMARK_TEMPLATE(BM_ArrayLoad, array_batch_test<16>)->Iterations(10000)->ThreadRange(1, 16);
BENCHMARK_TEMPLATE(BM_ArrayLoad, array_batch_test<32>)->Iterations(10000)->ThreadRange(1, 16);
BENCHMARK_TEMPLATE(BM_ArrayLoad, array_batch_test<76>)->Iterations(10000)->ThreadRange(1, 16);
BENCHMARK_TEMPLATE(BM_ArrayLoad, array_batch_test<128>)->Iterations(10000)->ThreadRange(1, 16);

/* batch test needn't use multi-thread */
BENCHMARK_TEMPLATE(BM_ArrayBatchLoad, array_batch_test<16>)
	->Iterations(10)->Arg(100)->Arg(1000)->Arg(10000)->Arg(20000);
BENCHMARK_TEMPLATE(BM_ArrayBatchLoad, array_batch_test<32>)
	->Iterations(10)->Arg(100)->Arg(1000)->Arg(10000)->Arg(20000);
BENCHMARK_TEMPLATE(BM_ArrayBatchLoad, array_batch_test<76>)
	->Iterations(10)->Arg(100)->Arg(1000)->Arg(10000)->Arg(20000);
BENCHMARK_TEMPLATE(BM_ArrayBatchLoad, array_batch_test<128>)
	->Iterations(10)->Arg(100)->Arg(1000)->Arg(10000)->Arg(20000);

BENCHMARK_MAIN();