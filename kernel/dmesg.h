#define SIZE_DMESG_BUFFER (DMESG_BFS * PGSIZE)

void pr_msg(const char *fmt, ...);

void pr_copy(uint64 p);

void addTicksToLog(uint ticks_more);

extern uint ticks_to_log;

#define DMESG_LOG(...) do { \
    if (ticks_to_log >= ticks) \
      pr_msg(__VA_ARGS__);\
} while(0)

