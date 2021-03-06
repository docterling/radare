/*
 * Copyright (C) 2007, 2008, 2009
 *       pancake <youterm.com>
 *
 * radare is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * radare is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with radare; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "main.h"
#include <dirent.h>

int io_write_last = 0;

int io_system(const char *command)
{
	FIND_FD(config.fd)
		IF_HANDLED( config.fd, system )
		return (plugins[i].system)(command);

	if (!memcmp(command, "help", 4)) {
		eprintf("Not in debugger.\n");
		return 0;
	}
	return radare_system(command);
}

/* io wrappers */
int io_open(const char *pathname, int flags, mode_t mode)
{
	FIND_OPEN(pathname) {
		IF_HANDLED(0, open)
		return plugins[i].open(pathname, flags, mode);
	}
	return -1;
}

ssize_t io_read(int fd, void *buf, size_t count)
{
//eprintf("io-read 0x%08llx %d\n", config.seek, count);
	if (io_map_read_at(config.seek, (u8 *)buf, count) != 0)
		return count;
	FIND_FD(fd)
		IF_HANDLED(fd, read)
		return plugins[i].read(fd, buf, count);
#if 0
	if (io_map_read_rest(config.seek, (u8 *)buf, count) != 0)
		return count;
#endif
	return -1;
}

ut64 io_lseek(int fd, ut64 offset, int whence)
{
	FIND_FD(fd)
		IF_HANDLED(fd, lseek)
		return plugins[i].lseek(fd, offset, whence);
	return -1;
}

ssize_t io_write(int fd, const void *buf, size_t count)
{
	if (!config_get("file.write")) {
		eprintf("Not in write mode\n");
		return -1;
	}
	io_write_last = count;
	FIND_FD(fd)
		IF_HANDLED(fd, write)
			return plugins[i].write(fd, buf, count);
	return -1;
}

int io_close(int fd)
{
	FIND_FD(fd)
		IF_HANDLED(fd, close)
			return plugins[i].close(fd);
	return -1;
}

int io_isdbg(int fd)
{
	FIND_FD(fd)
		IF_HANDLED(fd, open)
			return (int)(size_t)(plugins[i].debug);
	return 0;
}


/* mapping */

int maps_n = 0;
int maps[10];


struct list_head io_maps;

void io_map_init()
{
	INIT_LIST_HEAD(&io_maps);
}

int io_map_rm(const char *file)
{
	struct list_head *pos;
	list_for_each_prev(pos, &io_maps) {
		struct io_maps_t *im = list_entry(pos, struct io_maps_t, list);
		if (!strcmp(im->file, file)) {
			/* FREE THIS */
			eprintf("TODO\n");
			return 0;
		}
	}
	eprintf("Not found\n");
	return 0;
}

int io_map_list()
{
	int n = 0;
	struct list_head *pos;
	list_for_each(pos, &io_maps) {
		struct io_maps_t *im = list_entry(pos, struct io_maps_t, list);
		if (im->file[0] != '\0') {
			cons_printf("0x%08llx 0x%08llx start: 0x%llx size: 0x%llx %s\n",
				im->from, im->to, im->delta, im->to - im->from, im->file);
			n++;
		}
	}
	return n;
}

int io_map(const char *file, ut64 offset, ut64 delta, ut64 size)
{
	struct io_maps_t *im;
	int fd = open(file, O_RDONLY);
	if (fd == -1)
		return -1;
	im = (struct io_maps_t*)malloc(sizeof(struct io_maps_t));
	im->fd = fd;
	im->delta = delta;
	strncpy(im->file, file, 127);
	im->from = offset;
	if (size) im->to = offset+size;
	else im->to = offset+lseek(fd, 0, SEEK_END);
	list_add(&(im->list), &(io_maps));
	return fd;
}

int io_map_read_at(ut64 off, u8 *buf, ut64 len)
{
	struct list_head *pos;

	list_for_each(pos, &io_maps) {
		struct io_maps_t *im = list_entry(pos, struct io_maps_t, list);
		if (im->file[0] != '\0') {
			if (off >= im->from && off < im->to) {
				if (off + len > im->to)
					len = im->to - off;
				lseek(im->fd, off-im->from+im->delta, SEEK_SET);
				return read(im->fd, buf, len);
			}
		}
	}
	return 0;
}

int io_map_read_rest(ut64 off, u8 *buf, ut64 len)
{
	struct list_head *pos;

	list_for_each(pos, &io_maps) {
		struct io_maps_t *im = list_entry(pos, struct io_maps_t, list);
		if (im->file[0] != '\0') {
			if (off+len >= im->from && off < im->to) {
				lseek(im->fd, im->delta, SEEK_SET);
				return read(im->fd, buf+(im->from-(off+len)), len);
			}
		}
	}
	return 0;
}
