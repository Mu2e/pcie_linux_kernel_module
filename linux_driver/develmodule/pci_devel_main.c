/*  This file (pci_devel_main.c) was created by Ron Rechenmacher <ron@fnal.gov> on
    Apr 23, 2014. "TERMS AND CONDITIONS" governing this file are in the README
    or COPYING file. If you do not have such a file, one can be obtained by
    contacting Ron or Fermi Lab in Batavia IL, 60510, phone: 630-840-3000.
    $RCSfile: .emacs.gnu,v $
    rev="$Revision: 1.23 $$Date: 2012/01/23 15:32:40 $";
    */
#include <linux/module.h>	// module_param, THIS_MODULE
#include <linux/init.h>		// module_init,_exit
#include <linux/kernel.h>	// KERN_INFO, printk
#include <linux/fs.h>		/* struct inode */
#include <linux/device.h>       /* class_create */
#include <linux/cdev.h>         /* cdev_add */
#include <linux/types.h>        /* dev_t */
#include <linux/pci.h>          /* struct pci_dev *pci_get_device */
#include <linux/delay.h>	/* msleep */
#include <asm/uaccess.h>	/* access_ok, copy_to_user */

#include "trace.h"	/* TRACE */

#include "pcidevl_ioctl.h"	/* PCIDEVL_DEV_FILE */

#define DRIVER_NAME      "devl_driver"
#define XILINX_VENDOR_ID 0x10ee
#define XILINX_DEVICE_ID 0x7042


/* GLOBALS */

struct pci_dev *pci_dev_sp=0;

typedef struct
{   unsigned long basePAddr;    /**< Base address of device memory */
    unsigned long baseLen;      /**< Length of device memory */
    void __iomem * baseVAddr;   /**< VA - mapped address */
} bar_info_t;

bar_info_t      pcie_bar_info={0};


//////////////////////////////////////////////////////////////////////////////


static int devl_pci_probe( struct pci_dev *pdev, const struct pci_device_id *ent )
{
    int pciRet;

    TRACE( 6, "devl_pci_probe pdev=%p", pdev );
    /* Initialize device before it is used by driver. Ask low-level
     * code to enable I/O and memory. Wake up the device if it was
     * suspended. Beware, this function can fail.
     */
    pciRet = pci_enable_device( pdev );
    if (pciRet < 0)
    {
        TRACE( 0, KERN_ERR "PCI device enable failed." );
        return (pciRet);
    }

    /*
     * Enable bus-mastering on device. Calls pcibios_set_master() to do
     * the needed architecture-specific settings.
     */
    pci_set_master( pdev );

    pciRet = pci_request_regions( pdev, DRIVER_NAME );
    if (pciRet < 0)
    {   TRACE( 0, KERN_ERR "Could not request PCI regions." );
        pci_disable_device( pdev );
        return (pciRet);
    }

    pciRet = pci_set_dma_mask( pdev, DMA_32BIT_MASK );
    if (pciRet < 0)
    {   TRACE( 0, KERN_ERR "pci_set_dma_mask failed." );
        goto out2;
    }

    pci_dev_sp = pdev;		/* GLOBAL */
    return (0);

 out2:
    TRACE( 0, "devl_pci_probe - out2" );
    pci_release_regions( pdev );
    pci_disable_device( pdev );
    return (1);         /* error */
}   // devl_pci_probe

static void devl_pci_remove(struct pci_dev *pdev)
{
    TRACE( 6, "devl_pci_remove start pdev=%p", pdev );
    pci_release_regions( pdev );
    TRACE( 6, "devl_pci_remove after release_regions, before disable_device" );
    pci_disable_device( pdev );
    TRACE( 6, "devl_pci_remove complete" );
}   // devl_remove


static struct pci_device_id xilinx_ids[] = {
    { XILINX_VENDOR_ID, XILINX_DEVICE_ID, PCI_ANY_ID,PCI_ANY_ID,0,0,0UL },
    { }     /* terminate list with empty entry */
};

static struct pci_driver devl_driver =
    {   .name     = DRIVER_NAME,
        .id_table = xilinx_ids,
        .probe    =             devl_pci_probe,
        .remove   = __devexit_p(devl_pci_remove)
    };

void devl_pci_down( void )
{
    if (pcie_bar_info.baseVAddr != 0)
    {   TRACE( 6, "devl_pci_down - iounmap( pcie_bar_info.baseVAddr )" );
	iounmap( pcie_bar_info.baseVAddr );
	pcie_bar_info.baseVAddr = 0;
    }
    if (pci_dev_sp == 0)
	TRACE( 6, "devl_pci_down - pci_unregister - device NOT registered" );
    else
    {   pci_dev_sp =  0;
	pci_unregister_driver( &devl_driver );
	TRACE( 6, "devl_pci_down - pci_unregister called" );
    }
}   // devl_pci_down



int devl_ioctl(  struct inode *inode, struct file *filp
	       , unsigned int cmd, unsigned long arg )
{
    int			sts=0;

    if(_IOC_TYPE(cmd) != DEVL_IOC_MAGIC) return -ENOTTY;

    /* Check read/write and corresponding argument */
    if(_IOC_DIR(cmd) & _IOC_READ)
        if(!access_ok(VERIFY_WRITE, (void *)arg, _IOC_SIZE(cmd)))
            return -EFAULT;
    if(_IOC_DIR(cmd) & _IOC_WRITE)
        if(!access_ok(VERIFY_READ, (void *)arg, _IOC_SIZE(cmd)))
            return -EFAULT;

    switch(cmd)
    {
    case IOC_HELLO:
	TRACE( 6, "devl_ioctl - hello" );
	break;
    case IOC_REGISTER:
	if (pci_dev_sp == 0)
	{   sts = pci_register_driver( &devl_driver );
	    TRACE( 6, "devl_ioctl - pci_register=%d", sts );
	}
	else
	    TRACE( 6, "devl_ioctl - already registered" );
	break;
    case IOC_UNREGISTER:
	devl_pci_down();
	break;
    case IOC_IOREMAP:
	if (pci_dev_sp == 0)
	{   TRACE( 6, "devl_ioctl - IOREMAP first need to register" );
	    sts = pci_register_driver( &devl_driver );
	    TRACE( 6, "devl_ioctl - pci_register=%d", sts );
	}
	if (sts == 0)
	{   u32 size;
	    if ((size=pci_resource_len(pci_dev_sp,0/*bar*/)) == 0)
	    {   TRACE( 1, KERN_ERR "BAR 0 not valid, aborting." );
		return (-1);
	    }
	    if (!(pci_resource_flags(pci_dev_sp,0/*bar*/) & IORESOURCE_MEM))
	    {   TRACE( 1, KERN_ERR "BAR 0 is of wrong type, aborting." );
		return (-1);
	    }
	    pcie_bar_info.basePAddr = pci_resource_start( pci_dev_sp, 0/*bar*/ );
	    pcie_bar_info.baseLen   = size;

	    pcie_bar_info.baseVAddr = ioremap( pcie_bar_info.basePAddr, size );
	    if (pcie_bar_info.baseVAddr == 0UL)
	    {   TRACE( 1, KERN_ERR "Cannot map BAR 0 space, invalidating." );
		return (-1);
	    }
	    TRACE( 6, "devl_ioctl - IOREMAP - pcie_bar_info.baseVAddr=%p len=%u"
		  , pcie_bar_info.baseVAddr, size );
	}
	break;
    case IOC_IOUNMAP:
	if (pcie_bar_info.baseVAddr == 0)
	    TRACE( 6, "nothing to unmap" );
	else
	{   TRACE( 6, "iounmap-ing %p", pcie_bar_info.baseVAddr );
	    iounmap( pcie_bar_info.baseVAddr );
	    pcie_bar_info.baseVAddr  = 0;
	}
	break;
    case IOC_UINT32:
	if (pcie_bar_info.baseVAddr == 0)
	{   sts =devl_ioctl( inode, filp, IOC_IOREMAP, 0 );
	    if (sts != 0) { return -1; }
	}
	{   u32 off, val;
	    ulong base=(ulong)pcie_bar_info.baseVAddr;
	    if (get_user(off, (u32 __user *)arg))
		return -EFAULT;
	    base+=off;
	    val = *(u32*)base;
	    TRACE( 7, "reading base(%p) + offset(%u/0x%x) = 0x%08x"
		  , pcie_bar_info.baseVAddr, off, off, val );
	    return put_user( val, (u32 __user *)arg );
	}
	break;
    case IOC_IOOP:
	if (pcie_bar_info.baseVAddr == 0)
	{   sts =devl_ioctl( inode, filp, IOC_IOREMAP, 0 );
	    if (sts != 0) { return -1; }
	}
	{   struct ioc_ioop ioop;
	    ulong addr=(ulong)pcie_bar_info.baseVAddr;
	    if (copy_from_user( &ioop, (void*)arg, sizeof(struct ioc_ioop) ))
	    {   TRACE( 0, "devl_ioctl IOC_IOOP: copy_from_user failed" );
		return -EFAULT;
	    }
	    addr += ioop.offset;
	    if (ioop.ops_mask & ioop_write) *(u32*)addr   = ioop.write_val;
	    if (ioop.ops_mask & ioop_read)  ioop.read_val = *(u32*)addr;
	    if (copy_to_user( (void*)arg, &ioop, sizeof(struct ioc_ioop) ))
	    {   TRACE( 0, "devl_ioctl IOC_IOOP: copy_to_user failed" );
		return -EFAULT;
	    }
	}
	break;
    default:
	TRACE( 0, "devl_ioctl: unknown cmd" );
	return (-1); // some error
    }
    return (sts);
}   // devl_ioctl


static struct file_operations devl_file_ops =
{   .owner=   THIS_MODULE,
    .llseek=  NULL,             /* lseek        */
    .read=    NULL,             /* read         */
    .write=   NULL,             /* write        */
    .readdir= NULL,             /* readdir      */
    .poll=    NULL,             /* poll         */
    .ioctl=   devl_ioctl,        /* ioctl        */
    .mmap=    NULL,             /* mmap         */
    NULL,                       /* open         */
    NULL,                       /* flush        */
    NULL,                       /* release (close?)*/
    NULL,                       /* fsync        */
    NULL,                       /* fasync       */
    NULL,                       /* check_media_change */
    NULL,                       /* revalidate   */
    NULL                        /* lock         */
};

dev_t         devl_dev_number;
struct class *devl_dev_class;
struct cdev   devl_cdev;

int devl_fs_up( void )
{
    int sts;
    sts = alloc_chrdev_region( &devl_dev_number, 0, 1, "devl_drv" );

    if(sts < 0)
    {   TRACE( 3, "dcm_init(): Failed to get device numbers" );
        return (sts);
    }
    
    devl_dev_class = class_create( THIS_MODULE, "devl_dev" );
    
    cdev_init( &devl_cdev, &devl_file_ops );

    devl_cdev.owner = THIS_MODULE;
    devl_cdev.ops   = &devl_file_ops;

    sts = cdev_add ( &devl_cdev, devl_dev_number, 1 );
    device_create( devl_dev_class, NULL, devl_dev_number, NULL, PCIDEVL_DEV_FILE );

    return (0);
}   // devl_fs_up


void devl_fs_down( void )
{
    device_destroy( devl_dev_class, devl_dev_number);
    cdev_del( &devl_cdev );
    class_destroy( devl_dev_class);
    unregister_chrdev_region( devl_dev_number, 1 );
} // devl_fs_down



void free_mem( void )
{
}   // free_mem


//////////////////////////////////////////////////////////////////////////////


static int __init init_devl(void)
{
    int             ret=0;          /* SUCCESS */

    TRACE( 2, "init_devl" );

    // fs interface, pci, memory, events(i.e polling)

    ret = devl_fs_up();
    if (ret) goto out;

    return (ret);

 out:
    TRACE( 0, "Error - freeing memory" );
    devl_fs_down();
    return (-1);
}   // init_devl


static void __exit exit_devl(void)
{
    TRACE( 1, "exit_devl() called");

    devl_pci_down();

    devl_fs_down();

}   // exit_devl


module_init(init_devl);
module_exit(exit_devl);

MODULE_AUTHOR("Ron Rechenmacher");
MODULE_DESCRIPTION("devl pcie driver");
MODULE_LICENSE("GPL"); /* Get rid of taint message by declaring code as GPL */