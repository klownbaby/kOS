#pragma once

/* Define a standard status check return value */
typedef enum kstatus {
  STATUS_SUCCESS,
  STATUS_FAILED,
  STATUS_INVALID,
  STATUS_NOT_CONFIGURED,
  STATUS_INSUFFICIENT_SPACE,
  STATUS_UNKNOWN
} kstatus_t;

/* Paging structures */
typedef union {
    uint32_t data;

    struct {
        uint32_t present : 1;
        uint32_t rw : 1;
        uint32_t us : 1;
        uint32_t write_through : 1;
        uint32_t cache_disable : 1;
        uint32_t accessed : 1;
        uint32_t avail_low : 1;
        uint32_t page_size : 1;
        uint32_t avail_high : 4;
        uint32_t pt_addr : 20;
    } page_dir;
} page_dir_t;

typedef union {
    uint32_t data;

    struct {
        uint32_t present : 1;
        uint32_t rw : 1;
        uint32_t us : 1;
        uint32_t write_through : 1;
        uint32_t cache_disable : 1;
        uint32_t accessed : 1;
        uint32_t avail_low : 1;
        uint32_t page_size : 1;
        uint32_t global : 1;
        uint32_t avail_high : 3;
        uint32_t pt_addr : 20;
    } pte;
} pte_t;

struct freelist {
    struct freelist* next;
};

typedef struct {
    struct freelist* freelist;
} mmap_t;

