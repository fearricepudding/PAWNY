#pragma once

#include <linux/types.h>
#include <linux/stddef.h>

typedef __u32 canid_t;

struct canfd_frame {
	canid_t can_id;  /* 32 bit CAN_ID + EFF/RTR/ERR flags */
	__u8    len;     /* frame payload length in byte */
	__u8    flags;   /* additional flags for CAN FD */
	__u8    __res0;  /* reserved / padding */
	__u8    __res1;  /* reserved / padding */
	__u8    data[64] __attribute__((aligned(8)));
};
