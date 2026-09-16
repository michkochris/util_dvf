/******************************************************************************
 * Filename:    util_dvf_config.h
 * Author:      <michkochris@gmail.com>
 * Date:        2026-03-04
 * Description: Cascading configuration declarations for util_dvf
 * LICENSE:     GPL v3
 ******************************************************************************/

#ifndef UTIL_DVF_CONFIG_H
#define UTIL_DVF_CONFIG_H

#include "util_dvf_portable.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#ifdef __cplusplus
extern "C" {
#endif

extern char *g_dvf_base_dir;
extern char *g_dvf_config_path;
extern char *g_dvf_install_dir;
extern char *g_dvf_rpm_db_path;
extern char *g_dvf_cache_dir;
extern char *g_dvf_log_dir;
extern char *g_dvf_repo_base_url;
extern bool g_dvf_verbose_mode;
extern bool g_dvf_dry_run;

int util_dvf_config_init(const char *custom_config_path);
void util_dvf_config_cleanup(void);
char *util_dvf_get_config_value(const char *filepath, const char *key);

#ifdef __cplusplus
}
#endif

#endif /* UTIL_DVF_CONFIG_H */
