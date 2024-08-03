#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0xdc658e53, "module_layout" },
	{ 0xc4f0da12, "ktime_get_with_offset" },
	{ 0x14632f1f, "param_ops_int" },
	{ 0xfc898315, "hados_doe_hash_insert_batch" },
	{ 0xa7da1cca, "dma_free_attrs" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0x1e1e140e, "ns_to_timespec64" },
	{ 0x365acda7, "set_normalized_timespec64" },
	{ 0xe58c043f, "dma_alloc_attrs" },
	{ 0xb8677852, "hados_doe_hash_delete_batch" },
	{ 0x87a21cb3, "__ubsan_handle_out_of_bounds" },
	{ 0x2a8c91b9, "hados_doe_create_hashtbl" },
	{ 0xd0da656b, "__stack_chk_fail" },
	{ 0x92997ed8, "_printk" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0x849f07cd, "pci_get_device" },
	{ 0x7d517dd, "hados_doe_delete_hashtbl" },
};

MODULE_INFO(depends, "ys_doe");


MODULE_INFO(srcversion, "1E92CE28E1446F326BE73E0");
