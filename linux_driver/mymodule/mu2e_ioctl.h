 // This file (mu2e_ioctl.h) was created by Ron Rechenmacher <ron@fnal.gov> on
 // Feb  5, 2014. "TERMS AND CONDITIONS" governing this file are in the README
 // or COPYING file. If you do not have such a file, one can be obtained by
 // contacting Ron or Fermi Lab in Batavia IL, 60510, phone: 630-840-3000.
 // $RCSfile: .emacs.gnu,v $
 // rev="$Revision: 1.23 $$Date: 2012/01/23 15:32:40 $";

#ifndef MU2E_IOCTL_H
#define MU2E_IOCTL_H

#ifdef __KERNEL__
# include <asm/ioctl.h>		// _IOWR
#else
# include <sys/ioctl.h>		// _IOWR
#endif


/*
 For _IO,_IOR,_IOW,_IORW ref. Documentation/ioctl/ioctl-number.txt
 _IO   - implementation has neither copy_from nor copy_to user (or equivalents)
 _IOR  - implementation has copy_to_user   (or equiv., at end)
 _IOW  - implementation has copy_from_user (or equiv., at beginnning)
 _IOWR - implementaions has both copy_from_user (at beginnning) and
         copy_to_user (at end) 
NOTE: for _IOR, _IOW: the size is only for the data at the address used in the
      ioctl call; NOT for the size at an address contained within the data
      pointed to by the address used in the ioctl call.  So, if a small
      structure is pointed to (to be copied in) which has an address of a
      large buffer, the only thing that these macros should consider is the
      pointer used (directly) in the ioctl call. 
 */
#define MU2E_IOC_MAGIC 'C'

#define M_IOC_GET_TST_STATE  _IOWR( MU2E_IOC_MAGIC, 1, m_ioc_cmd_t )
#define M_IOC_TEST_START     _IOW ( MU2E_IOC_MAGIC, 2, m_ioc_cmd_t )
#define M_IOC_TEST_STOP      _IOW ( MU2E_IOC_MAGIC, 3, m_ioc_cmd_t )
#define M_IOC_GET_PCI_STATE  _IOR ( MU2E_IOC_MAGIC, 4, m_ioc_pcistate_t )
#define M_IOC_GET_ENG_STATE  _IOWR( MU2E_IOC_MAGIC, 5, m_ioc_engstate_t )
#define M_IOC_GET_DMA_STATS  _IOWR( MU2E_IOC_MAGIC, 6, m_ioc_engstats_t )
#define M_IOC_GET_TRN_STATS  _IOWR( MU2E_IOC_MAGIC, 7, TRNStatsArray )
#define M_IOC_GET_SW_STATS   _IOWR( MU2E_IOC_MAGIC, 8, SWStatsArray )

#define M_IOC_REG_ACCESS     _IOWR( MU2E_IOC_MAGIC, 9, m_ioc_reg_access_t )
#define M_IOC_RECV           _IOWR( MU2E_IOC_MAGIC,10, SWStatsArray )
#define M_IOC_SEND           _IOWR( MU2E_IOC_MAGIC,11, SWStatsArray )
#define M_IOC_DUMP           _IO  ( MU2E_IOC_MAGIC,12 )

typedef struct
{   int reg_offset;
    int access_type;		// 0=read, 1=write
    unsigned val;
} m_ioc_reg_access_t;

/** Structure used in IOCTL to start/stop a test & to get current test state */
typedef struct
{   int      Engine;        /**< Engine Number */
    unsigned TestMode;      /**< Test Mode - Enable TX, Enable loopback */
    unsigned MinPktSize;    /**< Min packet size */
    unsigned MaxPktSize;    /**< Max packet size */
} m_ioc_cmd_t;

/** Structure used in IOCTL to get PCIe state from driver */
typedef struct
{   unsigned Version;       /**< Hardware design version info */
    int      LinkState;     /**< Link State - up or down */
    int      LinkSpeed;     /**< Link Speed */
    int      LinkWidth;     /**< Link Width */
    unsigned VendorId;  /**< Vendor ID */
    unsigned DeviceId;  /**< Device ID */
    int      IntMode;       /**< Legacy or MSI interrupts */
    int      MPS;           /**< Max Payload Size */
    int      MRRS;          /**< Max Read Request Size */
    int      InitFCCplD;    /**< Initial FC Credits for Completion Data */
    int      InitFCCplH;    /**< Initial FC Credits for Completion Header */
    int      InitFCNPD;     /**< Initial FC Credits for Non-Posted Data */
    int      InitFCNPH;     /**< Initial FC Credits for Non-Posted Data */
    int      InitFCPD;      /**< Initial FC Credits for Posted Data */
    int      InitFCPH;      /**< Initial FC Credits for Posted Data */
} m_ioc_pcistate_t;

typedef struct
{   int      Engine;        /**< Engine Number */
    int      BDs;           /**< Total Number of BDs */
    int      Buffers;       /**< Total Number of buffers */
    unsigned MinPktSize;    /**< Minimum packet size */
    unsigned MaxPktSize;    /**< Maximum packet size */
    int      BDerrs;        /**< Total BD errors */
    int      BDSerrs;       /**< Total BD short errors - only TX BDs */
    int      IntEnab;       /**< Interrupts enabled or not */
    unsigned TestMode;      /**< Current Test Mode */
} m_ioc_engstate_t;

typedef struct  /* MAYBE THIS SHOULD GO ELSEWHERE??? */
{   int      Engine;        /**< Engine Number */
    unsigned LBR;           /**< Last Byte Rate */
    unsigned LAT;           /**< Last Active Time */
    unsigned LWT;           /**< Last Wait Time */
} DMAStatistics;

typedef struct
{   int            Count;   /**< Number of statistics captures */
    DMAStatistics *engptr;  /**< Pointer to array to store statistics */
} m_ioc_engstats_t;

typedef struct {
    unsigned int LTX;           /**< Last TX Byte Rate */
    unsigned int LRX;           /**< Last RX Byte Rate */
} TRNStatistics;

/** Structure used in IOCTL to get PCIe TRN statistics from driver */
typedef struct {
    int Count;                  /**< Number of statistics captures */
    TRNStatistics * trnptr;     /**< Pointer to array to store statistics */
} TRNStatsArray;

/** Structure used to hold software statistics */
typedef struct {
    int Engine;                 /**< Engine Number */
    unsigned int LBR;           /**< Last Byte Rate */
} SWStatistics;

/** Structure used in IOCTL to get software statistics from driver */
typedef struct {
    int Count;                  /**< Number of statistics captures */
    SWStatistics * swptr;       /**< Pointer to array to store statistics */
} SWStatsArray;


#endif // MU2E_IOCTL_H
