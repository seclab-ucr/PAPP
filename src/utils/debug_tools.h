#ifndef _DEBUG_TOOLS_H_
#define _DEBUG_TOOLS_H_

static void getScheduleStats(uint64_t * r1, uint64_t * r2) {
    // schedulerLog[schedulerLogPtr] = getContextSwitches();
    // schedulerLogPtr++;
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen("/proc/schedstat", "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    while ((read = getline(&line, &len, fp)) != -1) {
        if (line[0] == 'c' && line[1] == 'p' && line[2] == 'u' && line[3] == '2') {
            // FPRINTF_ERR "%s", line);
            sscanf(line, "cpu2 %*d %*d %llu %llu", r1, r2);
            break;
        }
    }

    fclose(fp);
    if (line)
        free(line);
    return;
}

static uint64_t getProcessEip(char* pid) {
    char fn[100];
    char c;
    char buf[1024];
    int i;
    sprintf(fn, "/proc/%s/stat", pid);
    // FPRINTF_ERR "%s\n", fn);
    uint64_t ret = 1;
    FILE * fp;
    size_t len = 0;
    ssize_t read = 0;
    fp = fopen(fn, "r");
    // FPRINTF_ERR "%u\n", fp);
    if (fp == NULL) {
        return -1;
    }
    i = 0;
    while ((c = getc(fp)) != EOF) {
        if (c != 0 && c != '\r' && c != '\n') {
            buf[i] = c;
            i ++;
        }
    }
    buf[i] = 0;
    sscanf(buf, "%*d (com.baidu.input) %*s %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %llu", &ret);
    // FPRINTF_ERR "%s\n", buf);
    fclose(fp);
    return ret;
}

static uint64_t getProcessEipShell(char * pid) {
    char cmd[100];
    uint64_t ret = 1;
    char * line = NULL;
    size_t len = 0;
    ssize_t read = 0;
    sprintf(cmd, "su -c cat /proc/%s/stat >> /mnt/sdcard/log_imeeip", pid);
    FILE *fp = popen(cmd, "r");
    if (fp == NULL) {
        return -1;
    }
    /*
    while ((read = getline(&line, &len, fp)) != -1) {
        FPRINTF_ERR "%s %d %d\n", cmd, ret, read);
        if (line[0] == '9') {
            sscanf(line, "%*d (com.baidu.input) %*s %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %llu", &ret);
            break;
        }
    }
    */
    pclose(fp);
    return ret;
}

#endif
