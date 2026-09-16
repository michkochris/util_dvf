/******************************************************************************
 * Filename:    util_dvf_db.c
 * Author:      <michkochris@gmail.com>
 * Date:        2026-03-04
 * Description: Binary database engine, pkginfo.bin serialization & host RPM sync implementation
 * LICENSE:     GPL v3
 ******************************************************************************/

#include "util_dvf_portable.h"
#include "util_dvf_db.h"
#include "util_dvf_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

/* Helper to recursively create directories (mkdir -p equivalent in C89) */
static int mkdir_p(const char *dir_path) {
    char tmp[PATH_MAX];
    char *p = NULL;
    size_t len;

    snprintf(tmp, sizeof(tmp), "%s", dir_path);
    len = strlen(tmp);
    if (len == 0) return 0;

    if (tmp[len - 1] == '/') {
        tmp[len - 1] = '\0';
    }

    for (p = tmp + 1; *p; p++) {
        if (*p == '/') {
            *p = '\0';
            if (mkdir(tmp, 0755) != 0 && errno != EEXIST) {
                return -1;
            }
            *p = '/';
        }
    }
    if (mkdir(tmp, 0755) != 0 && errno != EEXIST) {
        return -1;
    }
    return 0;
}

int util_dvf_db_init(void) {
    if (!g_dvf_library_path) {
        return -1;
    }
    return mkdir_p(g_dvf_library_path);
}

void util_dvf_db_cleanup(void) {
    /* Nothing permanent to cleanup in memory for db core */
}

void util_dvf_get_pkg_db_path(const DvfPkgInfo *pkg, char *dest, size_t max_len) {
    if (!pkg || !dest) return;
    snprintf(dest, max_len, "%s/%s-%s-%s.%s/pkginfo.bin",
             g_dvf_library_path ? g_dvf_library_path : "/srv/lib/util_dvf_dir/util_dvf_db",
             pkg->name, pkg->version, pkg->release, pkg->arch);
}

int util_dvf_save_pkg_info(const DvfPkgInfo *pkg) {
    char bin_path[PATH_MAX];
    char dir_path[PATH_MAX];
    FILE *fp;
    char *last_slash;

    if (!pkg) return -1;

    util_dvf_get_pkg_db_path(pkg, bin_path, sizeof(bin_path));
    snprintf(dir_path, sizeof(dir_path), "%s", bin_path);

    last_slash = strrchr(dir_path, '/');
    if (last_slash) {
        *last_slash = '\0';
        if (mkdir_p(dir_path) != 0) {
            fprintf(stderr, "[DVF-DB-ERROR] Failed to create directory: %s\n", dir_path);
            return -1;
        }
    }

    fp = fopen(bin_path, "wb");
    if (!fp) {
        fprintf(stderr, "[DVF-DB-ERROR] Failed to open %s for writing: %s\n", bin_path, strerror(errno));
        return -1;
    }

    if (fwrite(pkg, sizeof(DvfPkgInfo), 1, fp) != 1) {
        fprintf(stderr, "[DVF-DB-ERROR] Failed to write pkginfo.bin: %s\n", strerror(errno));
        fclose(fp);
        return -1;
    }

    fclose(fp);
    if (g_dvf_verbose_mode) {
        fprintf(stderr, "[DVF-DB] Successfully serialized package info to %s\n", bin_path);
    }
    return 0;
}

int util_dvf_load_pkg_info(const char *name, const char *version, const char *release, const char *arch, DvfPkgInfo *pkg) {
    char bin_path[PATH_MAX];
    FILE *fp;

    if (!name || !version || !release || !arch || !pkg) return -1;

    snprintf(bin_path, sizeof(bin_path), "%s/%s-%s-%s.%s/pkginfo.bin",
             g_dvf_library_path ? g_dvf_library_path : "/srv/lib/util_dvf_dir/util_dvf_db",
             name, version, release, arch);

    fp = fopen(bin_path, "rb");
    if (!fp) {
        return -1; /* Not found */
    }

    if (fread(pkg, sizeof(DvfPkgInfo), 1, fp) != 1) {
        fclose(fp);
        return -1;
    }

    fclose(fp);
    return 0;
}

int util_dvf_sync_host_rpm_db(void) {
    FILE *fp;
    char path[1024];
    int synced_count = 0;

    /* Use system rpm query command for universal, robust host rpm database syncing across all backend versions (sqlite or bdb) */
    fp = popen("rpm -qa --qf '%{NAME}|%{VERSION}|%{RELEASE}|%{ARCH}\\n' 2>/dev/null", "r");
    if (!fp) {
        fprintf(stderr, "[DVF-WARNING] 'rpm' command not available or failed to execute (running in standalone/chroot mode).\n");
        return 0;
    }

    if (g_dvf_verbose_mode) {
        fprintf(stderr, "[DVF-DB] Synchronizing host RPM database via rpm query...\n");
    }

    while (fgets(path, sizeof(path), fp) != NULL) {
        char *name, *version, *release, *arch;
        DvfPkgInfo pkg;
        size_t len;

        /* Remove trailing newline */
        path[strcspn(path, "\r\n")] = '\0';
        if (path[0] == '\0') continue;

        /* Parse format: name|version|release|arch */
        name = path;
        version = strchr(name, '|');
        if (!version) continue;
        *version++ = '\0';

        release = strchr(version, '|');
        if (!release) continue;
        *release++ = '\0';

        arch = strchr(release, '|');
        if (!arch) continue;
        *arch++ = '\0';

        memset(&pkg, 0, sizeof(DvfPkgInfo));

        len = strlen(name);
        if (len >= sizeof(pkg.name)) len = sizeof(pkg.name) - 1;
        memcpy(pkg.name, name, len);
        pkg.name[len] = '\0';

        len = strlen(version);
        if (len >= sizeof(pkg.version)) len = sizeof(pkg.version) - 1;
        memcpy(pkg.version, version, len);
        pkg.version[len] = '\0';

        len = strlen(release);
        if (len >= sizeof(pkg.release)) len = sizeof(pkg.release) - 1;
        memcpy(pkg.release, release, len);
        pkg.release[len] = '\0';

        len = strlen(arch);
        if (len >= sizeof(pkg.arch)) len = sizeof(pkg.arch) - 1;
        memcpy(pkg.arch, arch, len);
        pkg.arch[len] = '\0';

        if (pkg.name[0] != '\0') {
            util_dvf_save_pkg_info(&pkg);
            synced_count++;
        }
    }

    pclose(fp);

    printf("Host RPM database successfully synchronized. Synchronized %d packages.\n", synced_count);
    return 0;
}
