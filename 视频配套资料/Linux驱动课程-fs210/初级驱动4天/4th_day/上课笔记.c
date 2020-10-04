��Ҫ����
1, ��·���õ�ʵ��
2, mmap��ʵ��
3,�ж��°벿��ʵ�ַ�ʽ
	a, tasklet��ʵ��
	b, �������е�ʵ��
--------------------------------



Ӧ�õ���:
	open()
-----------------------
vfs:
	SYSCALL_DEFINE3(open, const char __user *, filename, int, flags, int, mode)
	sys_open()--fs/open.c
	{
		do_sys_open(AT_FDCWD, filename, flags, mode);
			|
			
			//  1, ����struct file ��¼open�и���������Ϣ, ����һ��fd, ��fd��struct file����]
			fd = get_unused_fd_flags(flags);
			struct file *f = do_filp_open(dfd, tmp, &op, lookup);
			fd_install(fd, f);
			//  2,����cdev�Ĵ���--��do_filp_open
					|chrdev_open����cdev
					const struct file_operations def_chr_fops = {
						.open = chrdev_open,
								|
								struct cdev *new = NULL;
								//�����豸���ҵ�cdev��kobj
								kobj = kobj_lookup(cdev_map, inode->i_rdev, &idx);
								//ͨ��kobj�ҵ�cdev
								new = container_of(kobj, struct cdev, kobj);
								inode->i_cdev = p = new;
								//��cdev��fop����file��f_op
								filp->f_op = fops_get(p->ops);
								//������cdev��fop��open����
								filp->f_op->open(inode,filp);
								
						.llseek = noop_llseek,
					};
	}
	
	-----------------------------------------
	����:
		const struct file_operations key_fops = {
			.open = key_drv_open,
			.write = key_drv_write,
			.read = key_drv_read,
			.release = key_drv_close,
		};
		
	
	

Ӧ�ò�:
		write()   ioctl()
-----------------------------
vfs :
	SYSCALL_DEFINE3(write
	sys_write()--fs/read_write.c
	{
		file = fget_light(fd, &fput_needed);
		vfs_write(file, buf, count, &pos);
			|
			if (file->f_op->write)
				ret = file->f_op->write(file, buf, count, pos);
	
	}
	
	SYSCALL_DEFINE3(ioctl
	sys_ioctl()--fs/ioctl.c
	{
		filp = fget_light(fd, &fput_needed);
		do_vfs_ioctl(filp, fd, cmd, arg);
			|
			 vfs_ioctl(filp, cmd, arg);
				|
				 filp->f_op->unlocked_ioctl(filp, cmd, arg);
	}
	-------------------------------------------
		const struct file_operations key_fops = {
			.unlocked_ioctl = led_drv_ioctl,
		}
	
	
	
	
	
	
	
1, ��·���õ�ʵ��

	struct pollfd {
		int fd; //����ص�fd
		short events;//ϣ����ص��¼�:��,д,����
		short revents;//���ڲ�ѯ,��ǰfd�Ƿ����˶�,д,����
	};	
	
	//�������fd---��׼����,�͵�ǰkey����
	struct pollfd pfds[2];

	pfds[0].fd = 0;
	pfds[0].events = POLLIN ;

	pfds[1].fd = fd;
	pfds[1].events = POLLIN;



	while(1)
	{	
		ret = poll(pfds, 2,  -1);
		
	}
	
	--------------------------------------
	������ʵ��poll�ӿ�
		unsigned int key_drv_poll(struct file *filp, struct poll_table_struct *pts)
		{
			unsigned int mask = 0;
			
			//  1, ����ǰ�ĵȴ�����ͷע�ᵽvfs��
			//����1---struct file����
			//����2---�ȴ�����ͷ
			//����3---��ȴ����й����ı��
			poll_wait(filp, &key_dev->wq_head, pts);

			// 2, ��������ݷ���һ��pollin,û�����ݷ���һ��0
			if(key_dev->have_data)
			{
				mask |= POLLIN;

			}

			return mask;
		}
	
	
	
3, mmap��ʵ��
	1,���ļ�io��һ��
	2,���̿ռ���������ݽ����ıȽϸ�Ч�ķ�ʽ
	3, ���ں˿ռ�������ڴ�ӳ�䵽�û��ռ�,ֱ�ӿ��Բ�����ַ
	
	
	
	 #include <sys/mman.h>

		//����1--ָ��ӳ�䵽�û��ռ�ĵ�ַ,һ�㶼��NULL,��ϵͳ�Զ�����
		//����2--ӳ��ĳ���
		//����3--���ڴ�ķ���Ȩ��PROT_EXEC,PROT_READ,PROT_WRITE
		//����4--�Ƿ����������ӳ��---MAP_PRIVATE,MAP_SHARED(������)
		//����5---�򿪵��ļ�
		//����6--���ڴ�Ķ���ƫ������ʼӳ��
		//����ֵ--ӳ�䵽�û��ռ�ĵ�ַ
       void *mmap(void *addr, size_t length, int prot, int flags,int fd, off_t offset);
     
	 int munmap(void *addr, size_t length);
	 
	 
	 ---------------------------------------------------------------
	 ����ʵ��: ʵ��fops��mmap�ӿ�
		int (*mmap) (struct file *, struct vm_area_struct *);

		int key_drv_mmap(struct file *filp, struct vm_area_struct *vma)
		{

			unsigned long addr;

			// ֻҪ����io_remap_pfn_range���ܽ��������ڴ�ӳ�䵽�û��ռ�
			//vma���������û��ռ�ӳ�������, vma��vfs�㴫�ݹ���

			addr = virt_to_phys(key_dev->virt_mem);

			vma->vm_flags |= VM_IO;
			vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);
			
			//����1---��ʾ����
			//����2---ӳ�䵽�û��ռ����ʼλ��
			//����3--��ӳ��������ַ��ҳ��ַ
			//����4---ӳ��Ĵ�С
			//����5--ӳ���Ȩ��
			if (io_remap_pfn_range(vma, vma->vm_start, addr >> PAGE_SHIFT,
							PAGE_SIZE, vma->vm_page_prot)) {
				printk(KERN_ERR "%s: io_remap_pfn_range failed\n",
					__func__);
				return -EAGAIN;
			}

			return 0;

		}

	
	
�ж��°벿�ı��:
	1, ��ʼ��tasklet
		struct tasklet_struct
		{
			struct tasklet_struct *next;
			unsigned long state;
			atomic_t count;
			void (*func)(unsigned long);
			unsigned long data;
		};
	
	2, ���жϵ��ϰ벿��tasklet���뵽�ں��߳���
	
		tasklet_schedule(struct tasklet_struct * t)
		
	3, ����tasklet
		tasklet_kill(struct tasklet_struct * t)
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	

