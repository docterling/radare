/* radare - LGPL - Copyright 2008-2009 pancake<nopcode.org> */

#include "r_io.h"
#include <stdio.h>

static struct r_io_handle_t *plugin;
static int cache_fd;
u64 r_io_seek = 0; // XXX should be store per io_handle

int r_io_init()
{
	r_io_map_init();
	r_io_section_init();
	r_io_handle_init();
	return 0;
}

int r_io_open(const char *file, int flags, int mode)
{
	struct r_io_handle_t *plugin = r_io_handle_resolve(file);
	if (plugin) {
		int fd = plugin->open(file, flags, mode);
		if (fd != -1)
			r_io_handle_open(fd, plugin);
		return fd;
	}
	return open(file, flags, mode);
}

int r_io_read(int fd, u8 *buf, int len)
{
	if (r_io_map_read_at(r_io_seek, buf, len) != 0)
		return len;
	if (fd != cache_fd)
		plugin = r_io_handle_resolve_fd(fd);
	if (plugin) {
		cache_fd = fd;
		return plugin->read(fd, buf, len);
	}
	return read(fd, buf, len);
}

int r_io_resize(const char *file, int flags, int mode)
{
#if 0
	/* TODO */
	struct r_io_handle_t *plugin = r_io_handle_resolve(file);
	if (plugin) {
		int fd = plugin->open(file, flags, mode);
		if (fd != -1)
			r_io_handle_open(fd, plugin);
		return fd;
	}
#endif
	return -1;
}

/* write mask */
static int r_io_write_mask_fd = -1;
static u8 *r_io_write_mask_buf;
static int r_io_write_mask_len;

int r_io_set_write_mask(int fd, const u8 *buf, int len)
{
	if (len) {
		r_io_write_mask_fd = fd;
		r_io_write_mask_buf = (u8 *)malloc(len);
		memcpy(r_io_write_mask_buf, buf, len);
		r_io_write_mask_len = len;
	} else r_io_write_mask_fd = -1;
	return 0;
}

int r_io_write(int fd, const u8 *buf, int len)
{
	int i, ret = -1;

	/* apply write binary mask */
	if (r_io_write_mask_fd != -1) {
		u8 *data = alloca(len);
		r_io_lseek(fd, r_io_seek, R_IO_SEEK_SET);
		r_io_read(fd, data, len);
		r_io_lseek(fd, r_io_seek, R_IO_SEEK_SET);
		for(i=0;i<len;i++) {
			data[i] = buf[i] & \
				r_io_write_mask_buf[i%r_io_write_mask_len];
		}
		buf = data;
	}

	if (r_io_map_write_at(r_io_seek, buf, len) != 0)
		return len;
	if (fd != cache_fd)
		plugin = r_io_handle_resolve_fd(fd);
	if (plugin) {
		cache_fd = fd;
		ret = plugin->write(fd, buf, len);
	} else ret = write(fd, buf, len);
	if (ret == -1)
		fprintf(stderr, "io: cannot write\n");
	return ret;
}

u64 r_io_lseek(int fd, u64 offset, int whence)
{
	int posix_whence = 0;
	if (whence == SEEK_SET)
		offset = r_io_section_align(offset, 0, 0);

	/* pwn seek value */
	switch(whence) {
	case R_IO_SEEK_SET:
		r_io_seek = offset;
		posix_whence = SEEK_SET;
		break;
	case R_IO_SEEK_CUR:
		r_io_seek += offset;
		posix_whence = SEEK_CUR;
		break;
	case R_IO_SEEK_END:
		r_io_seek = 0xffffffff;
		posix_whence = SEEK_END;
		break;
	}

	if (fd != cache_fd)
		plugin = r_io_handle_resolve_fd(fd);
	if (plugin) {
		cache_fd = fd;
		return plugin->lseek(fd, offset, whence);
	}
	// XXX can be problematic on w32..so no 64 bit offset?
	return lseek(fd, offset, posix_whence);
}

u64 r_io_size(int fd)
{
	u64 size, here = r_io_lseek(fd, 0, R_IO_SEEK_CUR);
	size = r_io_lseek(fd, 0, R_IO_SEEK_END);
	r_io_lseek(fd, here, R_IO_SEEK_SET);
	return size;
}

int r_io_system(int fd, const char *cmd)
{
	if (fd != cache_fd)
		plugin = r_io_handle_resolve_fd(fd);
	if (plugin) {
		cache_fd = fd;
		return plugin->system(cmd);
	}
	return system(cmd);
}

int r_io_close(int fd)
{
	if (fd != cache_fd)
		plugin = r_io_handle_resolve_fd(fd);
	if (plugin) {
		cache_fd = fd;
		r_io_handle_close(fd, plugin);
		return plugin->close(fd);
	}
	return close(fd);
}
