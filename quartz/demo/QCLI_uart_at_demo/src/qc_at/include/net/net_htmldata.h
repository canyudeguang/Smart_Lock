/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/* htmldata.h

   This file was built from 7 entities in input.txt.

*/

#ifndef _web_H_
#define _web_H_ 1

#ifndef NULL
#define NULL 0
#endif

struct vfs_file
{
	struct vfs_file *next;
	char name[32];
	uint16_t flags;
	uint8_t *data;
	uint32_t real_size;
	uint32_t comp_size;
	uint32_t buf_size;
	int (*cgi_func)(void *, void *, char **);
	void *method;
};

/* Status returned from cgi_func(void *, void *, char **) */
#define	FP_ERR		0x110
#define	FP_FILE		0x140
#define	FP_DONE		0x200
#define	FP_BADREQ	400
/* CGI routines */
extern int	cgi_setintf(void *, void *, char **);
extern int	cgi_showintf(void *, void *, char **);
extern int	cgi_demo(void *, void *, char **);
extern int	cgi_demo(void *, void *, char **);
extern int	cgi_demo(void *, void *, char **);

extern void webfiles_setup(void);
extern const unsigned char index_246iws[];
extern const unsigned char iot_295banner_246png[];
extern struct vfs_file webfiles_array[7];
extern struct vfs_file *vfsfiles;

#endif /* _web_H_ */

/********* End of file *************/
