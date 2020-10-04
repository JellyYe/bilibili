#include <linux/init.h>
#include <linux/module.h>

#include <linux/err.h>
#include <asm/uaccess.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <asm/io.h>
#include <linux/slab.h>
#include <linux/gpio.h>
#include <mach/gpio.h>

#define DEV_NAME "buzzer"
#define GPIOD_PHY_BASE 0xE02000A0
#define GPIOD_SIZE 24

#define BUZZER_ON_C _IO('B',0x3347)
#define BUZZER_OFF_C _IO('B',0x3346)

volatile unsigned long *gpd0con = NULL;
volatile unsigned long *gpd0dat = NULL;

struct s5pv210_buzzer{
	int major;
	int minor;
	struct class *buzzer_class;
	struct device *buzzer_device ;
};

struct s5pv210_buzzer *buzzer_dev ;

static int buzzer_open(struct inode *inode, struct file *file)
{
	printk("%s[%s:%d]\n",__func__,__FILE__,__LINE__);

	*gpd0con &= ~(0xf<<4);
	*gpd0con |= (0x1<<4);

	*gpd0dat &= ~(0x1<<1);
	
	return 0;
}

static ssize_t buzzer_write(struct file *file,
				  const char __user *user_buf,
				  size_t count, loff_t *ppos)
{
	int ret = 0;
	int buzzer_state ; 
	printk("%s[%s:%d]\n",__func__,__FILE__,__LINE__);

	if(IS_ERR(user_buf)){
		ret = -EINVAL;
		goto out;
	}

	if(copy_from_user(&buzzer_state,user_buf,sizeof(int))){
		return -EFAULT;
		goto out;
	}

#define BUZZER_ON 1
#define BUZZER_OFF 0

	switch(buzzer_state){
		
	case BUZZER_ON:
		*gpd0dat |= (0x1<<1);
		break;
		
	case BUZZER_OFF:
		*gpd0dat &= ~(0x1<<1);
		break;
		
	default:
		ret = -EINVAL;
		break;
	}

	return count;
		
out:
	return ret;
}

static int buzzer_close(struct inode *inode, struct file *file)
{
	printk("%s[%s:%d]\n",__func__,__FILE__,__LINE__);

	*gpd0dat &= ~(0x1<<1); 
	
	return 0;
}

long buzzer_ioctl (struct file *inode, unsigned int cmd, unsigned long args)
{
	switch(cmd){
		case BUZZER_ON_C:
			gpio_request(S5PV210_GPD0(1),"gpd0_1_buzzer");
			gpio_direction_output(S5PV210_GPD0(1),1);
			gpio_free(S5PV210_GPD0(1));
			break;

		case BUZZER_OFF_C:
			gpio_request(S5PV210_GPD0(1),"gpd0_1_buzzer");
			gpio_direction_output(S5PV210_GPD0(1),0);
			gpio_free(S5PV210_GPD0(1));
			break;

		default:
			printk("%s[%s:%d]:unkown cmd\n",__func__,__FILE__,__LINE__);
			return -EINVAL;
	}

	return 0;
}


struct file_operations buzzer_ops = {
	.owner = THIS_MODULE,
	.open = buzzer_open,
	.write = buzzer_write,
	.release = buzzer_close,	
	.unlocked_ioctl = buzzer_ioctl,
};


static int __init buzzer_init(void)
{
	int ret = 0;

	buzzer_dev = kzalloc(sizeof(struct s5pv210_buzzer),GFP_KERNEL);
	if(buzzer_dev == NULL){
		printk(KERN_ERR"kzallo error");
		return -ENOMEM;
	}
	buzzer_dev->major = 0;
	ret = register_chrdev(buzzer_dev->major,DEV_NAME,&buzzer_ops);
	if(!ret){
		goto err_free;
	}

	if(!buzzer_dev->major){
		buzzer_dev->major = ret;
	}

	buzzer_dev->buzzer_class = class_create(THIS_MODULE,"buzzer_class");
	if(IS_ERR(buzzer_dev->buzzer_class)){
		ret = PTR_ERR(buzzer_dev->buzzer_class);
		goto err_unregister;
	}

	buzzer_dev->minor = 0;
	buzzer_dev->buzzer_device = device_create(buzzer_dev->buzzer_class,NULL,MKDEV(buzzer_dev->major,buzzer_dev->minor),NULL,DEV_NAME);
	if(IS_ERR(buzzer_dev->buzzer_device)){
			ret = PTR_ERR(buzzer_dev->buzzer_device);
			goto err_class_destory;
		}

	gpd0con = ioremap(GPIOD_PHY_BASE,GPIOD_SIZE);
	if(!gpd0con){
		ret = -ENOMEM;
		goto err_device_destroy;
	}

	gpd0dat = gpd0con + 1;
	
	printk("------^_^%s-------\n",__FUNCTION__);
	return 0;//Õý³£·µ»Ø

err_device_destroy:
	device_destroy(buzzer_dev->buzzer_class,MKDEV(buzzer_dev->major,buzzer_dev->minor));
err_class_destory:
	class_destroy(buzzer_dev->buzzer_class);
err_unregister:
	unregister_chrdev(buzzer_dev->major,DEV_NAME);
err_free:
	kfree(buzzer_dev);
	return ret;
}

static void __exit buzzer_exit(void)
{
	printk("------^_^%s-------\n",__FUNCTION__);

	kfree(buzzer_dev);
	
	iounmap(gpd0con);

	device_destroy(buzzer_dev->buzzer_class,MKDEV(buzzer_dev->major,buzzer_dev->minor));


	class_destroy(buzzer_dev->buzzer_class);

	unregister_chrdev(buzzer_dev->major,DEV_NAME);
	
	
}

module_init(buzzer_init);
module_exit(buzzer_exit);

MODULE_LICENSE("GPL");





