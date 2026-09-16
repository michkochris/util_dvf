/******************************************************************************
 * Filename:    util_dvf_config.c
 * Author:      <michkochris@gmail.com>
 * Date:        2026-03-04
 * Description: Cascading configuration parser for util_dvf (Strict /etc/dvf/dvfconfig)
 * LICENSE:     GPL v3
 ******************************************************************************/

#include "util_dvf_portable.h"
#include "util_dvf_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <ctype.h>

char *g_dvf_base_dir = NULL;
char *g_dvf_config_path = NULL;
char *g_dvf_install_dir = NULL;
char *g_dvf_rpm_db_path = NULL;
char *g_dvf_cache_dir = NULL;
char *g_dvf_log_dir = NULL;
char *g_dvf_repo_base_url = NULL;
bool g_dvf_verbose_mode = false;
bool g_dvf_dry_run = false;

static char *trim_whitespace(char *str) {
    char *end;
    while(isspace((unsigned char)*str)) str++;
    if(*str == 0) return str;
    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
    return str;
}

char *util_dvf_get_config_value(const char *filepath, const char *key) {
    FILE *fp;
    char line[1024];
    char *val = NULL;

    if (!filepath || !key) return NULL;
    fp = fopen(filepath, "r");
    if (!fp) return NULL;

    while (fgets(line, sizeof(line), fp)) {
        char *p = trim_whitespace(line);
        char *eq;
        if (p[0] == '#' || p[0] == '\0') continue;
        eq = strchr(p, '=');
        if (eq) {
            *eq = '\0';
            if (strcmp(trim_whitespace(p), key) == 0) {
                val = strdup(trim_whitespace(eq + 1));
                break;
            }
        }
    }
    fclose(fp);
    return val;
}

int util_dvf_config_init(const char *custom_config_path) {
    char *env_path = getenv("DVF_CONFIG_PATH");

    /* Strict cascading order: custom_config_path -> DVF_CONFIG_PATH -> /etc/dvf/dvfconfig */
    if (custom_config_path && access(custom_config_path, R_OK) == 0) {
        g_dvf_config_path = strdup(custom_config_path);
    } else if (env_path && access(env_path, R_OK) == 0) {
        g_dvf_config_path = strdup(env_path);
    } else if (access("/etc/dvf/dvfconfig", R_OK) == 0) {
        g_dvf_config_path = strdup("/etc/dvf/dvfconfig");
    }

    if (g_dvf_config_path) {
        char *v;
        v = util_dvf_get_config_value(g_dvf_config_path, "install_dir");
        g_dvf_install_dir = v ? v : strdup("/usr");

        v = util_dvf_get_config_value(g_dvf_config_path, "rpm_db_path");
        g_dvf_rpm_db_path = v ? v : strdup("/var/lib/rpm");

        v = util_dvf_get_config_value(g_dvf_config_path, "cache_dir");
        g_dvf_cache_dir = v ? v : strdup("/var/cache/dvf");

        v = util_dvf_get_config_value(g_dvf_config_path, "log_dir");
        g_dvf_log_dir = v ? v : strdup("/var/log/dvf");

        v = util_dvf_get_config_value(g_dvf_config_path, "repo_base_url");
        g_dvf_repo_base_url = v ? v : strdup("https://download.fedoraproject.org/pub/fedora/linux/");
    } else {
        g_dvf_install_dir = strdup("/usr");
        g_dvf_rpm_db_path = strdup("/var/lib/rpm");
        g_dvf_cache_dir = strdup("/var/cache/dvf");
        g_dvf_log_dir = strdup("/var/log/dvf");
        g_dvf_repo_base_url = strdup("https://download.fedoraproject.org/pub/fedora/linux/");
    }

    return 0;
}

void util_dvf_config_cleanup(void) {
    free(g_dvf_config_path);
    free(g_dvf_install_dir);
    free(g_dvf_rpm_db_path);
    free(g_dvf_cache_dir);
    free(g_dvf_log_dir);
    free(g_dvf_repo_base_url);
    g_dvf_config_path = NULL;
    g_dvf_install_dir = NULL;
    g_dvf_rpm_db_path = NULL;
    g_dvf_cache_dir = NULL;
    g_dvf_log_dir = NULL;
    g_dvf_repo_base_url = NULL;
}
