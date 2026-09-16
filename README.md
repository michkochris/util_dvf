> [!IMPORTANT]
> **ACTIVE DEVELOPMENT:** `util_dvf` (Vamped Dandified Yum - `dvf`) is currently under active, rigorous development as a zero-dependency, dual-architecture low-level C89/90 package manager with extended C++ FFI features for Fedora/Red Hat RPM ecosystems.

# util_dvf: Vamped Dandified Yum (dvf)

`util_dvf` brings the lightning-fast, modular design principles of `runepkg` into the notoriously complex world of RPM and DNF ecosystems. Designed for embedded systems, high-performance environments, and power users, `dvf` cuts through traditional DNF bloat while maintaining absolute transactional integrity and deterministic execution.

---

## Architectural Vision

Because RPM package headers, CPIO payloads, and DNF repository metadata (`repomd.xml`, primary SQLite/XML databases) carry immense structural complexity, `util_dvf` is built from day one as a strict **Finite State Machine (FSM)** transaction engine. 

By piggybacking on `runepkg`'s proven cascading configuration patterns and binary autocompletion architecture, `util_dvf` delivers uncompromising speed, safety, and predictability.

---

## Core Components (`util_dvf/`)

- **`util_dvf_config.c` / `.h`**: Cascading configuration parser strictly bound to `/etc/dvf/dvfconfig` and environment overrides (`DVF_CONFIG_PATH`), laying the groundwork for persistent reboot-resilient state registries.
- **`util_dvf_fsm.c` / `.h`**: Strict FSM transaction engine managing atomic lifecycle stages:
  - `STATE_IDLE`
  - `STATE_RESOLVING` (dependency graphing & RPM header acquisition)
  - `STATE_VERIFYING` (GPG signature & SHA256/MD5 checksum validation)
  - `STATE_TRANSACTION_PREP` (RPM database locking & payload staging)
  - `STATE_EXECUTING` (CPIO unpacking & atomic database updates)
  - `STATE_COMMITTED` / `STATE_ROLLED_BACK` / `STATE_CLEANUP`
- **`util_dvf_cli.c` / `.h`**: Interleaved command dispatcher with early `--verbose` / `-d` debug tracing parsed before any state transition occurs, supporting commands like `install`, `remove`, `update`, `search`, `--print-config`, and `--print-config-file`.
- **`util_dvf_auto.c` / `.h`**: Memory-mapped (`mmap`) binary autocompletion index (`dvf_autocomplete.bin`) delivering instant shell autocompletion.

---

## Build Instructions

Built with strict C89 security hardening (`-std=c89 -Wall -Wextra -Wpedantic -Werror -Wshadow -Wformat=2 -Wundef -D_FORTIFY_SOURCE=2 -fstack-protector-strong -fPIE`):

```bash
make clean
make util_dvf
```

### Diagnostic Commands

- `util_dvf --print-config` — Display active path, RPM database, and repository settings.
- `util_dvf --print-config-file` — Show the absolute path to the configuration file in use.
