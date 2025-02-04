// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/process/process_handle.h"

#include <tlhelp32.h>
#include <windows.h>

#include "base/win/scoped_handle.h"
#include "base/win/windows_version.h"

namespace base {

ProcessId GetCurrentProcId() {
  return ::GetCurrentProcessId();
}

ProcessHandle GetCurrentProcessHandle() {
  return ::GetCurrentProcess();
}

ProcessId GetProcId(ProcessHandle process) {
  // This returns 0 if we have insufficient rights to query the process handle.
#if defined(CASTANETS)
  if (int(process) == 7777)
    return 7777;
#endif
  return GetProcessId(process);
}

ProcessId GetParentProcessId(ProcessHandle process) {
  ProcessId child_pid = GetProcId(process);
  PROCESSENTRY32 process_entry;
  process_entry.dwSize = sizeof(PROCESSENTRY32);

  win::ScopedHandle snapshot(CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0));
  if (snapshot.IsValid() && Process32First(snapshot.Get(), &process_entry)) {
    do {
      if (process_entry.th32ProcessID == child_pid)
        return process_entry.th32ParentProcessID;
    } while (Process32Next(snapshot.Get(), &process_entry));
  }

  // TODO(zijiehe): To match other platforms, -1 (UINT32_MAX) should be returned
  // if |child_id| cannot be found in the |snapshot|.
  return 0u;
}

}  // namespace base
