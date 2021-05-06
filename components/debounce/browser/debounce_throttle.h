/* Copyright 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_DEBOUNCE_DEBOUNCE_THROTTLE_H_
#define BRAVE_COMPONENTS_DEBOUNCE_DEBOUNCE_THROTTLE_H_

#include "base/memory/weak_ptr.h"
#include "services/network/public/mojom/url_response_head.mojom-forward.h"
#include "third_party/blink/public/common/loader/url_loader_throttle.h"

namespace debounce {

class DebounceService;

class DebounceThrottle : public blink::URLLoaderThrottle {
 public:
  DebounceThrottle(DebounceService* debounce_service);
  ~DebounceThrottle() override;

  DebounceThrottle(const DebounceThrottle&) = delete;
  DebounceThrottle& operator=(const DebounceThrottle&) = delete;

  static std::unique_ptr<DebounceThrottle> MaybeCreateThrottleFor(
      DebounceService* debounce_service);

  // Implements blink::URLLoaderThrottle.
  void WillStartRequest(network::ResourceRequest* request,
                        bool* defer) override;

 private:
  DebounceService* debounce_service_;  // not owned
  base::WeakPtrFactory<DebounceThrottle> weak_factory_{this};
};

}  // namespace debounce

#endif  // BRAVE_COMPONENTS_DEBOUNCE_DEBOUNCE_THROTTLE_H_
