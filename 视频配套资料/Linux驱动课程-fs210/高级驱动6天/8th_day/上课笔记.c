��Ҫ����---Framebuffer��ϵͳ--LCD������
1,Framebuffer��ϵͳ�Ŀ��
2,LCD����������ֲ
3, ����LOGO������
4, Ӧ�ó������LCD���ķ���
5, Framebuffer��ϵͳ����ִ�е�����
-------------------------------------------
����֪ʶ��:�趨�Զ���ƽ̨���ݵķ�ʽ
1,��pev�ĳ�ʼ����ʱ�����һ��
	struct platform_device smdkv210_dm9000 = {
		.name		= "dm9000",
		.id		= -1,
		.num_resources	= ARRAY_SIZE(smdkv210_dm9000_resources),
		.resource	= smdkv210_dm9000_resources,
		.dev		= {
			.platform_data	= &smdkv210_dm9000_platdata,
		},
	};

2,ͨ���ӿ���ʱ�޸��Զ�������
	void __init s3c_fb_set_platdata(struct s3c_fb_platdata *pd)// pd����ƽ̨�Զ�������
	{
		struct s3c_fb_platdata *npd;

		if (!pd) {
			printk(KERN_ERR "%s: no platform data\n", __func__);
			return;
		}

		npd = kmemdup(pd, sizeof(struct s3c_fb_platdata), GFP_KERNEL);
		if (!npd)
			printk(KERN_ERR "%s: no memory for platform data\n", __func__);

		s3c_device_fb.dev.platform_data = npd;
	}




1,Framebuffer��ϵͳ�Ŀ��

	Ӧ�ò�
	------------------------------------------------	
	fbͨ�ò�:֪��ӳ��,���ǲ�֪����η����Դ�
		1, ������û�����
		2, ʵ���Դ��ӳ��
	drivers/video/fbmem.c
	
	-------------------------------------------------
	drivers/video/s3c-fb.c
	lcd��������:֪�������Դ�,���ǲ�֪�����ӳ��
		1,��ʼ��lcd������
		2, ʵ���Դ�ķ���
		
		

		
		
2, LCD����������ֲ--ʱ��--Documentation/fb/framebuffer.txt

			 soc                lcd��                    			����
��ֱ����	
			VSPW				tvpw   	   1<y<20 y=10			vsync_len
			VBPD                tvb-tvpw    23-tvpw  13			upper_margin
			LINVAL              tvd			480					yres
			VFPD				tvfp		22					lower_margin

ˮƽ����
			HSPW				thpw		1<x<40  x=20		hsync_len
			HBPD				thb-thpw	46-thpw  26			left_margin
			HOZVAL				thd			800					xres
			HFPD				thfp		210					right_margin
		

	pdev:
		struct platform_device s3c_device_fb = {
			.name		  = "s3c-fb",
			.id		  = -1,
			.num_resources	  = ARRAY_SIZE(s3c_fb_resource),
			.resource	  = s3c_fb_resource,
			.dev.dma_mask	  = &s3c_device_fb.dev.coherent_dma_mask,
			.dev.coherent_dma_mask = 0xffffffffUL,
		};
		
	smdkv210_machine_init(void)
		|
		s3c_fb_set_platdata(&smdkv210_lcd0_pdata);// smdkv210_lcd0_pdata�����Զ�������
			
		
	1,��ֲ��ʱ����Ҫ���趨�Զ�������
	arch/arm/mach-s5pv210/mach-smdkv210.c
	���ݲ����޸�����
		static struct s3c_fb_pd_win smdkv210_fb_win0 = {
			.win_mode = {
				.left_margin	= 26,
				.right_margin	= 210,
				.upper_margin	= 13,
				.lower_margin	= 22,
				.hsync_len	= 20,
				.vsync_len	= 10,
				.xres		= 800,
				.yres		= 480,
			},
			.max_bpp	= 32,
			.default_bpp	= 16, // ��Ӧ���й�
		};

		static struct s3c_fb_platdata smdkv210_lcd0_pdata __initdata = {
			.win[0]		= &smdkv210_fb_win0,
			.vidcon0	= VIDCON0_VIDOUT_RGB | VIDCON0_PNRMODE_RGB,
			.vidcon1	= VIDCON1_INV_HSYNC | VIDCON1_INV_VSYNC,
			.setup_gpio	= s5pv210_fb_gpio_setup_24bpp,
		};


	2, ȷ��fbͨ�ò��lcd��������������Ѿ�����:
		 make menuconfig
			Device Drivers  --->
				Graphics support  ---> 
					 <*> Support for frame buffer devices  --->// fbmem.c
							<*>   Samsung S3C framebuffer support  // s3c-fb.c
					
					 Console display driver support  --->
						<*> Framebuffer Console support //��ͼƬ���ص��Դ���ȥ��ʾ
					
					[*] Bootup logo  --->
						[*]   Standard black and white Linux logo (NEW)
						[*]   Standard 16-color Linux logo (NEW) 
						[*]   Standard 224-color Linux logo (NEW)//��ʾ����һ��ͼƬ
						
	
	3, ��������:
		 smdkv210_machine_init(void)
			|
			gpio_request(S5PV210_GPD0(0), "gpd0");
			gpio_direction_output(S5PV210_GPD0(0),1);
			gpio_free(S5PV210_GPD0(0));
	
			make zImage -j2
		�����ں�:
			cp -raf arch/arm/boot/zImage /tftpboot
		


3, ����LOGO������---Ϊ�ں����һ��ͼƬ,��������ʱ��ȥָ��
	  jpg/bmp------ת����ppm ----> ���뵽�ں�---ָ����ʾ���logo
	  
	  
	 
			
	�����һ���ļ�������ں�:
		
	  Kconfig
		
			576 config I2C_S3C2410  //  CONFIG_I2C_S3C2410
			577         tristate "S3C2410 I2C Driver"
			578         depends on HAVE_S3C2410_I2C
			579         help
			580           Say Y here to include support for I2C controller in the
			581           Samsung SoCs.

	  
	  Makefile
			obj-$(CONFIG_I2C_S3C2410)       += i2c-s3c2410.o
	  
	  
	  1,����ppm�ļ�
	   drivers/video/logo
			logo_girl_clut224.ppm
			
			
	  2, �޸�Kconfig��Makefile
				
			drivers/video/logo/Kconfig
					 31 config LOGO_GIRL_CLUT224
					 32         bool "Linux Girl for test logo"
					 33         help
					 34           you can you choose a girl
			
			drivers/video/logo/Makefile
					19 obj-$(CONFIG_LOGO_GIRL_CLUT224)  += logo_girl_clut224.o
		
			make  menuconfig
				Device Drivers  --->
					Graphics support  --->
						[*] Bootup logo  --->
							[*]   Linux Girl for test logo 
							
			
			make zImage -j2
			���һ������ɹ�:logo_girl_clut224.c
				const struct linux_logo logo_girl_clut224 __initconst = {
					.type		= LINUX_LOGO_CLUT224,
					.width		= 800,
					.height		= 480,
					.clutsize	= 224,
					.clut		= logo_girl_clut224_clut,
					.data		= logo_girl_clut224_data
				};
		3, ָ����ʾ���logo
		
			vim include/linux/linux_logo.h
				 51 extern const struct linux_logo logo_girl_clut224;
		
			vim drivers/video/logo/logo.c
				  65         if (depth >= 8) {
				 66 #ifdef CONFIG_LOGO_LINUX_CLUT224
				 67                 /* Generic Linux logo */
				 68                 logo = &logo_linux_clut224;
				 69 #endif
				
				 //--------------->
				 72 #ifdef CONFIG_LOGO_GIRL_CLUT224
				 73                 logo = &logo_girl_clut224;
				 74 #endif
				 75
			
			make zImage -j2
		
			�����ں�:
				cp -raf arch/arm/boot/zImage  /tftpboot/
				
				
---------------------------------------------------------			
4, Ӧ�ó������LCD���ķ���
5, Framebuffer��ϵͳ����ִ�е�����


fbmem.c
static const struct file_operations fb_fops = {
	.owner =	THIS_MODULE,
	.read =		fb_read,
	.write =	fb_write,
	.unlocked_ioctl = fb_ioctl,
#ifdef CONFIG_COMPAT
	.compat_ioctl = fb_compat_ioctl,
#endif
	.mmap =		fb_mmap,
	.open =		fb_open,
	.release =	fb_release,
#ifdef HAVE_ARCH_FB_UNMAPPED_AREA
	.get_unmapped_area = get_fb_unmapped_area,
#endif
#ifdef CONFIG_FB_DEFERRED_IO
	.fsync =	fb_deferred_io_fsync,
#endif
	.llseek =	default_llseek,
};
	module_init(fbmem_init);
		|
		proc_create("fb", 0, NULL, &fb_proc_fops);//��/procĿ¼�´�����һ��fb�ļ�
		// ע�����豸��
		register_chrdev(FB_MAJOR,"fb",&fb_fops)
		//������
		fb_class = class_create(THIS_MODULE, "graphics");

-------------------------------------------------------------
s3c-fb.c
// fb_info
struct fb_info {
	struct fb_var_screeninfo var;	//�ɱ����--��Ҫ��ʱ��,bpp
	struct fb_fix_screeninfo fix;	// �̶�����---�Դ����Ϣ--һ������֮��Ͳ���ı�
}

struct fb_fix_screeninfo {
	unsigned long smem_start;	/* Start of frame buffer mem */
					/* (physical address) */
	__u32 smem_len;			/* Length of frame buffer mem */
}

struct fb_var_screeninfo {//�ɱ����--��Ҫ��ʱ��,bpp,����û��ṩ�ӿ�
	__u32 xres;			/* visible resolution		*/
	__u32 yres;
	__u32 bits_per_pixel;		/* guess what			*/
	
	__u32 pixclock;			/* pixel clock in ps (pico seconds) */
	__u32 left_margin;		/* time from sync to picture	*/
	__u32 right_margin;		/* time from picture to sync	*/
	__u32 upper_margin;		/* time from sync to picture	*/
	__u32 lower_margin;
	__u32 hsync_len;		/* length of horizontal sync	*/
	__u32 vsync_len;		/* length of vertical sync	*/
}

pdrv
	platform_driver_register(&s3c_fb_driver);
		|
		s3c_fb_probe(struct platform_device *pdev)
			|
			//  ��ȡ�Զ�������
			pd = pdev->dev.platform_data;
			//����һ��ȫ�ֵ��豸����
			sfb = kzalloc(sizeof(struct s3c_fb), GFP_KERNEL);
			// ʱ��ʹ��
			sfb->bus_clk = clk_get(dev, "lcd");
			clk_enable(sfb->bus_clk);
			//��ַӳ��
			sfb->regs = ioremap(res->start, resource_size(res));
			//�ж�����
			sfb->irq_no = res->start;
			ret = request_irq(sfb->irq_no, s3c_fb_irq,
					  0, "s3c_fb", sfb);
					  
			
			//��ʼ��gpio��
			pd->setup_gpio();

			//��ƽ��ת
			writel(pd->vidcon1, sfb->regs + VIDCON1);
			
			
			//���ڲ���,���dma�Դ����, fb_info��ʼ���Լ�ע��
			s3c_fb_probe_win(sfb, win, fbdrv->win[win],&sfb->windows[win]);
				|
				struct fb_info *fbinfo;
				fbinfo = framebuffer_alloc(sizeof(struct s3c_fb_win) + palette_size * sizeof(u32), sfb->dev);
				
				// ��ʼ��
				//�����Դ�
				s3c_fb_alloc_memory(sfb, win);
					|
					fbi->fix.smem_len = size; //  xres  * yres *max_bpp /8
					// dma_alloc_writecombine�����Դ�������ַ
					// map_dma���Դ�������ַ
					fbi->screen_base = dma_alloc_writecombine(sfb->dev, size,
						  &map_dma, GFP_KERNEL);
					
					fbi->fix.smem_start = map_dma; //��¼���̶�����
				
				
				fb_videomode_to_var(&fbinfo->var, initmode);
							|//��ƽ̨�Զ��������е�ʱ��ֵ���浽var��
							var->xres = mode->xres;
							var->yres = mode->yres;
							var->xres_virtual = mode->xres;
							var->yres_virtual = mode->yres;
							var->xoffset = 0;
							var->yoffset = 0;
							var->pixclock = mode->pixclock;
							var->left_margin = mode->left_margin;
							var->right_margin = mode->right_margin;
							var->upper_margin = mode->upper_margin;
							var->lower_margin = mode->lower_margin;
							var->hsync_len = mode->hsync_len;
							var->vsync_len = mode->vsync_len;
							var->sync = mode->sync;
							var->vmode = mode->vmode & FB_VMODE_MASK;

				fbinfo->fix.type	= FB_TYPE_PACKED_PIXELS;
				fbinfo->fix.accel	= FB_ACCEL_NONE;
				fbinfo->var.activate	= FB_ACTIVATE_NOW;
				fbinfo->var.vmode	= FB_VMODE_NONINTERLACED;
				fbinfo->var.bits_per_pixel = windata->default_bpp;
				fbinfo->fbops		= &s3c_fb_ops;
				fbinfo->flags		= FBINFO_FLAG_DEFAULT;
				fbinfo->pseudo_palette  = &win->pseudo_palette;
				
				
				//��ʱ���ֵ��ֵ��lcd��������ȥ,��dma�Դ�ĵ�ַд�뵽lcd��������
				s3c_fb_set_par(fbinfo);
					|
					data = VIDTCON0_VBPD(var->upper_margin - 1) |
						   VIDTCON0_VFPD(var->lower_margin - 1) |
						   VIDTCON0_VSPW(var->vsync_len - 1);

					writel(data, regs + sfb->variant.vidtcon);

					data = VIDTCON1_HBPD(var->left_margin - 1) |
						   VIDTCON1_HFPD(var->right_margin - 1) |
						   VIDTCON1_HSPW(var->hsync_len - 1);

					/* VIDTCON1 */
					writel(data, regs + sfb->variant.vidtcon + 4);

					data = VIDTCON2_LINEVAL(var->yres - 1) |
						   VIDTCON2_HOZVAL(var->xres - 1);
					writel(data, regs + sfb->variant.vidtcon + 8);
					
					//���Դ����ʼλ�úͽ���λ��д�뵽lcd��������
					writel(info->fix.smem_start, buf + sfb->variant.buf_start);

					data = info->fix.smem_start + info->fix.line_length * var->yres;
					writel(data, buf + sfb->variant.buf_end);

				
				// ע��
				register_framebuffer(fbinfo);
					|
					 do_register_framebuffer(fb_info);
						|
						for (i = 0 ; i < FB_MAX; i++)
							if (!registered_fb[i])
								break;
						fb_info->dev = device_create(fb_class, fb_info->device,
								MKDEV(FB_MAJOR, i), NULL, "fb%d", i);
							
						registered_fb[i] = fb_info; //����ǰע���fb_info����������



										
mmap()
	|
	xx_mmap()
		|
		struct fb_info *info = file_fb_info(file); //�������еõ� fb_info
		unsigned long off;
		unsigned long start;
		// �Դ����ʼλ��
		start = info->fix.smem_start;
		//����ƫ����Ϊ0
		off += start;
		
		io_remap_pfn_range(vma, vma->vm_start, off >> PAGE_SHIFT,
			     vma->vm_end - vma->vm_start, vma->vm_page_prot)	

------------------------------------------------------------
4, Ӧ�ó������LCD���ķ���


	1, ���豸
		 int fd = open("/dev/fb0", O_RDWR);


	2, ��ȡ��lcd������Ϣxres, yres, bpp
		struct fb_var_screeninfo var;
		
		ioctl(fd, FBIOGET_VSCREENINFO, &var);
							|
							.unlocked_ioctl = fb_ioctl,
									|
									struct fb_info *info = file_fb_info(file);//
												|//��registered_fb�����еõ�fb_info
												struct fb_info *info = registered_fb[fbidx];
									do_fb_ioctl(info, cmd, arg);
										|
										void __user *argp = (void __user *)arg;
										switch (cmd) {
											case FBIOGET_VSCREENINFO:
												var = info->var;
												copy_to_user(argp, &var, sizeof(var)) ? -EFAULT : 0;
												
										
		size_t length = var.xres * var.yres * var.bits_per_pixel / 8							
	3,ӳ���Դ浽�û��ռ�
		
	
		 char *addr = mmap(NULL, size_t length, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

		framebuffer_ptr =(char *)mmap(  NULL,//�����ֵΪNULL,���ʾ���ں����Զ��������һ������ռ�
							     screensize,		//�ռ��С
							     PROT_READ|PROT_WRITE,//Ȩ��
							     MAP_SHARED,		//�Ƿ���Թ���
							     framebuffer_fd,		//�ļ�������
							     0);			//���ĸ��ط���ʼ
	4, �õ�ͼƬ����,������д�뵽addr
		/*1.��һ��ͼƬ*/
		pic_fd =open(argv[2],O_RDWR);
		printf("pic_fd=%d\n",pic_fd);

		/*2.��ȡͼƬ��С*/
		len =lseek(pic_fd, 0, SEEK_END);
		printf("len =%ld\n",len);

		buffer =(char *)malloc(len);

		lseek(pic_fd, 0, SEEK_SET);
		/*3.��ȡͼƬ����*/
		read(pic_fd,buffer,len); 
		
		// buffer ����bmp��ͼƬ����, framebuffer_ptrӳ��֮����Դ�λ��
		draw_bmp(buffer,(unsigned short *) framebuffer_ptr);
			|
			 bmp_read_file(bmpfilename, &bmpdata);//����bmp�ļ�ͷ
			 bmp2fb16_rgb565(bmpdata, fb);
				|//bmp2fb16_rgb565(unsigned char *bmpdata,unsigned short *fb16)
				 fb16_buff = fb16; //  fb16_buff����framebuffer_ptr,ӳ��֮����Դ�λ��
				for (y = ysize; y > 0 ; y--)
				{
					for (x = 0 ; x < xsize; x++) /*copy one line to frame buffer*/
					{	
						/*copy one pixel to frame buffer*/
						b = *bmpdata;
						bmpdata++;
						b >>= 3;
						g = *bmpdata;
						bmpdata++;
						g >>= 2;
						r = *bmpdata;
						bmpdata++;
						r >>= 3;
						pixel16 = (unsigned short) ((r << 11) | (g << 5) | b);	
						*( fb16_buff +  (y* xsize + x)  ) = pixel16;//��һ��������д���Դ�
					}
				}
				









