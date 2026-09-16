/******************************************************************************
 * Filename:    util_dvf_rpm.c
 * Author:      <michkochris@gmail.com>
 * Date:        2026-03-04
 * Description: Raw binary RPM header parser & DvfPkgInfo mapper implementation
 * LICENSE:     GPL v3
 ******************************************************************************/

#include "util_dvf_portable.h"
#include "util_dvf_rpm.h"
#include "util_dvf_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

/* Helper to read 32-bit big-endian integer from RPM binary blob */
static uint32_t read_be32(const unsigned char *buf) {
    uint32_t val;
    memcpy(&val, buf, sizeof(uint32_t));
    return ntohl(val);
}

int util_dvf_rpm_parse_header(const char *rpm_filepath, DvfPkgInfo *pkg_info) {
    FILE *fp;
    unsigned char lead[96];
    unsigned char magic[8];
    uint32_t index_count, store_size;
    DvfHeaderIndexEntry *indices = NULL;
    char *store = NULL;
    size_t i;
    long signature_header_start;

    if (!rpm_filepath || !pkg_info) return -1;
    memset(pkg_info, 0, sizeof(DvfPkgInfo));

    fp = fopen(rpm_filepath, "rb");
    if (!fp) {
        fprintf(stderr, "[DVF-RPM-ERROR] Failed to open RPM file: %s\n", rpm_filepath);
        return -1;
    }

    /* 1. Skip 96-byte RPM lead */
    if (fread(lead, 1, 96, fp) != 96) {
        fprintf(stderr, "[DVF-RPM-ERROR] Failed to read RPM lead\n");
        fclose(fp);
        return -1;
    }

    if (read_be32(lead) != RPM_LEAD_MAGIC) {
        fprintf(stderr, "[DVF-RPM-ERROR] Invalid RPM lead magic number\n");
        fclose(fp);
        return -1;
    }

    /* 2. Skip Signature Header */
    signature_header_start = ftell(fp);
    if (fread(magic, 1, 8, fp) != 8) {
        fclose(fp);
        return -1;
    }
    if (read_be32(magic) != RPM_HEADER_MAGIC) {
        fprintf(stderr, "[DVF-RPM-ERROR] Invalid signature header magic\n");
        fclose(fp);
        return -1;
    }

    /* Read signature header index count and store size */
    {
        unsigned char header_info[8];
        uint32_t sig_index_count, sig_store_size;
        long sig_total_size;

        if (fread(header_info, 1, 8, fp) != 8) {
            fclose(fp);
            return -1;
        }
        sig_index_count = read_be32(header_info);
        sig_store_size = read_be32(header_info + 4);

        /* Signature header size = 16 bytes header + (16 * index_count) + store_size */
        sig_total_size = 8 + 8 + (sig_index_count * 16) + sig_store_size;

        /* RPM signature headers are padded to 8-byte boundaries */
        if (sig_total_size % 8 != 0) {
            sig_total_size += (8 - (sig_total_size % 8));
        }

        fseek(fp, signature_header_start + sig_total_size, SEEK_SET);
    }

    /* 3. Read Immutable Main Header */
    if (fread(magic, 1, 8, fp) != 8) {
        fclose(fp);
        return -1;
    }
    if (read_be32(magic) != RPM_HEADER_MAGIC) {
        fprintf(stderr, "[DVF-RPM-ERROR] Invalid main header magic\n");
        fclose(fp);
        return -1;
    }

    {
        unsigned char header_info[8];
        if (fread(header_info, 1, 8, fp) != 8) {
            fclose(fp);
            return -1;
        }
        index_count = read_be32(header_info);
        store_size = read_be32(header_info + 4);
    }

    /* Allocate and read index entries */
    indices = (DvfHeaderIndexEntry *)malloc(index_count * sizeof(DvfHeaderIndexEntry));
    if (!indices) {
        fclose(fp);
        return -1;
    }

    for (i = 0; i < index_count; i++) {
        unsigned char entry_buf[16];
        if (fread(entry_buf, 1, 16, fp) != 16) {
            free(indices);
            fclose(fp);
            return -1;
        }
        indices[i].tag = read_be32(entry_buf);
        indices[i].type = read_be32(entry_buf + 4);
        indices[i].offset = read_be32(entry_buf + 8);
        indices[i].count = read_be32(entry_buf + 12);
    }

    /* Allocate and read data store */
    store = (char *)malloc(store_size);
    if (!store) {
        free(indices);
        fclose(fp);
        return -1;
    }

    if (fread(store, 1, store_size, fp) != store_size) {
        free(indices);
        free(store);
        fclose(fp);
        return -1;
    }

    /* Record payload offset where CPIO archive begins */
    pkg_info->payload_offset = ftell(fp);
    snprintf(pkg_info->compression_type, sizeof(pkg_info->compression_type), "%s", "gzip");

    /* 4. Extract critical tags into DvfPkgInfo */
    for (i = 0; i < index_count; i++) {
        uint32_t tag = indices[i].tag;
        uint32_t offset = indices[i].offset;

        if (offset >= store_size) continue;

        if (tag == RPMTAG_NAME) {
            snprintf(pkg_info->name, sizeof(pkg_info->name), "%s", store + offset);
        } else if (tag == RPMTAG_VERSION) {
            snprintf(pkg_info->version, sizeof(pkg_info->version), "%s", store + offset);
        } else if (tag == RPMTAG_RELEASE) {
            snprintf(pkg_info->release, sizeof(pkg_info->release), "%s", store + offset);
        } else if (tag == RPMTAG_ARCH) {
            snprintf(pkg_info->arch, sizeof(pkg_info->arch), "%s", store + offset);
        } else if (tag == RPMTAG_SOURCERPM) {
            snprintf(pkg_info->source_rpm, sizeof(pkg_info->source_rpm), "%s", store + offset);
        }
    }

    if (g_dvf_verbose_mode) {
        fprintf(stderr, "[DVF-RPM] Parsed RPM Header -> Name: %s, Version: %s, Release: %s, Arch: %s\n",
                pkg_info->name, pkg_info->version, pkg_info->release, pkg_info->arch);
    }

    free(indices);
    free(store);
    fclose(fp);
    return 0;
}
