#define SIZE_DMESG_BUFFER (DMESG_BFS * PGSIZE)

void pr_msg(const char *fmt, ...);

void pr_copy(uint64 p);
