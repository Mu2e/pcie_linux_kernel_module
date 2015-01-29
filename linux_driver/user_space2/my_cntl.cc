 // This file (my_cntl.cc) was created by Ron Rechenmacher <ron@fnal.gov> on
 // Feb  6, 2014. "TERMS AND CONDITIONS" governing this file are in the README
 // or COPYING file. If you do not have such a file, one can be obtained by
 // contacting Ron or Fermi Lab in Batavia IL, 60510, phone: 630-840-3000.
 // $RCSfile: .emacs.gnu,v $
 // rev="$Revision: 1.23 $$Date: 2012/01/23 15:32:40 $";

#include <stdio.h>		// printf
#include <fcntl.h>		// open, O_RDONLY
#include <libgen.h>		// basename
#include <string.h>		// strcmp
#include <stdlib.h>		// strtoul

#include "mu2edev.hh"		// mu2edev
#include "mu2e_mmap_ioctl.h"	// m_ioc_cmd_t

#define USAGE "\
   usage: %s <start|stop>\n\
          %s read <offset>\n\
          %s write <offset> <val>\n\
examples: %s start\n\
", basename(argv[0]), basename(argv[0]), basename(argv[0]), basename(argv[0])




int
main(  int	argc
     , char	*argv[] )
{
    int                sts=0;
    int                fd;
    char              *cmd;
    m_ioc_cmd_t        ioc_cmd;
    m_ioc_reg_access_t reg_access; 
    mu2edev            dev;

    if (argc < 2) { printf(USAGE); return (1); }
    cmd = argv[1];

    fd = open( "/dev/" MU2E_DEV_FILE, O_RDONLY );
    if (fd == -1) { perror("open /dev/" MU2E_DEV_FILE); return (1); }

    if      (strcmp(cmd,"start") == 0)
    {
	sts = ioctl( fd, M_IOC_TEST_START, &ioc_cmd );
    }
    else if (strcmp(cmd,"stop") == 0)
    {
	sts = ioctl( fd, M_IOC_TEST_STOP, &ioc_cmd );
    }
    else if (strcmp(cmd,"read") == 0)
    {
	if (argc < 3) { printf(USAGE); return (1); }
	reg_access.reg_offset = strtoul(argv[2],NULL,0);
	reg_access.access_type = 0;
	sts = ioctl( fd, M_IOC_REG_ACCESS, &reg_access );
	if (sts) { perror("ioctl M_IOC_REG_ACCESS read"); return (1); }
	printf( "0x%08x\n", reg_access.val );
    }
    else if (strcmp(cmd,"write") == 0)
    {
	if (argc < 4) { printf(USAGE); return (1); }
	reg_access.reg_offset  = strtoul(argv[2],NULL,0);
	reg_access.access_type = 1;
	reg_access.val         = strtoul(argv[3],NULL,0);
	sts = ioctl( fd, M_IOC_REG_ACCESS, &reg_access );
	if (sts) { perror("ioctl M_IOC_REG_ACCESS write"); return (1); }
    }
    else if (strcmp(cmd,"dump") == 0)
    {
	sts = ioctl( fd, M_IOC_DUMP );
	if (sts) { perror("ioctl M_IOC_REG_ACCESS write"); return (1); }
    }
    else if (strcmp(cmd,"write_loopback_data") == 0)
    {
	sts = dev.init();
	if (sts) { perror("dev.init"); return (1); }
	reg_access.reg_offset  = 0x9108;
	reg_access.access_type = 1;
	reg_access.val         = 0x3f;
	sts = ioctl( fd, M_IOC_REG_ACCESS, &reg_access );
	if (sts) { perror("ioctl M_IOC_REG_ACCESS write"); return (1); }
	int chn=0;
	struct
	{   DataHeaderPacket hdr;
	    DataPacket       data[7];
	} data={{{0}}};
	data.hdr.s.TransferByteCount = 64;
	if (argc > 2) data.hdr.s.TransferByteCount = strtoul( argv[2],NULL,0 );
	data.hdr.s.Valid = 1;
	data.hdr.s.PacketType = 5;  // could be overwritten below
	data.hdr.s.RingID = 0;
	data.hdr.s.PacketCount = (data.hdr.s.TransferByteCount-16)/16;// minus header, / packet size
	//data.hdr.s.ts10 = 0x3210;
	data.hdr.s.ts32 = 0x7654;
	data.hdr.s.ts54 = 0xba98;
	data.hdr.s.data32 = 0xdead;
	data.hdr.s.data54 = 0xbeaf;
	unsigned bytes=16;
	for (unsigned ii=0; ii<7; ++ii)
	{
	    for (unsigned jj=0; jj<8; ++jj)
	    {
		if (bytes >= data.hdr.s.TransferByteCount) goto out;
		data.data[jj].data10 = (jj<<8)|1;
		data.data[jj].data32 = (jj<<8)|2;
		data.data[jj].data54 = (jj<<8)|3;
		data.data[jj].data76 = (jj<<8)|4;
		data.data[jj].data98 = (jj<<8)|5;
		data.data[jj].dataBA = (jj<<8)|6;
		data.data[jj].dataDC = (jj<<8)|7;
		data.data[jj].dataFE = (jj<<8)|8;
	    }
	}
 out:
	sts = dev.write_loopback_data( chn, &data,data.hdr.s.TransferByteCount );
    }
    else
    {
	printf("unknown cmd %s\n", cmd ); return (1);
    }

    printf( "sts=%d\n", sts );
    return (0);
}   // main