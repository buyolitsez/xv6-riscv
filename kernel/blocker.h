int blget();

int bllock(int);

int blrelease(int);

int bldelete(int);

enum blocker_cmd {BGET, BACQUIRE, BRELEASE, BDELETE};
