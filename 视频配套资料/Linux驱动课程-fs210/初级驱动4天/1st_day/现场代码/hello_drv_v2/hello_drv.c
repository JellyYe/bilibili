#include <linux/init.h>
#include <linux/module.h>


#include "math.h"


static int number = 120;
static char *name = "Alibaba";


//extern int my_add(int a, int b);
//extern int my_sub(int a, int b);



static int __init hello_init(void)
{

	printk("-------^_^ %s-------\n", __FUNCTION__);

	printk("number = %d\n", number);
	printk("name = %s\n", name);


	printk("a+b = %d\n", my_add(30, 8));
	printk("a-b = %d\n", my_sub(38, 8));


	
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


