#ifndef __NEXT_MON_SIO_H__

enum SIO_ARGS {
	SIO_AUTOBOOT,
	SIO_SPECIFIED,
	SIO_ANY
};

struct device {
	int (*d_open)();
	int (*d_close)();
	int (*d_read)();
	int (*d_write)();
	int (*d_label_blkno)();
	int d_io_type;
	#define D_IOT_CHAR	0
	#define D_IOT_PACKET	1
};

struct sio {
	enum SIO_ARGS si_args;
	unsigned int si_ctrl, si_unit, si_part;
	struct device *si_dev;
	unsigned int si_blklen;
	unsigned int si_lastlba;
	caddr_t si_sadmem, si_protmem, si_devmem;
};

#define __NEXT_MON_SIO_H__
#endif