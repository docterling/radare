#ifndef _INCLUDE_RDB_H_
#define _INCLUDE_RDB_H_

#define _FILE_OFFSET_BITS 64
#define _GNU_SOURCE
#include <stdio.h>
#include <sys/types.h>
#include "radare.h"
#include "list.h"

// TODO: rename to ref_t or addr_t or something like that
struct xrefs_t {
	u64 addr;  /* offset of the cross reference */
	u64 from;  /* where the code/data is referenced */
	int type;  /* 0 = code, 1 = data, -1 = unknown */
	struct list_head list;
};

enum {
	BLK_TYPE_HEAD,  // first block
	BLK_TYPE_BODY,  // conditional jump
	BLK_TYPE_LAST,  // ret
	BLK_TYPE_FOOT   // unknown jump
};

extern const char *block_type_names[4];

/* code block */
struct block_t {
	char *name;
	char *comment;
	u64 addr;
	int type;

	int ignored;
	int framesize;

	unsigned int n_bytes;
	unsigned char *bytes;
	unsigned int checksum; // 32 bit checksum of the block bytes

	unsigned int n_calls;
	struct list_head calls;

	unsigned int n_xrefs;
	struct list_head xrefs;
	struct list_head list;
	u64 tnext; // true conditional jump
	u64 fnext; // false conditional jump

	void *data; // user data
};


/* program structure */
struct program_t {
	char *name;
#if 0
	int setuid; // WTF?
	int setgid;
#endif

	u64 entry;

	int n_blocks;
	struct list_head blocks;
	/* TODO */
	// sections
	struct list_head list;
};

/* functions */
struct program_t *program_new(char *file);
int block_set_bytes(struct program_t *program, u64 addr, char *hexpairs);
int program_free(struct program_t *program);
void program_reset(struct program_t *program);
int program_serialize(struct program_t *program, char *file);
struct block_t *block_new(u64 addr);
struct block_t *block_get(struct program_t *program, u64 addr);
struct block_t *block_get_new(struct program_t *program, u64 addr);
struct block_t *block_split_new(struct program_t *program, u64 addr);
int block_set_framesize(struct program_t *program, u64 addr, int size);
int block_set_name(struct program_t *program, u64 addr, char *name);
int block_add_xref(struct program_t *program, u64 addr, u64 from);
int block_set_comment(struct program_t *program, u64 addr, char *comment);
int block_set_bytes(struct program_t *program, u64 addr, char *hexpairs);
int block_add_call(struct program_t *program, u64 addr, u64 dest);
struct program_t *program_open(char *file);
int rdb_diff(struct program_t *a, struct program_t *b, int mode);

// XXX
struct program_t *code_analyze(u64 seek, int depth);

#endif
