/******************************************************************************
 * Filename:    util_dvf_auto.h
 * Author:      <michkochris@gmail.com>
 * Date:        2026-03-04
 * Description: Binary mmap autocompletion engine declarations for util_dvf
 * LICENSE:     GPL v3
 ******************************************************************************/

#ifndef UTIL_DVF_AUTO_H
#define UTIL_DVF_AUTO_H

#include "util_dvf_portable.h"

#ifdef __cplusplus
extern "C" {
#endif

int util_dvf_auto_init(const char *index_path);
void util_dvf_auto_cleanup(void);
int util_dvf_auto_complete(const char *prefix);

#ifdef __cplusplus
}
#endif

#endif /* UTIL_DVF_AUTO_H */
