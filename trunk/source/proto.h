#ifndef _PROTO_H_
#define _PROTO_H_

#define SIZEOF_CHAR 1
#define SIZEOF_SHORT 2
#define SIZEOF_INT 4

#include <winsock2.h>
#include <bittypes.h>
typedef u_int32_t uint32_t;

typedef u_short n_short;			/* short as received from the net */
typedef u_int	n_long;			/* long as received from the net */

typedef	u_int	n_time;			/* ms since 00:00 GMT, byte rev */


#include "tcp.h"
#include "ip.h"

#endif