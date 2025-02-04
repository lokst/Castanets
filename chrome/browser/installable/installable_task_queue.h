// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_INSTALLABLE_INSTALLABLE_TASK_QUEUE_H_
#define CHROME_BROWSER_INSTALLABLE_INSTALLABLE_TASK_QUEUE_H_

#include "chrome/browser/installable/installable_data.h"
#include "chrome/browser/installable/installable_params.h"

#include "base/callback.h"
#include "base/gtest_prod_util.h"

using InstallableTask = std::pair<InstallableParams, InstallableCallback>;

// InstallableTaskQueue keeps track of pending tasks.
class InstallableTaskQueue {
 public:
  InstallableTaskQueue();
  ~InstallableTaskQueue();

  // Adds task to the end of the active list of tasks to be processed.
  void Add(InstallableTask task);

  // Moves the current task from the main to the paused list.
  void PauseCurrent();

  // Moves all paused tasks to the main list.
  void UnpauseAll();

  // Reports whether there are any tasks in the main list.
  bool HasCurrent() const;

  // Reports whether there are any tasks in the paused list.
  bool HasPaused() const;

  // Returns the currently active task.
  InstallableTask& Current();

  // Advances to the next task.
  void Next();

  // Clears all tasks from the main and paused list.
  void Reset();

 private:
  // The list of <params, callback> pairs that have come from a call to
  // InstallableManager::GetData.
  std::vector<InstallableTask> tasks_;

  // Tasks which are waiting indefinitely for a service worker to be detected.
  std::vector<InstallableTask> paused_tasks_;

  friend class InstallableManagerBrowserTest;
  FRIEND_TEST_ALL_PREFIXES(InstallableManagerBrowserTest,
                           CheckLazyServiceWorkerPassesWhenWaiting);

  FRIEND_TEST_ALL_PREFIXES(InstallableManagerBrowserTest,
                           CheckLazyServiceWorkerNoFetchHandlerFails);
};

#endif  // CHROME_BROWSER_INSTALLABLE_INSTALLABLE_TASK_QUEUE_H_
