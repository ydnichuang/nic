#include <linux/module.h>
#include <linux/init.h>
#include <linux/io.h>

#define PCI_VENDOR_ID_DOE       0x1f47
#define PCI_DEVICE_ID_DOE       0x1006
#define RDMA_RQ_DEPTH			128
/// @cond
struct pair_item {
	int32_t index;
	char    data[64];
};
/// @endcond

typedef struct {
        dma_addr_t dma_addr;
        void *vaddr;
        int   size;
} cdma_t;

static unsigned long pa= 0x1000;

static cdma_t addr;

static int __init array_test_init(void)
{
	ioremap(pa, 0x40);
}
module_init(array_test_init);

static void __exit array_test_exit(void)
{

}
module_exit(array_test_exit);

module_param(usignal, int, 0664);
MODULE_LICENSE("GPL v2");
