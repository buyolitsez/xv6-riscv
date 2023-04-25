#define SIZE_DMESG_BUFFER (DMESG_BFS * PGSIZE)

void pr_msg(const char *fmt, ...);

void pr_copy(uint64 p);

void check_dmesg_logging(uint flag);

extern uint dmesg_log_interrupt;
extern uint dmesg_log_switching;
extern uint dmesg_log_syscall;
extern uint dmesg_log_reg;

#define DMESG_LOG(TYPE, ...) do { \
    if (dmesg_log_ ## TYPE) \
      pr_msg(__VA_ARGS__);\
} while(0)

