#ifndef BANANA_DOMAIN_STATUS_H
#define BANANA_DOMAIN_STATUS_H

typedef enum BananaStatus {
    BANANA_OK = 0,
    BANANA_ERROR_INVALID_INPUT = 1,
    BANANA_ERROR_OVERFLOW = 2,
    BANANA_ERROR_NOT_FOUND = 3
} BananaStatus;

#endif