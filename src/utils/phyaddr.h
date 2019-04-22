#ifndef _PHYADDR_H_
#define _PHYADDR_H_

/*
 * Get physical Address
 */
#define PAGEMAP_ENTRY 8
#define GET_BIT(X,Y) (X & ((uint64_t)1<<Y)) >> Y
#define SET_BIT(X,Y) X = X | ((uint64_t)1<<Y)
#define CLEAR_BIT(X,Y) X = X & (~((uint64_t)1<<Y))
#define GET_PFN(X) X & 0x7FFFFFFFFFFFFF

static const int __endian_bit = 1;
#define is_bigendian() ( (*(char*)&__endian_bit) == 0 )

#if 0
static unsigned long getPhyAddr(unsigned long virt_addr) {
    // printf("Big endian? %d\n", is_bigendian());
    int i, c, pid, status;
    FILE * f;
    char * end;
    uint64_t file_offset, read_val;
    int pagesize = sysconf(_SC_PAGESIZE);
    f = fopen("/proc/self/pagemap", "rb");
    if (!f) {
        // printf("Error! Cannot open pagemap\n");
        return -1;
    }

    //Shifting by virt-addr-offset number of bytes
    //and multiplying by the size of an address (the size of an entry in pagemap file)
    file_offset = virt_addr / getpagesize() * PAGEMAP_ENTRY;
    // printf("Vaddr: 0x%lx, Page_size: %d, Entry_size: %d\n", virt_addr, pagesize, PAGEMAP_ENTRY);
    // printf("Reading %s at 0x%llx\n", path_buf, (unsigned long long) file_offset);
    status = fseek(f, file_offset, SEEK_SET);
    if (status) {
        perror("Failed to do fseek!");
        return -1;
    }
    // errno = 0;
    read_val = 0;
    unsigned char c_buf[PAGEMAP_ENTRY];
    for (i = 0; i < PAGEMAP_ENTRY; i++) {
        c = getc(f);
        if (c == EOF) {
            printf("\nReached end of the file\n");
            return -1;
        }
        if (is_bigendian())
            c_buf[i] = c;
        else
            c_buf[PAGEMAP_ENTRY - i - 1] = c;
        // printf("[%d]0x%x ", i, c);
    }
    for (i = 0; i < PAGEMAP_ENTRY; i++) {
        //printf("%d ",c_buf[i]);
        read_val = (read_val << 8) + c_buf[i];
    }
    // printf("\n");
    // printf("Result: 0x%llx\n", (unsigned long long) read_val);
    //if(GET_BIT(read_val, 63))
    if (GET_BIT(read_val, 63)) {
        // printf("PFN: 0x%llx\n",(unsigned long long) GET_PFN(read_val));
        unsigned long pfn = GET_PFN(read_val);
        unsigned long paddr = (pfn * pagesize) + (virt_addr & (pagesize - 1));
        return (unsigned long) paddr;
    } else {
        // printf("Page not present\n");
        return 0;
    }
    // if(GET_BIT(read_val, 62))
    //   printf("Page swapped\n");
    fclose(f);
    return -1;
}
#endif

static int getPhyAddrs(PNP_ADDR * virt_addrs, PNP_ADDR * phy_addrs, int n) {
    // printf("Big endian? %d\n", is_bigendian());
    int i, c, pid, status;
    FILE * f;
    char * end;
    uint64_t file_offset, read_val;
    int pagesize = sysconf(_SC_PAGESIZE);
    unsigned long virt_addr;
    f = fopen("/proc/self/pagemap", "rb");
    if (!f) {
        FPRINTF_ERR "Error! Cannot open pagemap\n");
        return 0;
    }

    int x = 0;
    while (x < n) {
        virt_addr = virt_addrs[x];
        file_offset = virt_addr / pagesize * PAGEMAP_ENTRY;
        status = fseek(f, file_offset, SEEK_SET);
        if (status) {
            FPRINTF_ERR "Failed to do fseek!\n");
            return -1;
        }
        // errno = 0;
        read_val = 0;
        unsigned char c_buf[PAGEMAP_ENTRY];
        for (i = 0; i < PAGEMAP_ENTRY; i++) {
            c = getc(f);
            if (c == EOF) {
                FPRINTF_ERR "\nReached end of the file\n");
                return -1;
            }
            if (is_bigendian())
                c_buf[i] = c;
            else
                c_buf[PAGEMAP_ENTRY - i - 1] = c;
        }
        for (i = 0; i < PAGEMAP_ENTRY; i++) {
            read_val = (read_val << 8) + c_buf[i];
        }
        // fprintf(stderr, "%#016llx\n", read_val);
        if (GET_BIT(read_val, 63)) {
            // printf("PFN: 0x%llx\n",(unsigned long long) GET_PFN(read_val));
            PNP_ADDR pfn = GET_PFN(read_val);
            PNP_ADDR paddr = (pfn * pagesize) + (virt_addr & (pagesize - 1));
            phy_addrs[x] = paddr;
        } else {
            // printf("Page not present\n");
            phy_addrs[x] = 0;
        }
        x = x + 1;
    }
    fclose(f);
    return 1;
}

static int __getPhyAddrs(FILE * f, PNP_ADDR * virt_addrs, PNP_ADDR * phy_addrs, int n) {
    // printf("Big endian? %d\n", is_bigendian());
    int i, c, pid, status;
    char * end;
    uint64_t file_offset, read_val;
    int pagesize = sysconf(_SC_PAGESIZE);
    uint64_t virt_addr;
    // f = fopen("/proc/self/pagemap", "rb");
    // if (!f) {
    //     FPRINTF_ERR "Error! Cannot open pagemap\n");
    //     return 0;
    // }

    int x = 0;
    while (x < n) {
        virt_addr = virt_addrs[x];
        file_offset = virt_addr / pagesize * PAGEMAP_ENTRY;
        status = fseek(f, file_offset, SEEK_SET);
        // fprintf(stderr, "%llu ", file_offset);
        if (status) {
            FPRINTF_ERR "Failed to do fseek!\n");
            return -1;
        }
        // errno = 0;
        read_val = 0;
        unsigned char c_buf[PAGEMAP_ENTRY];
        for (i = 0; i < PAGEMAP_ENTRY; i++) {
            c = getc(f);
            if (c == EOF) {
                FPRINTF_ERR "\nReached end of the file\n");
                return -1;
            }
            if (is_bigendian())
                c_buf[i] = c;
            else
                c_buf[PAGEMAP_ENTRY - i - 1] = c;
        }
        for (i = 0; i < PAGEMAP_ENTRY; i++) {
            read_val = (read_val << 8) + c_buf[i];
        }
        // fprintf(stderr, "%#018llx\n", read_val);
        if (GET_BIT(read_val, 63)) {
            // printf("PFN: 0x%llx\n",(unsigned long long) GET_PFN(read_val));
            unsigned long pfn = GET_PFN(read_val);
            unsigned long paddr = (pfn * pagesize) + (virt_addr & (pagesize - 1));
            phy_addrs[x] = paddr;
        } else {
            // printf("Page not present\n");
            phy_addrs[x] = 0;
        }
        x = x + 1;
    }
    return 0;
}

static PNP_ADDR getPhyAddr(PNP_ADDR virt_addr) {
    PNP_ADDR ret;
    getPhyAddrs(&virt_addr, &ret, 1);
    return ret;
}

#endif
