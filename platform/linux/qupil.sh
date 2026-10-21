#!/bin/sh
# SPDX-FileCopyrightText: 2006-2026 Felix Hammer
# SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#
# Starts Qupil from the folder this script is in, also through a link to it
# (desktop, start menu). Arguments are passed on (e.g. --lang de).

dir=$(dirname "$(readlink -f "$0")")
exec "$dir/bin/qupil" "$@"
