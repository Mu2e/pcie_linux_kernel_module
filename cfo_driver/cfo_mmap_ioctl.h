// This file (cfo_ioctl.h) was created by Ron Rechenmacher <ron@fnal.gov> on
// Feb  5, 2014. "TERMS AND CONDITIONS" governing this file are in the README
// or COPYING file. If you do not have such a file, one can be obtained by
// contacting Ron or Fermi Lab in Batavia IL, 60510, phone: 630-840-3000.
// $RCSfile: .emacs.gnu,v $
// rev="$Revision: 1.23 $$Date: 2012/01/23 15:32:40 $";

#ifndef CFO_MMAP_IOCTL_H
#define CFO_MMAP_IOCTL_H

#ifdef __KERNEL__
# include <asm/ioctl.h>		// _IOWR
# include "trace.h"
#else
# include <sys/ioctl.h>		// _IOWR
# include <unistd.h>		// sysconf
# include "trace.h"
# include <stdint.h>		// uint16_t
# include <sys/types.h>
#endif


#define CFO_DEV_FILE       "mu2ecfo"
#define CFO_MAX_CHANNELS	2

/// <summary>
/// Data Header Packet definition, hardware view
/// </summary>
union DataHeaderPacket
{
	/// <summary>
	/// Raw packet
	/// </summary>
	struct
	{
		uint16_t w0; ///< Word 0
		uint16_t w1; ///< Word 1
		uint16_t w2; ///< Word 2
		uint16_t w3; ///< Word 3
		uint16_t w4; ///< Word 4
		uint16_t w5; ///< Word 5
		uint16_t w6; ///< Word 6
		uint16_t w7; ///< Word 7
	} w;
	/// <summary>
	/// Decoded packet
	/// </summary>
	struct
	{
		uint16_t TransferByteCount; ///< Block Byte count

		uint16_t Resv1 : 4; ///< Reserved
		uint16_t PacketType : 4; ///< Type of packet
		uint16_t RingID : 4; ///< Ring ID of packet
		uint16_t Resv0 : 3; ///< Reserved
		uint16_t Valid : 1; ///< Is the packet valid?

		uint16_t PacketCount : 8; ///< Packet count requested
		uint16_t Resv2 : 8; ///< Reserved
		uint16_t ts10; ///< Timestamp bytes 1 and 2 (Least significant)
		uint16_t ts32; ///< Timestamp bytes 3 and 4
		uint16_t ts54; ///< Timestamp bytes 5 and 6 (Most significant)
		uint16_t data32; ///< Data bytes 1 and 2
		uint16_t data54; ///< Data bytes 3 and 4
	} s;
};
/// <summary>
/// Data Request Packet, hardware view
/// </summary>
union DataRequestPacket
{
	/// <summary>
	/// Raw Packet
	/// </summary>
	struct
	{
		uint16_t w0; ///< Word 0
		uint16_t w1; ///< Word 1
		uint16_t w2; ///< Word 2
		uint16_t w3; ///< Word 3
		uint16_t w4; ///< Word 4
		uint16_t w5; ///< Word 5
		uint16_t w6; ///< Word 6
		uint16_t w7; ///< Word 7
	} w;
	/// <summary>
	/// Decoded packet
	/// </summary>
	struct
	{
		uint16_t TransferByteCount; ///< Block Byte count

		uint16_t Resv1 : 4; ///< Reserved
		uint16_t PacketType : 4; ///< Type of packet
		uint16_t RingID : 4; ///< Ring ID of packet
		uint16_t Resv0 : 3; ///< Reserved
		uint16_t Valid : 1; ///< Is the packet valid?

		uint16_t PacketCount : 8; ///< Packet count requested
		uint16_t Resv2 : 8; ///< Reserved
		uint16_t ts10; ///< Timestamp bytes 1 and 2 (Least significant)
		uint16_t ts32; ///< Timestamp bytes 3 and 4
		uint16_t ts54; ///< Timestamp bytes 5 and 6 (Most significant)
		uint16_t data32; ///< Data bytes 1 and 2
		uint16_t data54; ///< Data bytes 3 and 4
	} s;
};
/// <summary>
/// DataPacket definition, hardware view
/// </summary>
struct DataPacket
{
	uint16_t data10;///< Bytes 1 and 2 of the Data Packet
	uint16_t data32;///< Bytes 3 and 4 of the Data Packet
	uint16_t data54;///< Bytes 5 and 6 of the Data Packet
	uint16_t data76;///< Bytes 7 and 8 of the Data Packet
	uint16_t data98;///< Bytes 9 and 10 of the Data Packet
	uint16_t dataBA;///< Bytes 11 and 12 of the Data Packet
	uint16_t dataDC;///< Bytes 13 and 14 of the Data Packet
	uint16_t dataFE;  ///< Bytes 15 and 16 of the Data Packet
};

// Used in kernel mmap function
#define page2chDirMap( pg, chn, dir, map ) \
	chn =  pg   /4; \
	dir = (pg%4)/2;				\
	map = (pg%2)/1

// Used in user space interface library
#define chnDirMap2offset( chn, dir, map ) \
	( sysconf(_SC_PAGE_SIZE)*(chn*4)		\
	 +sysconf(_SC_PAGE_SIZE)*((dir&1)*2)	\
	 +sysconf(_SC_PAGE_SIZE)*(map&1) )

#define idx_add( idx, add, chn, dir )				 \
	__extension__ ({unsigned num_buffs=cfo_channel_info_[chn][dir].num_buffs; \
	(add<0)								\
		?(((unsigned)-add>idx)					\
		  ?(num_buffs-(-add-idx))%num_buffs		\
		  :(idx-(-add))%num_buffs				\
		  )								\
		:(idx+add)%num_buffs;					\
	})

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
#define CFO_IOC_MAGIC 'C'

#define M_IOC_REG_ACCESS     _IOWR( CFO_IOC_MAGIC, 9, m_ioc_reg_access_t )
#define M_IOC_GET_TST_STATE  _IOWR( CFO_IOC_MAGIC, 1, m_ioc_cmd_t )
#define M_IOC_TEST_START     _IOW ( CFO_IOC_MAGIC, 2, m_ioc_cmd_t )
#define M_IOC_TEST_STOP      _IOW ( CFO_IOC_MAGIC, 3, m_ioc_cmd_t )

#define M_IOC_GET_PCI_STATE  _IOR ( CFO_IOC_MAGIC, 4, m_ioc_pcistate_t )
#define M_IOC_GET_ENG_STATE  _IOWR( CFO_IOC_MAGIC, 5, m_ioc_engstate_t )
#define M_IOC_GET_DMA_STATS  _IOWR( CFO_IOC_MAGIC, 6, m_ioc_engstats_t )
#define M_IOC_GET_TRN_STATS  _IOWR( CFO_IOC_MAGIC, 7, TRNStatsArray )

#define M_IOC_GET_INFO	     _IOWR( CFO_IOC_MAGIC,12, m_ioc_get_info_t  )
#define M_IOC_BUF_GIVE       _IO  ( CFO_IOC_MAGIC,13 )//arg=(chn<<24)|(dir<<16)|num
#define M_IOC_DUMP           _IO  ( CFO_IOC_MAGIC,14 )
#define M_IOC_BUF_XMIT       _IO  ( CFO_IOC_MAGIC,16 )

/// <summary>
/// Register Access information
/// </summary>
typedef struct
{
	int reg_offset; ///< Offset of register from BAR0
	int access_type;		///< 0=read, 1=write
	unsigned val;	///< Value of register
} m_ioc_reg_access_t;

/** Structure used in IOCTL to start/stop a test & to get current test state */
typedef struct
{
	int      Engine;        /**< Engine Number */
	unsigned TestMode;      /**< Test Mode - Enable TX, Enable loopback */
	unsigned MinPktSize;    /**< Min packet size */
	unsigned MaxPktSize;    /**< Max packet size */
} m_ioc_cmd_t;

/** Structure used in IOCTL to get PCIe state from driver */
typedef struct
{
	unsigned Version;       /**< Hardware design version info */
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

/// <summary>
/// Structure used in IOCTL to get DMA Engine state from driver
/// </summary>
typedef struct
{
	int      Engine;        /**< Engine Number */
	int      BDs;           /**< Total Number of BDs */
	int      Buffers;       /**< Total Number of buffers */
	unsigned MinPktSize;    /**< Minimum packet size */
	unsigned MaxPktSize;    /**< Maximum packet size */
	int      BDerrs;        /**< Total BD errors */
	int      BDSerrs;       /**< Total BD short errors - only TX BDs */
	int      IntEnab;       /**< Interrupts enabled or not */
	unsigned TestMode;      /**< Current Test Mode */
} m_ioc_engstate_t;

/// <summary>
/// Structure used in IOCTL to get DMA statistics from driver (single data point)
/// </summary>
typedef struct  /* MAYBE THIS SHOULD GO ELSEWHERE??? */
{
	int      Engine;        /**< Engine Number */
	unsigned LBR;           /**< Last Byte Rate */
	unsigned LAT;           /**< Last Active Time */
	unsigned LWT;           /**< Last Wait Time */
} DMAStatistics;

/// <summary>
/// Structure used in IOCTL to get DMA statistics from driver
/// </summary>
typedef struct
{
	int            Count;   /**< Number of statistics captures */
	DMAStatistics *engptr;  /**< Pointer to array to store statistics */
} m_ioc_engstats_t;

/// <summary>
/// Structure used in IOCTL to get transmit and receive statistics from driver
/// </summary>
typedef struct
{
	unsigned int LTX;           /**< Last TX Byte Rate */
	unsigned int LRX;           /**< Last RX Byte Rate */
} TRNStatistics;

/** Structure used in IOCTL to get PCIe TRN statistics from driver */
typedef struct
{
	int Count;                  /**< Number of statistics captures */
	TRNStatistics * trnptr;     /**< Pointer to array to store statistics */
} TRNStatsArray;

/** Structure used to hold software statistics */
typedef struct
{
	int Engine;                 /**< Engine Number */
	unsigned int LBR;           /**< Last Byte Rate */
} SWStatistics;

/** Structure used in IOCTL to get software statistics from driver */
typedef struct
{
	int Count;                  /**< Number of statistics captures */
	SWStatistics * swptr;       /**< Pointer to array to store statistics */
} SWStatsArray;

//------------------------------------------

typedef unsigned char cfo_databuff_t[0x10000];


typedef enum {
	CFO_DMA_Engine_DAQ = 0,
	CFO_DMA_Engine_DCS = 1,
	CFO_DMA_Engine_Invalid,
} CFO_DMA_Engine;

typedef enum {
	CFO_DMA_Direction_C2S = 0,
	CFO_DMA_Direction_S2C = 1,
	CFO_DMA_Direction_Invalid,
} CFO_DMA_Direction;

enum { C2S, S2C };
enum { CFO_MAP_BUFF, CFO_MAP_META };

#define CFO_Register_Engine_Control( eng, dir ) (((eng*0x100)+(dir*0x2000))+0x4)

/// <summary>
/// Structure used in IOCTL to get information about DMA transfer buffer status
/// </summary>
typedef struct
{
	int chn; ///< Channel (DAQ or DCS)
	int dir; ///< Direction (C2S or S2C)
	int	tmo_ms; ///< Timeout for buffers
	unsigned buff_size; ///< Size of the buffers in this chn/dir
	unsigned num_buffs; ///< Number of buffers in this chn/dir
	unsigned hwIdx; ///< Current buffer index for the hardware
	unsigned swIdx; ///< Current buffer index for the software
    /* unsigned cmpltIdx; maybe also need... particularly/especially for S2C */
} m_ioc_get_info_t;


/* This inline references cfo_channel_info_ -- the name of a variable in both
   userspace and kernel land. For userspace the variable is defined in
   the cfodev class (and therefore the using namsspace
 */
static inline unsigned cfo_chn_info_delta_(int chn, int dir, m_ioc_get_info_t(*cfo_channel_info_)[CFO_MAX_CHANNELS][2])
{
	unsigned hw = (*cfo_channel_info_)[chn][dir].hwIdx;
	unsigned sw = (*cfo_channel_info_)[chn][dir].swIdx;
	unsigned retval;
	if (dir == C2S)
		retval = ((hw >= sw)
			? hw - sw
			: (*cfo_channel_info_)[chn][dir].num_buffs + hw - sw);
	else
		retval = ((sw >= hw)
			? (*cfo_channel_info_)[chn][dir].num_buffs - (sw - hw)
			: hw - sw);

	TRACE(21, "cfo_mmap_ioctl::delta_ chn=%d dir=%d hw=%u sw=%u num_buffs=%u delta=%u"
		, chn, dir, hw, sw, (*cfo_channel_info_)[chn][dir].num_buffs, retval);
	return retval;
}



// stuff from obsolete include/xpmon_be.h
#define LINK_UP             1           /**< Link State is Up */
#define INT_NONE            0x0         /**< No Interrupt capability */
#define INT_LEGACY          0x1         /**< Legacy Interrupts capability */
#define INT_MSI             0x2         /**< MSI Interrupts capability */
#define INT_MSIX            0x3         /**< MSI-X Interrupts capability */


#endif // CFO_MMAP_IOCTL_H