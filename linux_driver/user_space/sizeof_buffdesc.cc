 // This file (tt.cc) was created by Ron Rechenmacher <ron@fnal.gov> on
 // Feb  7, 2014. "TERMS AND CONDITIONS" governing this file are in the README
 // or COPYING file. If you do not have such a file, one can be obtained by
 // contacting Ron or Fermi Lab in Batavia IL, 60510, phone: 630-840-3000.
 // $RCSfile: .emacs.gnu,v $
 // rev="$Revision: 1.23 $$Date: 2012/01/23 15:32:40 $";

#include <stdio.h>		// printf
#include <stdint.h>

//typedef unsigned u64;
#include "../mymodule/xdma_hw.h"

int
main(  int	argc
     , char	*argv[] )
{
    printf("sizeof(mu2e_buffdesc_S2C_t)=%lu\n",sizeof(mu2e_buffdesc_S2C_t));
    return (0);
}   // main
