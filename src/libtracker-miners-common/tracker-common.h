/*
 * Copyright (C) 2008, Nokia <ivan.frade@nokia.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 */

#ifndef __LIBTRACKER_COMMON_H__
#define __LIBTRACKER_COMMON_H__

#if !defined (__LIBTRACKER_COMMON_INSIDE__) && !defined (TRACKER_COMPILATION)
#error "only <libtracker-miners-common/tracker-common.h> must be included directly."
#endif

#include <glib.h>

#define __LIBTRACKER_COMMON_INSIDE__

#ifdef HAVE_POWER
#include "tracker-power.h"
#endif

#include "tracker-dbus.h"
#include "tracker-debug.h"
#include "tracker-domain-ontology.h"
#include "tracker-enums.h"
#include "tracker-error-report.h"
#include "tracker-file-utils.h"
#include "tracker-fts-config.h"
#include "tracker-ioprio.h"
#include "tracker-miner.h"
#include "tracker-miner-proxy.h"
#include "tracker-sched.h"
#include "tracker-seccomp.h"
#include "tracker-term-utils.h"
#include "tracker-type-utils.h"
#include "tracker-utils.h"
#include "tracker-locale.h"
#include "tracker-miners-enum-types.h"

#undef __LIBTRACKER_COMMON_INSIDE__

#endif /* __LIBTRACKER_COMMON_H__ */
