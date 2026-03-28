#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${ROOT_DIR}/build"
DIST_DIR="${ROOT_DIR}/dist"
BIN_NAME="anker-powerconf-c200-linux-tools"
VERSION="${1:-}"
TARGET_ARCH="${2:-}"
CC_BIN="${3:-${CC:-cc}}"

if [[ -z "${VERSION}" || -z "${TARGET_ARCH}" ]]; then
  echo "usage: $0 <version> <amd64|arm64> [compiler]" >&2
  exit 2
fi

mkdir -p "${DIST_DIR}"
mkdir -p "${BUILD_DIR}"

make -C "${ROOT_DIR}" clean
make -C "${ROOT_DIR}" test
make -C "${ROOT_DIR}" CC="${CC_BIN}"

PACKAGE_ROOT="${DIST_DIR}/${BIN_NAME}-${VERSION}-linux-${TARGET_ARCH}"
rm -rf "${PACKAGE_ROOT}" "${PACKAGE_ROOT}.tar.gz" "${PACKAGE_ROOT}.tar.gz.sha256"
mkdir -p "${PACKAGE_ROOT}"

install -m 0755 "${BUILD_DIR}/${BIN_NAME}" "${PACKAGE_ROOT}/${BIN_NAME}"
install -m 0644 "${ROOT_DIR}/README.md" "${PACKAGE_ROOT}/README.md"
install -m 0644 "${ROOT_DIR}/LICENSE" "${PACKAGE_ROOT}/LICENSE"

tar -C "${DIST_DIR}" -czf "${PACKAGE_ROOT}.tar.gz" \
  "$(basename "${PACKAGE_ROOT}")"

(cd "${DIST_DIR}" && sha256sum "$(basename "${PACKAGE_ROOT}").tar.gz" > "$(basename "${PACKAGE_ROOT}").tar.gz.sha256")
