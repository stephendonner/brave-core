/* Copyright 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/components/debounce/browser/debounce_throttle.h"

#include "brave/components/debounce/browser/debounce_service.h"
#include "services/network/public/cpp/request_mode.h"
#include "services/network/public/cpp/resource_request.h"
#include "url/gurl.h"

namespace debounce {

// static
std::unique_ptr<DebounceThrottle> DebounceThrottle::MaybeCreateThrottleFor(
    DebounceService* debounce_service) {
#if 0  // TODO(pilgrim) add runtime flag here
  if (!base::FeatureList::IsEnabled(brave_shields::features::kBraveDebounce))
    return nullptr;
#endif
  return std::make_unique<DebounceThrottle>(debounce_service);
}

DebounceThrottle::DebounceThrottle(DebounceService* debounce_service)
    : debounce_service_(debounce_service) {}

DebounceThrottle::~DebounceThrottle() = default;

void DebounceThrottle::WillStartRequest(network::ResourceRequest* request,
                                        bool* defer) {
  GURL debounced_url;
  if (debounce_service_->Debounce(request->url, &debounced_url)) {
    LOG(ERROR) << request->url << " -> " << debounced_url;
    request->url = debounced_url;
    if (!request->site_for_cookies.IsEquivalent(
            net::SiteForCookies::FromUrl(debounced_url))) {
      url::Origin debounced_site_for_cookies_origin =
          url::Origin::Create(debounced_url);
      request->request_initiator = debounced_site_for_cookies_origin;
      request->trusted_params = network::ResourceRequest::TrustedParams();
      request->trusted_params->isolation_info = net::IsolationInfo::Create(
          net::IsolationInfo::RequestType::kOther,
          debounced_site_for_cookies_origin, debounced_site_for_cookies_origin,
          net::SiteForCookies::FromOrigin(debounced_site_for_cookies_origin));
    }
    delegate_->RestartWithFlags(/* additional_load_flags */ 0);
  }
}

}  // namespace debounce
