/**
 * @file version.c
 * 
 * @copyright Copyright (C) 2025
 * Upstream: AKJ7/dinorunner; original copyright year retained.
 * SPDX-License-Identifier: GPL-3.0-only
 * You may use, distribute and modify this code under the GNU General
 * Public License, version 3. See LICENSE in the repository root.
 */

#include "dinorunner.h"

static const unsigned char kVersionMajor = 2;
static const unsigned char kVersionMinor = 0;
static const unsigned char kVersionPatch = 0;

unsigned char dinorunner_getversion(struct version_s* version) {
  version->major = kVersionMajor;
  version->minor = kVersionMinor;
  version->patch = kVersionPatch;
  return 1u;
}
