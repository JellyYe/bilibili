




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

		// ��ȡ����
		int value = gpio_get_value(S5PV210_GPH0(1));

		if(value)
		{
			//̧��
			//�������ϱ����û�
			//����1---�ĸ��豸�ϱ���
			//����2---�ϱ�������������
			//����3---�ϱ���ֵ
			//����4--������״̬/ֵ
			//input_report_key(btn_input, KEY_DOWN, 0);
			input_event(btn_input, EV_KEY, KEY_DOWN,  0);
			//���Ҫ��һ������/ͬ��
			input_sync(btn_input);
		
		}else{
			//����
			input_event(btn_input, EV_KEY, KEY_DOWN,  1);
			input_sync(btn_input);

		}
		
       
        return IRQ_HANDLED;
}

static int __init simple_btn_input_init(void)
{
		int ret;

		/*
			a, ����һ��input device����
			b, ��ʼ��input device����
			c, ע��input device����
			d, Ӳ����ʼ��				
		*/

		//a, ����һ��input device����
		btn_input = input_allocate_device();
		if(btn_input == NULL)
		{
			printk("input_allocate_device error\n");
			return -ENOMEM;
		}

		//b, ��ʼ��input device����
		//���豸�ܹ�����������������---EV_KEY��ʾ������������
		btn_input->evbit[0] |= BIT_MASK(EV_KEY);
		//�ܹ������ĸ�����---�����ܹ������¼� KEY_DOWN, KEY_ESC
		// btn_input->keybit[108/32] |= 1<<(108%32);
		btn_input->keybit[BIT_WORD(KEY_DOWN)] |= BIT_MASK(KEY_DOWN);

	   //c, ע��input device����
	   ret = input_register_device(btn_input);
	   if(ret != 0)
	   {
	   		printk("input_register_device error\n");
			
			goto err_free_input;
	   }

#if 1
		//d, Ӳ����ʼ��--�����ж�
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




