// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/android/build_info.h"
#include "base/android/jni_android.h"
#include "base/android/scoped_java_ref.h"
#include "base/metrics/histogram_macros.h"
#include "jni/DecoderService_jni.h"
#include "sandbox/linux/seccomp-bpf-helpers/seccomp_starter_android.h"
#include "sandbox/sandbox_features.h"

#if BUILDFLAG(USE_SECCOMP_BPF)
#include "base/memory/ptr_util.h"
#include "sandbox/linux/seccomp-bpf-helpers/baseline_policy_android.h"
#endif

void InitializePhotoPickerSandbox(
    JNIEnv* env,
    const base::android::JavaParamRef<jclass>& jcaller) {
  auto* info = base::android::BuildInfo::GetInstance();
  sandbox::SeccompStarterAndroid starter(info->sdk_int(), info->device());

#if BUILDFLAG(USE_SECCOMP_BPF)
  // The policy compiler is only available if USE_SECCOMP_BPF is enabled.
  starter.set_policy(base::MakeUnique<sandbox::BaselinePolicyAndroid>());
#endif
  starter.StartSandbox();

  UMA_HISTOGRAM_ENUMERATION("Android.SeccompStatus.PhotoPickerSandbox",
                            starter.status(),
                            sandbox::SeccompSandboxStatus::STATUS_MAX);
}
