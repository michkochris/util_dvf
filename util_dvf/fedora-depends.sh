#!/usr/bin/env bash
# fedora-depends.sh - Modular dependency installer for util_dvf (Vamped Dandified Yum)
# Supports Core RPM/DNF build requirements, SQLite host bindings, and C++ FFI suite.

BOLD='\033[1m'
RESET='\033[0m'
RED='\033[1;31m'
GREEN='\033[1;32m'
YELLOW='\033[1;33m'
CYAN='\033[1;36m'
PURPLE='\033[1;35m'
BLUE='\033[1;34m'

set -e

# Default flags
INSTALL_CORE=0
INSTALL_EXTENDED=0
INSTALL_SQLITE=0

show_help() {
    echo "Usage: $0 [options]"
    echo ""
    echo "Options:"
    echo "  --core      Install basic C build tools (gcc, make, tar, binutils, etc.)"
    echo "  --sqlite    Install SQLite development libraries for host RPM DB sync"
    echo "  --extended  Install C++ FFI, libcurl, and OpenSSL networking suite"
    echo "  --all       Install everything (default if no options provided)"
    echo "  --help      Show this help message"
}

# Parse arguments
if [[ $# -eq 0 ]]; then
    INSTALL_CORE=1
    INSTALL_SQLITE=1
    INSTALL_EXTENDED=1
else
    for arg in "$@"; do
        case $arg in
            --core)     INSTALL_CORE=1 ;;
            --sqlite)   INSTALL_SQLITE=1 ;;
            --extended) INSTALL_EXTENDED=1 ;;
            --all)      INSTALL_CORE=1; INSTALL_SQLITE=1; INSTALL_EXTENDED=1 ;;
            --help)     show_help; exit 0 ;;
            *)          echo -e "${RED}Unknown option: $arg${RESET}"; show_help; exit 1 ;;
        esac
    done
fi

echo -e "${BOLD}${CYAN}--------------------------------------------------------${RESET}"
echo -e "${BOLD}${CYAN}   util_dvf: Preparing the Fedora Ritual of Dependencies ${RESET}"
echo -e "${BOLD}${CYAN}--------------------------------------------------------${RESET}"

if [[ $INSTALL_CORE -eq 1 ]]; then
    echo -e "${BOLD}${BLUE}[shell]${RESET} Ensuring /bin/sh points to /bin/bash..."
    if [ "$(readlink /bin/sh)" != "bash" ] && [ "$(readlink /bin/sh)" != "/bin/bash" ]; then
        echo -e "${YELLOW}Switching /bin/sh from $(readlink /bin/sh) to /bin/bash...${RESET}"
        sudo rm -f /bin/sh
        sudo ln -s /bin/bash /bin/sh
    fi

    echo -e "${BOLD}${BLUE}[forge]${RESET} Gathering Core Runes (Fedora C Build Tools)..."
    sudo dnf install -y @development-tools binutils tar gzip xz gcc make bison flex gawk texinfo libtool ncurses-devel gmp-devel mpfr-devel libmpc-devel time rsync
fi

if [[ $INSTALL_SQLITE -eq 1 ]]; then
    echo -e "${BOLD}${YELLOW}[sqlite]${RESET} Invoking Host RPM SQLite Connector Dependencies..."
    sudo dnf install -y sqlite sqlite-devel
fi

if [[ $INSTALL_EXTENDED -eq 1 ]]; then
    echo -e "${BOLD}${PURPLE}[ritual]${RESET} Invoking Extended Magic (C++, libcurl, OpenSSL, & zlib)..."
    sudo dnf install -y gcc-c++ libcurl-devel openssl-devel zlib-devel
fi

echo -e "${BOLD}${GREEN}--------------------------------------------------------${RESET}"
echo -e "${BOLD}${GREEN}✅ The Fedora ritual is complete. The forge is ready.${RESET}"
echo ""
echo -e "Next steps:"
echo -e "  - Verify Environment: ${CYAN}./version-check.sh${RESET}"
echo -e "  - Build util_dvf:     ${CYAN}make${RESET}"
echo -e "${BOLD}${GREEN}--------------------------------------------------------${RESET}"
