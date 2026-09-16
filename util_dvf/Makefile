# ******************************************************************************
# * Filename:    Makefile
# * Author:      <michkochris@gmail.com>
# * Date:        2026-03-04
# * Description: Makefile for util_dvf (Vamped Dandified Yum) - Dual Architecture Low-Level RPM Package Manager
# * LICENSE:     GPL v3
# ******************************************************************************/

CC      ?= gcc
CXX     ?= g++
V       ?= 0

# --- Installation Variables ---
DESTDIR ?=
PREFIX  ?= /usr
ETCDIR  ?= /etc

# --- Strict Flags and Security Hardening ---
BASE_CFLAGS   = -Wall -Wextra -Wpedantic -Werror -Wshadow -Wformat=2 -Wundef \
                -std=c89 -D_GNU_SOURCE -D_XOPEN_SOURCE=700 -g -O2 \
                -D_FORTIFY_SOURCE=2 -fstack-protector-strong -fPIE -MMD -MP \
                -pthread -fdiagnostics-color=always
BASE_CXXFLAGS = -Wall -Wextra -Wpedantic -Werror -Wshadow -Wformat=2 -Wundef \
                -std=c++17 -D_GNU_SOURCE -g -O2 -D_FORTIFY_SOURCE=2 \
                -fstack-protector-strong -fPIE -MMD -MP -pthread -fdiagnostics-color=always
BASE_LDFLAGS  = -pthread -pie -Wl,-z,relro,-z,now -Wl,-z,noexecstack

CFLAGS   := $(BASE_CFLAGS) $(CFLAGS)
CXXFLAGS := $(BASE_CXXFLAGS) $(CXXFLAGS)
LDFLAGS  := $(BASE_LDFLAGS) $(LDFLAGS)

TARGET = util_dvf
C_SOURCES = util_dvf_cli.c util_dvf_config.c util_dvf_auto.c util_dvf_fsm.c
OBJS = $(C_SOURCES:.c=.o)
HEADERS = util_dvf_portable.h util_dvf_config.h util_dvf_auto.h util_dvf_cli.h util_dvf_fsm.h

all: $(TARGET)

util_dvf: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) *.o *.d

install: $(TARGET)
	install -d $(DESTDIR)$(PREFIX)/bin
	install -m 755 $(TARGET) $(DESTDIR)$(PREFIX)/bin/$(TARGET)
	install -d $(DESTDIR)$(ETCDIR)/dvf
	if [ -f dvfconfig ]; then install -m 644 dvfconfig $(DESTDIR)$(ETCDIR)/dvf/dvfconfig; fi

.PHONY: all util_dvf clean install
