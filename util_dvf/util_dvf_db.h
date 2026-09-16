/******************************************************************************
 * Filename:    util_dvf_db.h
 * Author:      <michkochris@gmail.com>
 * Date:        2026-03-04
 * Description: Binary database engine, pkginfo.bin serialization & host RPM sync declarations
 * LICENSE:     GPL v3
 ******************************************************************************/

#ifndef UTIL_DVF_DB_H
#define UTIL_DVF_DB_H

#include "util_dvf_portable.h"
#include <stdio.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Master flattened package metadata struct for FSM & binary serialization */
typedef struct {
    char name[128];         /* RPMTAG_NAME */
    char version[64];       /* RPMTAG_VERSION */
    char release[64];       /* RPMTAG_RELEASE */
    char arch[32];          /* RPMTAG_ARCH */
    char epoch[16];         /* RPMTAG_EPOCH */
    int require_count;
    int provide_count;
    int file_count;
    off_t payload_offset;
    char compression_type[16]; /* "gzip", "xz", "zstd" */
    char checksum[128];     /* SHA256 digest */
    char source_rpm[256];
} DvfPkgInfo;

int util_dvf_db_init(void);
void util_dvf_db_cleanup(void);

/* Construct path: library_path / pkgname-version-release.arch / pkginfo.bin */
void util_dvf_get_pkg_db_path(const DvfPkgInfo *pkg, char *dest, size_t max_len);

/* Serialize DvfPkgInfo to pkginfo.bin */
int util_dvf_save_pkg_info(const DvfPkgInfo *pkg);

/* Deserialize DvfPkgInfo from pkginfo.bin */
int util_dvf_load_pkg_info(const char *name, const char *version, const char *release, const char *arch, DvfPkgInfo *pkg);

/* Sync host RPM database (/var/lib/rpm/rpmdb.sqlite) into local library_path binary cache */
int util_dvf_sync_host_rpm_db(void);

#ifdef __cplusplus
}
#endif

#endif /* UTIL_DVF_DB_H */
