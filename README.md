# dimse-engine

C++ **C-STORE SCU** wrapper around DCMTK for Open Health Services.

Design: [`../designs/dimse-engine.md`](../designs/dimse-engine.md)

## Prerequisites

- C++17 compiler (`g++` / `clang++`)
- CMake ≥ 3.16
- DCMTK already built and installed as **shared libraries** at `$DCMTK_LIB`

```
$DCMTK_LIB/
  include/dcmtk/...
  lib/libofstd.so
  lib/liboflog.so
  lib/libdcmdata.so
  lib/libdcmnet.so
  ...
```

## Build (does not compile DCMTK)

```bash
export DCMTK_LIB=/path/to/your/dcmtk/install
cd dimse-engine
./scripts/build.sh
```

Binary: `build/dimse-engine` (rpath points at `$DCMTK_LIB/lib`).

## Run

```bash
./build/dimse-engine \
  --host 127.0.0.1 \
  --port 11112 \
  --calling-ae OHS-SCU \
  --called-ae STORESCP \
  --file /path/to/instance.dcm
```

## Layout

| Path | Role |
|------|------|
| `include/ohs/dimse/` | Public headers (no DCMTK types) |
| `src/cstore_scu.cpp` | DCMTK `DcmStorageSCU` implementation |
| `src/main.cpp` | Phase 1 CLI harness |
| `scripts/build.sh` | CMake configure + build |
| `CMakeLists.txt` | Links shared libs from `$DCMTK_LIB` |
