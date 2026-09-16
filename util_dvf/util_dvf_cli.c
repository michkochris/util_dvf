/******************************************************************************
 * Filename:    util_dvf_cli.c
 * Author:      <michkochris@gmail.com>
 * Date:        2026-03-04 (Synced)
 * Description: Interleaved command dispatcher with early --verbose tracing for util_dvf
 * LICENSE:     GPL v3
 ******************************************************************************/

#include "util_dvf_portable.h"
#include "util_dvf_cli.h"
#include "util_dvf_config.h"
#include "util_dvf_fsm.h"
#include "util_dvf_db.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void print_usage(const char *progname) {
    printf("Vamped Dandified Yum (util_dvf) - Dual Architecture Low-Level RPM Package Manager\n");
    printf("Usage: %s [options] <command> [packages...]\n", progname);
    printf("Options:\n");
    printf("  -v, --verbose               Enable early verbose tracing\n");
    printf("  -d, --debug                 Enable debug logging\n");
    printf("  -c, --config <path>         Specify cascading config file\n");
    printf("  --dry-run                   Simulate transaction without writing\n");
    printf("  --print-config              Print all active path and repository settings\n");
    printf("  --print-config-file         Show the path to the configuration file in use\n");
    printf("Commands:\n");
    printf("  sync                        Synchronize host RPM database state into local library cache\n");
    printf("  install <pkg>               Install RPM package\n");
    printf("  remove <pkg>                Remove RPM package\n");
    printf("  update [pkg]                Update RPM package(s)\n");
    printf("  search <query>              Search repository packages\n");
    printf("  print-config                Print all active path and repository settings\n");
    printf("  print-config-file           Show the path to the configuration file in use\n");
}

static void handle_print_config(void) {
    printf("=== util_dvf Active Configuration ===\n");
    printf("Config File Path : %s\n", g_dvf_config_path ? g_dvf_config_path : "(default / none)");
    printf("Install Dir      : %s\n", g_dvf_install_dir ? g_dvf_install_dir : "(null)");
    printf("Library Path     : %s\n", g_dvf_library_path ? g_dvf_library_path : "(null)");
    printf("RPM DB Path      : %s\n", g_dvf_rpm_db_path ? g_dvf_rpm_db_path : "(null)");
    printf("Cache Dir        : %s\n", g_dvf_cache_dir ? g_dvf_cache_dir : "(null)");
    printf("Log Dir          : %s\n", g_dvf_log_dir ? g_dvf_log_dir : "(null)");
    printf("Repo Base URL    : %s\n", g_dvf_repo_base_url ? g_dvf_repo_base_url : "(null)");
}

static void handle_print_config_file(void) {
    if (g_dvf_config_path) {
        printf("%s\n", g_dvf_config_path);
    } else {
        printf("No configuration file active (using built-in defaults).\n");
    }
}

int util_dvf_dispatch_args(int argc, char **argv) {
    int i;
    char *config_path = NULL;
    const char *action = NULL;
    const char *target_pkg = NULL;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
            g_dvf_verbose_mode = true;
            fprintf(stderr, "[DVF-TRACE] Early verbose mode enabled\n");
        } else if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--debug") == 0) {
            g_dvf_verbose_mode = true;
            fprintf(stderr, "[DVF-DEBUG] Early debug mode enabled\n");
        } else if ((strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--config") == 0) && i + 1 < argc) {
            config_path = argv[++i];
        } else if (strcmp(argv[i], "--dry-run") == 0) {
            g_dvf_dry_run = true;
        } else if (strcmp(argv[i], "--print-config") == 0) {
            /* Handled early or in dispatch */
        } else if (strcmp(argv[i], "--print-config-file") == 0) {
            /* Handled early or in dispatch */
        }
    }

    if (util_dvf_config_init(config_path) != 0) {
        fprintf(stderr, "[DVF-ERROR] Failed to initialize configuration\n");
        return 1;
    }

    /* Check flags that trigger direct print & exit */
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--print-config") == 0) {
            handle_print_config();
            util_dvf_config_cleanup();
            return 0;
        } else if (strcmp(argv[i], "--print-config-file") == 0) {
            handle_print_config_file();
            util_dvf_config_cleanup();
            return 0;
        }
    }

    if (g_dvf_verbose_mode) {
        fprintf(stderr, "[DVF-CONFIG] Install Dir: %s\n", g_dvf_install_dir ? g_dvf_install_dir : "(null)");
        fprintf(stderr, "[DVF-CONFIG] Library Path: %s\n", g_dvf_library_path ? g_dvf_library_path : "(null)");
        fprintf(stderr, "[DVF-CONFIG] RPM DB Path: %s\n", g_dvf_rpm_db_path ? g_dvf_rpm_db_path : "(null)");
        fprintf(stderr, "[DVF-CONFIG] Repo Base URL: %s\n", g_dvf_repo_base_url ? g_dvf_repo_base_url : "(null)");
    }

    for (i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            if ((strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--config") == 0) && i + 1 < argc) {
                i++;
            }
            continue;
        }
        if (!action) {
            action = argv[i];
        } else if (!target_pkg) {
            target_pkg = argv[i];
        }
    }

    if (!action) {
        print_usage(argv[0]);
        util_dvf_config_cleanup();
        return 0;
    }

    if (strcmp(action, "print-config") == 0) {
        handle_print_config();
    } else if (strcmp(action, "print-config-file") == 0) {
        handle_print_config_file();
    } else if (strcmp(action, "sync") == 0) {
        if (g_dvf_verbose_mode) {
            fprintf(stderr, "[DVF-SYNC] Starting host RPM database synchronization...\n");
        }
        if (util_dvf_sync_host_rpm_db() == 0) {
            /* Handled in sync func */
        } else {
            fprintf(stderr, "[DVF-ERROR] Host RPM synchronization failed.\n");
            util_dvf_config_cleanup();
            return 1;
        }
    } else if (strcmp(action, "install") == 0 || strcmp(action, "remove") == 0 || strcmp(action, "update") == 0) {
        DvfTransactionContext ctx;
        if (!target_pkg) {
            target_pkg = "unknown-package";
        }
        if (g_dvf_verbose_mode) {
            fprintf(stderr, "[DVF-FSM] Initializing transaction for action '%s', package '%s'\n", action, target_pkg);
        }
        if (util_dvf_fsm_init(&ctx, target_pkg, "1.0.0") == 0) {
            util_dvf_execute_transaction(&ctx);
        }
    } else if (strcmp(action, "search") == 0) {
        printf("Searching repositories for: %s\n", target_pkg ? target_pkg : "(all)");
    } else {
        fprintf(stderr, "[DVF-ERROR] Unknown command: %s\n", action);
        print_usage(argv[0]);
        util_dvf_config_cleanup();
        return 1;
    }

    util_dvf_config_cleanup();
    return 0;
}

int main(int argc, char **argv) {
    return util_dvf_dispatch_args(argc, argv);
}
