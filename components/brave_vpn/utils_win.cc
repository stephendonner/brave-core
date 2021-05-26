/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/components/brave_vpn/utils_win.h"

#include <ras.h>
#include <raserror.h>
#include <stdio.h>

#include "base/logging.h"

namespace brave_vpn {

namespace {

// https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-formatmessage
void PrintSystemError(DWORD error) {
  DWORD cBufSize = 512;
  TCHAR lpszErrorString[512];

  DWORD bufLen =
      FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                    NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                    lpszErrorString, cBufSize, NULL);
  if (bufLen) {
    LOG(ERROR) << lpszErrorString;
  }
}

}  // namespace

// https://docs.microsoft.com/en-us/windows/win32/api/ras/nf-ras-rasgeterrorstringa
void PrintRasError(DWORD error) {
  DWORD cBufSize = 512;
  TCHAR lpszErrorString[512];

  if (error > RASBASE && error < RASBASEEND) {
    if (RasGetErrorString(error, lpszErrorString, cBufSize) == ERROR_SUCCESS) {
      LOG(ERROR) << lpszErrorString;
      return;
    }
  }

  PrintSystemError(error);
}

}  // namespace brave_vpn
