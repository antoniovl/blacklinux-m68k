#ifndef __NEXT_MON_REGION_H__

struct mon_region {
	long first_phys_addr;
	long last_phys_addr;
};
typedef struct mon_region *mon_region_t;

#define __NEXT_MON_REGION_H__
#endif