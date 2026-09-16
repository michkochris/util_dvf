/******************************************************************************
 * Filename:    util_dvf_fsm.h
 * Author:      <michkochris@gmail.com>
 * Date:        2026-03-04
 * Description: FSM transaction engine declarations for util_dvf
 * LICENSE:     GPL v3
 ******************************************************************************/

#ifndef UTIL_DVF_FSM_H
#define UTIL_DVF_FSM_H

#include "util_dvf_portable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    DVF_STATE_IDLE = 0,
    DVF_STATE_RESOLVING,
    DVF_STATE_VERIFYING,
    DVF_STATE_TRANSACTION_PREP,
    DVF_STATE_EXECUTING,
    DVF_STATE_COMMITTED,
    DVF_STATE_ROLLED_BACK,
    DVF_STATE_CLEANUP,
    DVF_STATE_FAILED
} DvfState;

typedef enum {
    DVF_ACTION_CREATE = 0,
    DVF_ACTION_OVERWRITE,
    DVF_ACTION_DELETE
} DvfJournalAction;

typedef struct DvfJournalEntry {
    DvfJournalAction action;
    char target_path[PATH_MAX];
    char backup_path[PATH_MAX];
    struct DvfJournalEntry *next;
} DvfJournalEntry;

typedef struct {
    DvfState state;
    char package_name[256];
    char version[128];
    char staging_dir[PATH_MAX];
    DvfJournalEntry *journal_head;
    int journal_count;
    bool committed;
} DvfTransactionContext;

int util_dvf_fsm_init(DvfTransactionContext *ctx, const char *pkg_name, const char *version);
const char *util_dvf_state_to_string(DvfState state);
DvfState util_dvf_fsm_transition(DvfTransactionContext *ctx, DvfState next_state);
DvfState util_dvf_execute_transaction(DvfTransactionContext *ctx);

#ifdef __cplusplus
}
#endif

#endif /* UTIL_DVF_FSM_H */
