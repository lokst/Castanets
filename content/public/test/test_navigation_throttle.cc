// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/public/test/test_navigation_throttle.h"

#include "base/bind.h"
#include "base/optional.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/navigation_handle.h"

namespace content {

TestNavigationThrottle::TestNavigationThrottle(NavigationHandle* handle)
    : NavigationThrottle(handle), weak_ptr_factory_(this) {}

TestNavigationThrottle::~TestNavigationThrottle() {}

NavigationThrottle::ThrottleCheckResult
TestNavigationThrottle::WillStartRequest() {
  return ProcessMethod(WILL_START_REQUEST);
}

NavigationThrottle::ThrottleCheckResult
TestNavigationThrottle::WillRedirectRequest() {
  return ProcessMethod(WILL_REDIRECT_REQUEST);
}

NavigationThrottle::ThrottleCheckResult
TestNavigationThrottle::WillProcessResponse() {
  return ProcessMethod(WILL_PROCESS_RESPONSE);
}

const char* TestNavigationThrottle::GetNameForLogging() {
  return "TestNavigationThrottle";
}

int TestNavigationThrottle::GetCallCount(ThrottleMethod method) {
  return method_properties_[method].call_count;
}

void TestNavigationThrottle::SetResponse(
    ThrottleMethod method,
    ResultSynchrony synchrony,
    NavigationThrottle::ThrottleCheckResult result) {
  CHECK_LT(method, NUM_THROTTLE_METHODS) << "Invalid throttle method";
  if (synchrony == ASYNCHRONOUS) {
    DCHECK(result.action() == NavigationThrottle::CANCEL_AND_IGNORE ||
           result.action() == NavigationThrottle::CANCEL ||
           result.action() == NavigationThrottle::BLOCK_REQUEST_AND_COLLAPSE)
        << "Invalid result for asynchronous response. Must have a valid action "
           "for CancelDeferredNavigation().";
  }
  method_properties_[method].synchrony = synchrony;
  method_properties_[method].result = result;
}

void TestNavigationThrottle::SetResponseForAllMethods(
    ResultSynchrony synchrony,
    NavigationThrottle::ThrottleCheckResult result) {
  for (size_t method = 0; method < NUM_THROTTLE_METHODS; method++) {
    SetResponse(static_cast<ThrottleMethod>(method), synchrony, result);
  }
}

void TestNavigationThrottle::SetCallback(ThrottleMethod method,
                                         base::Closure callback) {
  method_properties_[method].callback = callback;
}

void TestNavigationThrottle::OnWillRespond() {}

NavigationThrottle::ThrottleCheckResult TestNavigationThrottle::ProcessMethod(
    ThrottleMethod method) {
  method_properties_[method].call_count++;
  if (!method_properties_[method].callback.is_null())
    method_properties_[method].callback.Run();

  NavigationThrottle::ThrottleCheckResult result =
      method_properties_[method].result;
  if (method_properties_[method].synchrony == ASYNCHRONOUS) {
    DCHECK_CURRENTLY_ON(BrowserThread::UI);
    BrowserThread::PostTask(
        BrowserThread::UI, FROM_HERE,
        base::BindOnce(&TestNavigationThrottle::TestNavigationThrottle::
                           CancelAsynchronously,
                       weak_ptr_factory_.GetWeakPtr(), result));
    return NavigationThrottle::DEFER;
  }
  OnWillRespond();
  return result;
}

void TestNavigationThrottle::CancelAsynchronously(
    NavigationThrottle::ThrottleCheckResult result) {
  OnWillRespond();
  CancelDeferredNavigation(result);
}

TestNavigationThrottle::MethodProperties::MethodProperties() {}
TestNavigationThrottle::MethodProperties::~MethodProperties() {}

}  // namespace content
