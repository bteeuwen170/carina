int chdir(const char *path);
int chmod(const char *path, mode_t mode);
int chown(const char *path, uid_t owner, gid_t group);
//clone(u64 flags);
int close(int fd);
//int create(const char *path, int flags);
int exec(const char *path, char *const argv[]);
pid_t fork(void);
getgid
gethn	//Hostname
getime
getpid
getuid
halt
mkdir
mkpipe
mount
nsleep
int open(const char *path, int flags);
read(int fd, void *buf, size_t count);
readdir
reboot(void);
rename
rmdir
setgid
sethn	//Hostname
setime
setuid
signal
signal
stat
sync
umount
write
