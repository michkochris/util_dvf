/******************************************************************************
 * Filename:    util_dvf_cli.h
 * Author:      <michkochris@gmail.com>
 * Date:        2026-03-04
 * Description: CLI dispatcher declarations for util_dvf
 * LICENSE:     GPL v3
 ******************************************************************************/

#ifndef UTIL_DVF_CLI_H
#define UTIL_DVF_CLI_H

#include "util_dvf_portable.h"

#ifdef __cplusplus
extern "C" {
#endif

int util_dvf_dispatch_args(int argc, char **argv);

#ifdef __cplusplus
}
#endif

#endif /* UTIL_DVF_CLI_H */
