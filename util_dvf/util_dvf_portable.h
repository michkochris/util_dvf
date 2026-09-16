/******************************************************************************
 * Filename:    util_dvf_portable.h
 * Author:      <michkochris@gmail.com>
 * Date:        2026-03-04
 * Description: Portable definitions and types for util_dvf (C89/90 compliant)
 * LICENSE:     GPL v3
 ******************************************************************************/

#ifndef UTIL_DVF_PORTABLE_H
#define UTIL_DVF_PORTABLE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#ifdef __cplusplus
}
#endif

#endif /* UTIL_DVF_PORTABLE_H */
