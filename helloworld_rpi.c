// A simple loadable kernel module that prints information onto the Kernel Ring Buffer
#include <linux/module.h>

static int __init hello_init(void)
{
	pr_info("Hello world init\n");
	return 0;
}

static void __exit hello_exit(void)
{
	pr_info("Hello world exit\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Edgar Granados");
MODULE_DESCRIPTION("A simple kernel Module that prints Hello World onto the Kernel Ring Buffer");


