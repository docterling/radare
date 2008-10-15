
extern struct reflines_t *reflines;

int data_set_len(u64 off, u64 len);
void data_info();
int data_set(u64 off, int type);
struct data_t *data_add_arg(u64 off, int type, const char *arg);
struct data_t *data_add(u64 off, int type);
u64 data_seek_to(u64 offset, int type, int idx);
struct data_t *data_get(u64 offset);
struct data_t *data_get_range(u64 offset);
struct data_t *data_get_between(u64 from, u64 to);
int data_type_range(u64 offset);
int data_type(u64 offset);
int data_end(u64 offset);
int data_size(u64 offset);
int data_list();
int data_xrefs_print(u64 addr, int type);
int data_xrefs_add(u64 addr, u64 from, int type);
int data_xrefs_at(u64 addr);
void data_xrefs_del(u64 addr, u64 from, int data /* data or code */);
void data_comment_del(u64 offset, const char *str);
void data_comment_add(u64 offset, const char *str);
void data_comment_list();
void data_xrefs_here(u64 addr);
void data_xrefs_list();
char *data_comment_get(u64 offset, int lines);
void data_comment_init(int new);
void data_reflines_init();
int data_printd(int delta);
