#include "banana_status.h"

const char *banana_status_remediation(int status)
{
    switch (status)
    {
    case BANANA_OK:
        return "";
    case BANANA_INVALID_ARGUMENT:
        return "Check inputs against the wrapper signature.";
    case BANANA_OVERFLOW:
        return "Reduce input magnitudes; result exceeded int32 range.";
    case BANANA_INTERNAL_ERROR:
        return "Native allocation or internal invariant failed.";
    case BANANA_DB_ERROR:
        return "Postgres call failed; check libpq logs.";
    case BANANA_DB_NOT_CONFIGURED:
        return "Set BANANA_PG_CONNECTION (host/port/user/password/dbname).";
    case BANANA_NOT_FOUND:
        return "Lookup miss; verify the supplied identifier.";
    case BANANA_BUFFER_TOO_SMALL:
        return "Reallocate with the size returned in the size out-param.";
    default:
        return "Unknown status code.";
    }
}
