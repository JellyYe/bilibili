




#include <linux/module.h>
#include <linux/init.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>


struct input_dev *btn_input;
static int irqno ;

irqreturn_t button_interrupt(int irq, void *dev_id)
{
		printk("--------%s-----------\n", __FUNCTION__);

		// 获取数据
		int value = gpio_get_value(S5PV210_GPH0(1));

		if(value)
		{
			//抬起
			//将数据上报给用户
			//参数1---哪个设备上报的
			//参数2---上报那种数据类型
			//参数3---上报码值
			//参数4--按键的状态/值
			//input_report_key(btn_input, KEY_DOWN, 0);
			input_event(btn_input, EV_KEY, KEY_DOWN,  0);
			//最后要有一个结束/同步
			input_sync(btn_input);
		
		}else{
			//按下
			input_event(btn_input, EV_KEY, KEY_DOWN,  1);
			input_sync(btn_input);

		}
		
       
        return IRQ_HANDLED;
}

static int __init simple_btn_input_init(void)
{
		int ret;

		/*
			a, 分配一个input device对象
			b, 初始化input device对象
			c, 注册input device对象
			d, 硬件初始化				
		*/

		//a, 分配一个input device对象
		btn_input = input_allocate_device();
		if(btn_input == NULL)
		{
			printk("input_allocate_device error\n");
			return -ENOMEM;
		}

		//b, 初始化input device对象
		//该设备能够产生哪种数据类型---EV_KEY表示产生按键数据
		btn_input->evbit[0] |= BIT_MASK(EV_KEY);
		//能够产生哪个按键---比如能够产生下键 KEY_DOWN, KEY_ESC
		// btn_input->keybit[108/32] |= 1<<(108%32);
		btn_input->keybit[BIT_WORD(KEY_DOWN)] |= BIT_MASK(KEY_DOWN);

	   //c, 注册input device对象
	   ret = input_register_device(btn_input);
	   if(ret != 0)
	   {
	   		printk("input_register_device error\n");
			
			goto err_free_input;
	   }

#if 1
		//d, 硬件初始化--申请中断
		irqno = IRQ_EINT(1);
		ret = request_irq(irqno, button_interrupt, IRQF_TRIGGER_FALLING|IRQF_TRIGGER_RISING,
						"key_input_eint1", NULL);
		if(ret != 0)
		{
			printk("request_irq error\n");
			
			goto err_unregister;
		}

#endif

		return 0;


err_unregister:
	input_unregister_device(btn_input);
err_free_input:
	input_free_device(btn_input);
	return ret;
}



static void __exit simple_btn_input_exit(void)
{
	  // free_irq(irqno, NULL);
	   input_unregister_device(btn_input);
       input_free_device(btn_input);
}

module_init(simple_btn_input_init);
module_exit(simple_btn_input_exit);
MODULE_LICENSE("GPL");




