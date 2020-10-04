#include <linux/init.h>
#include <linux/module.h>


static int number = 120;
static char *name = "Alibaba";

static int __init hello_init(void)
{

	printk("-------^_^ %s-------\n", __FUNCTION__);

	printk("number = %d\n", number);
	printk("name = %s\n", name);
	return 0;
}


static void __exit hello_exit(void)
{

	printk("-------^_^ %s-------\n", __FUNCTION__);

	

}


module_init(hello_init);
module_exit(hello_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("wukong@qq.com");
MODULE_DESCRIPTION("This is a simplest hello driver");


module_param(number, int, 0644);
module_param(name, charp, 0644);


