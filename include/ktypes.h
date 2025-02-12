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

