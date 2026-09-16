/******************************************************************************
 * Filename:    util_dvf_rpm.h
 * Author:      <michkochris@gmail.com>
 * Date:        2026-03-04
 * Description: Raw binary RPM header parser & DvfPkgInfo mapper declarations
 * LICENSE:     GPL v3
 ******************************************************************************/

#ifndef UTIL_DVF_RPM_H
#define UTIL_DVF_RPM_H

#include "util_dvf_portable.h"
#include "util_dvf_db.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/* RPM Magic Numbers */
#define RPM_LEAD_MAGIC   0xedabeedb
#define RPM_HEADER_MAGIC 0x8eade801

/* Common RPM Tag IDs */
#define RPMTAG_NAME      1000
#define RPMTAG_VERSION   1001
#define RPMTAG_RELEASE   1002
#define RPMTAG_EPOCH     1003
#define RPMTAG_ARCH      1022
#define RPMTAG_SOURCERPM 1044

/* RPM Header Index Entry */
typedef struct {
    uint32_t tag;
    uint32_t type;
    uint32_t offset;
    uint32_t count;
} DvfHeaderIndexEntry;

/* Parse a local .rpm file binary header and populate DvfPkgInfo */
int util_dvf_rpm_parse_header(const char *rpm_filepath, DvfPkgInfo *pkg_info);

#ifdef __cplusplus
}
#endif

#endif /* UTIL_DVF_RPM_H */
