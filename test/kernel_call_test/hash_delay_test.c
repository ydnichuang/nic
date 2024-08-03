#include <linux/cdev.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/pci.h>
#include <linux/spinlock.h>
#include <linux/timex.h>
#include <linux/time64.h>
#include "ys_doe_kapi.h"

#define PCI_VENDOR_ID_DOE       0x1f47
#define PCI_DEVICE_ID_DOE       0x1006
#define RDMA_MTT_DEPTH			128

/// @cond
struct pair_item {
        char    key[32];
        char    value[64];
}__attribute__((packed));

struct index_item {
        char key[32];
}__attribute__((packed));
/// @endcond


typedef struct {
        dma_addr_t dma_addr;
        void *vaddr;
        int   size;
} cdma_t;

static int usignal = 1;

static cdma_t addr;
static cdma_t k_addr;

static int __init array_test_init(void)
{
    struct pci_dev *dev = NULL;
	int i, j, success_count, ret;
	uint8_t tbl_id = 1;
	char buf[64];
	struct pair_item *pair_list = NULL;
    struct index_item *index_list = NULL;
    unsigned char c;
    struct timespec64 starttime,endtime;
    struct timespec64 timeuse;

	tbl_id = 2;
	// 获取doe设备
    dev = pci_get_device(PCI_VENDOR_ID_DOE, PCI_DEVICE_ID_DOE, NULL);
    if (!dev)
    {
        printk("Get DOE dev failed!\n");
        return -1;
    }

    addr.size = sizeof(struct pair_item) * RDMA_MTT_DEPTH;
    addr.vaddr = dma_alloc_coherent(&dev->dev, addr.size, &addr.dma_addr, GFP_KERNEL);
    if (addr.vaddr == NULL)
		addr.dma_addr = 0;
	else{ 
		memset(addr.vaddr, 0, addr.size);
        pair_list = addr.vaddr;
    }

    k_addr.size = sizeof(struct index_item) * RDMA_MTT_DEPTH;
    k_addr.vaddr = dma_alloc_coherent(&dev->dev, k_addr.size, &k_addr.dma_addr, GFP_KERNEL);
    if (k_addr.vaddr == NULL)
		k_addr.dma_addr = 0;
	else{ 
		memset(k_addr.vaddr, 0, k_addr.size);
        index_list = k_addr.vaddr;
    }

	// 创建hash表，表深度为128, key长度为32，value长度为64
	ret = hados_doe_create_hashtbl(tbl_id, RDMA_MTT_DEPTH, 32, 64, NULL);
	if (ret != 0) {
		printk("call hados_doe_create_hashtbl failed. errcode:%d\n", ret);
		return -1;
	}

	do {
        if(usignal){
            for (i = 0, c = 65; i < 64; i++) {
			    buf[i] = c;
			    c++;
			    if (c > 122)
				    c = 65;
		    }

            // 批量插入
		    for (i = 0; i < RDMA_MTT_DEPTH; ++i) {
			    memcpy(pair_list[i].key, "helloworld", sizeof("helloworld"));
			    // 这里只是为了测试在hello后面加了1到9作为key值区分
			    pair_list[i].key[strlen("helloworld")] = i + 1;
			    memcpy(pair_list[i].value, buf, sizeof(buf));
		    }

            // 批量删除
            for ( i = 0; i < RDMA_MTT_DEPTH; ++i) {
                memcpy(index_list[i].key, "helloworld", sizeof("helloworld"));
                index_list[i].key[strlen("helloworld")] = i + 1;
            }

		    // 在索引1位置插入数据
            for ( j = 0; j < 100; j++)
            {
                ktime_get_boottime_ts64(&starttime);
                success_count = hados_doe_hash_insert_batch(tbl_id, RDMA_MTT_DEPTH, pair_list);
		        if (success_count != RDMA_MTT_DEPTH) {
			        printk("call hados_doe_hash_insert_batch failed. succ: %d\n", success_count);
			        return 0;
		        }
                ktime_get_boottime_ts64(&endtime);
                timeuse = timespec64_sub(endtime, starttime);
		printk("The first %d tests,hash table batch insert 64 bytes value average latency is timeuse = %lld ns\n",j,(timespec64_to_ns(&timeuse)/RDMA_MTT_DEPTH));

                if(j != 99){
                    ktime_get_boottime_ts64(&starttime);
                    success_count = hados_doe_hash_delete_batch(tbl_id, RDMA_MTT_DEPTH, index_list);
                    if (success_count != RDMA_MTT_DEPTH) {
                        printk("call hados_doe_hash_delete_batch failed. succ: %d\n",
                                success_count);
                        return 0;
                    }
                    ktime_get_boottime_ts64(&endtime);
                    timeuse = timespec64_sub(endtime, starttime);

                    printk("----The first %d tests,hash table batch delete 64 bytes value average latency is timeuse = %lld ns\n",j,(timespec64_to_ns(&timeuse)/RDMA_MTT_DEPTH));
                }
            }
		}

	} while (0);

	return 0;
}
module_init(array_test_init);

static void __exit array_test_exit(void)
{
    struct pci_dev *dev = NULL;
    uint8_t tbl_id = 2;
    int ret;

    dev = pci_get_device(PCI_VENDOR_ID_DOE, PCI_DEVICE_ID_DOE, NULL);
    if (!dev)
    {
        printk("Get DOE dev failed!\n");
        return;
    }

    if (k_addr.vaddr)
    {
       dma_free_coherent(&dev->dev, k_addr.size, k_addr.vaddr, k_addr.dma_addr);
       k_addr.vaddr = NULL;
    }

    if (addr.vaddr)
    {
       dma_free_coherent(&dev->dev, addr.size, addr.vaddr, addr.dma_addr);
       addr.vaddr = NULL;
    }    

	// 删除表
	ret = hados_doe_delete_hashtbl(tbl_id);
	if (ret != 0) {
		printk("call hados_doe_delete_hashtbl failed. errcode:%d\n", ret);
		return;
	}
}
module_exit(array_test_exit);

module_param(usignal, int, 0664);
MODULE_LICENSE("GPL v2");
