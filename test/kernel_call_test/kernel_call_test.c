/*
 * @Author: Xing Wang (wangx_ddpt@yusur.tech)
 * @date: 2024-03-12 09:37:50
 * @last_author: Xing Wang (wangx_ddpt@yusur.tech)
 * @last_edit_time: 2024-03-13 16:58:16
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include "ys_doe_kapi.h"

static int kernel_call_test_init(void)
{
	int i, ret, depth, key_len, value_len, data_len;
	__u8 *array_batch_data;
	__u8 *hash_batch_key;
	__u8 *hash_batch_value;
	__u8 batch_key[12] = {0, 0, 0, 0, 1, 0, 0, 0, 2, 0, 0, 0};
	__u8 data[128] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
	__u8 key[96] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
	__u8 value[128] = {0};
	depth = 100000;
	value_len = 128;

	/* create array table */
	data_len = 16;
	ret = hados_doe_create_arraytbl(0, depth, data_len, NULL);
	if (ret != 0) {
		printk(KERN_INFO "DOE_KERNEL_TEST: create arraytbl(%d) failed!\n", data_len);
		return -1;
	}

	data_len = 32;
	ret = hados_doe_create_arraytbl(1, depth, data_len, NULL);
	if (ret != 0) {
		printk(KERN_INFO "DOE_KERNEL_TEST: create arraytbl(%d) failed!\n", data_len);
		goto error_create_arraytbl_32;
	}

	data_len = 76;
	ret = hados_doe_create_arraytbl(2, depth, data_len, NULL);
	if (ret != 0) {
		printk(KERN_INFO "DOE_KERNEL_TEST: create arraytbl(%d) failed!\n", data_len);
		goto error_create_arraytbl_76;
	}

	data_len = 128;
	ret = hados_doe_create_arraytbl(3, depth, data_len, NULL);
	if (ret != 0) {
		printk(KERN_INFO "DOE_KERNEL_TEST: create arraytbl(%d) failed!\n", data_len);
		goto error_create_arraytbl_128;
	}

	printk(KERN_INFO "DOE_KERNEL_TEST: Create array table success.\n");

	/* create hash table */
	key_len = 36;
	ret = hados_doe_create_hashtbl(4, depth, key_len, value_len, NULL);
	if (ret != 0) {
		printk(KERN_INFO "DOE_KERNEL_TEST: create hashtbl(%d-%d) failed!\n", key_len, value_len);
		goto error_create_hashtbl_36;
	}

	key_len = 60;
	ret = hados_doe_create_hashtbl(5, depth, key_len, value_len, NULL);
	if (ret != 0) {
		printk(KERN_INFO "DOE_KERNEL_TEST: create hashtbl(%d-%d) failed!\n", key_len, value_len);
		goto error_create_hashtbl_60;
	}

	key_len = 96;
	ret = hados_doe_create_hashtbl(6, depth, key_len, value_len, NULL);
	if (ret != 0) {
		printk(KERN_INFO "DOE_KERNEL_TEST: create hashtbl(%d-%d) failed!\n", key_len, value_len);
		goto error_create_hashtbl_96;
	}

	printk(KERN_INFO "DOE_KERNEL_TEST: Create hash table success.\n");

	/* array table command test */
	ret = hados_doe_array_store(0, 0, data, 16);
	if (ret != 0) {
		printk(KERN_INFO "DOE_KERNEL_TEST: Store to arraytbl(16) failed!\n");
		goto array_32_test;
	}

	ret = hados_doe_array_load(0, 0, data, 128);
	if (ret != 0)
		printk(KERN_INFO "DOE_KERNEL_TEST: Load from arraytbl(16) failed!\n");

	ret = hados_doe_array_read_clear(0, 0, data, 128);
	if (ret != 0)
		printk(KERN_INFO "DOE_KERNEL_TEST: Readclear arraytbl(16) failed!\n");

array_32_test:
	ret = hados_doe_array_store(1, 0, data, 32);
	if (ret != 0) {
		printk(KERN_INFO "DOE_KERNEL_TEST: Store to arraytbl(32) failed!\n");
		goto array_76_test;
	}

	ret = hados_doe_array_load(1, 0, data, 128);
	if (ret != 0)
		printk(KERN_INFO "DOE_KERNEL_TEST: Load from arraytbl(32) failed!\n");

	ret = hados_doe_array_read_clear(1, 0, data, 128);
	if (ret != 0)
		printk(KERN_INFO "DOE_KERNEL_TEST: Readclear arraytbl(32) failed!\n");

array_76_test:
	ret = hados_doe_array_store(2, 0, data, 76);
	if (ret != 0) {
		printk(KERN_INFO "DOE_KERNEL_TEST: Store to arraytbl(76) failed!\n");
		goto array_128_test;
	}

	ret = hados_doe_array_load(2, 0, data, 128);
	if (ret != 0)
		printk(KERN_INFO "DOE_KERNEL_TEST: Load from arraytbl(76) failed!\n");

	ret = hados_doe_array_read_clear(2, 0, data, 128);
	if (ret != 0)
		printk(KERN_INFO "DOE_KERNEL_TEST: Readclear arraytbl(76) failed!\n");

array_128_test:
	ret = hados_doe_array_store(3, 0, data, 128);
	if (ret != 0) {
		printk(KERN_INFO "DOE_KERNEL_TEST: Store to arraytbl(128) failed!\n");
		goto array_batch_test;
	}

	ret = hados_doe_array_load(3, 0, data, 128);
	if (ret != 0)
		printk(KERN_INFO "DOE_KERNEL_TEST: Load from arraytbl(128) failed!\n");

	ret = hados_doe_array_read_clear(3, 0, data, 128);
	if (ret != 0)
		printk(KERN_INFO "DOE_KERNEL_TEST: Readclear arraytbl(128) failed!\n");

array_batch_test:
	/* array batch operator */
	array_batch_data = kzalloc((16 + 4) * 3, GFP_KERNEL);
	for (i = 0; i < 3; ++i)
		*(array_batch_data + (16 + 4) * i) = i;
	if (hados_doe_array_store_batch(0, 3, array_batch_data) != 3) {
		printk(KERN_INFO "DOE_KERNEL_TEST: Batch store arraytbl(16) failed!\n");
		goto hash_36_test;
	}
	
	if (hados_doe_array_load_batch(0, 3, batch_key, array_batch_data) != 3)
		printk(KERN_INFO "DOE_KERNEL_TEST: Batch load arraytbl(16) failed!\n");

	if (hados_doe_array_readclear_batch(0, 3, batch_key, array_batch_data) != 3)
		printk(KERN_INFO "DOE_KERNEL_TEST: Batch readclear arraytbl(16) failed!\n");

	
hash_36_test:
	kfree(array_batch_data);

	/* hash operator test */
	ret = hados_doe_hash_query(4, key, 36, value, 128);
	if (ret == 0)
		printk(KERN_INFO "DOE_KERNEL_TEST: Query from hashtbl(36) success while no key insert!\n");
	
	ret = hados_doe_hash_insert(4, key, 36, value, 128);
	if (ret != 0) {
		printk(KERN_INFO "DOE_KERNEL_TEST: Insert to hashtbl(36) failed!\n");
		goto hash_60_test;
	}

	ret = hados_doe_hash_query(4, key, 36, value, 128);
	if (ret != 0)
		printk(KERN_INFO "DOE_KERNEL_TEST: Query from hashtbl(36) failed!\n");

	ret = hados_doe_hash_delete(4, key, 36);
	if (ret != 0)
		printk(KERN_INFO "DOE_KERNEL_TEST: Delete from hashtbl(36) failed!\n");

hash_60_test:
	ret = hados_doe_hash_insert(5, key, 60, value, 128);
	if (ret != 0) {
		printk(KERN_INFO "DOE_KERNEL_TEST: Insert to hashtbl(60) failed!\n");
		goto hash_96_test;
	}

	ret = hados_doe_hash_query(5, key, 60, value, 128);
	if (ret != 0)
		printk(KERN_INFO "DOE_KERNEL_TEST: Query from hashtbl(60) failed!\n");

	ret = hados_doe_hash_delete(5, key, 60);
	if (ret != 0)
		printk(KERN_INFO "DOE_KERNEL_TEST: Delete from hashtbl(60) failed!\n");

hash_96_test:
	ret = hados_doe_hash_insert(6, key, 96, value, 128);
	if (ret != 0) {
		printk(KERN_INFO "DOE_KERNEL_TEST: Insert to hashtbl(96) failed!\n");
		goto hash_batch_test;
	}

	ret = hados_doe_hash_query(6, key, 96, value, 128);
	if (ret != 0)
		printk(KERN_INFO "DOE_KERNEL_TEST: Query from hashtbl(96) failed!\n");

	ret = hados_doe_hash_delete(6, key, 96);
	if (ret != 0)
		printk(KERN_INFO "DOE_KERNEL_TEST: Delete from hashtbl(96) failed!\n");
	
	printk(KERN_INFO "DOE_KERNEL_TEST: Test all success!\n");

hash_batch_test:
	/* hash batch */
	hash_batch_value = kzalloc((36 + 128) * 3, GFP_KERNEL);
	for (i = 0; i < 3; ++i)
		*(hash_batch_value + (36 + 128) * i) = i + 1;

	hash_batch_key = kzalloc(36 * 3, GFP_KERNEL);
	for (i = 0; i < 3; ++i)
		*(hash_batch_key + 36 * i) = i + 1;

	ret = hados_doe_hash_insert_batch(4, 3, hash_batch_value);
	if (ret != 3) {
		printk(KERN_INFO "DOE_KERNEL_TEST: Batch insert hashtbl(36) failed!. ret=%d\n", ret);
		goto err_batch_hash;
	}

	if (hados_doe_hash_query_batch(4, 3, hash_batch_key, hash_batch_value) != 3)
		printk(KERN_INFO "DOE_KERNEL_TEST: Batch query hashtbl(36) failed!\n");

	if (hados_doe_hash_delete_batch(4, 3, hash_batch_key) != 3)
		printk(KERN_INFO "DOE_KERNEL_TEST: Batch delete hashtbl(36) failed!\n");

err_batch_hash:
	kfree(hash_batch_key);
	kfree(hash_batch_value);

	hados_doe_delete_hashtbl(6);
error_create_hashtbl_96:
	hados_doe_delete_hashtbl(5);
error_create_hashtbl_60:
	hados_doe_delete_hashtbl(4);
error_create_hashtbl_36:
	hados_doe_delete_arraytbl(3);
error_create_arraytbl_128:
	hados_doe_delete_arraytbl(2);
error_create_arraytbl_76:
	hados_doe_delete_arraytbl(1);
error_create_arraytbl_32:
	hados_doe_delete_arraytbl(0);
	printk(KERN_INFO "DOE_TEST: complete\n");

	return 0;
}

static void kernel_call_test_exit(void)
{
    printk(KERN_INFO " Hello World exit\n ");
}

module_init(kernel_call_test_init);
module_exit(kernel_call_test_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Xing Wang <wangx_ddpt@yusur.com>");
MODULE_DESCRIPTION("Kernel module for testing doe kernel interface.");
