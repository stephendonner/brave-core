/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ads/internal/ad_pacing/inline_content_ads/inline_content_ad_pacing.h"

#include "base/rand_util.h"
#include "bat/ads/internal/logging.h"

namespace ads {
namespace inline_content_ads {

namespace {

bool ShouldPace(const CreativeInlineContentAdInfo& ad) {
  const double rand = base::RandDouble();
  if (rand <= ad.ptr) {
    return false;
  }

  BLOG(2, "Pacing ad delivery for creative instance id "
              << ad.creative_instance_id << " [Roll(" << ad.ptr << "):" << rand
              << "]");

  return true;
}

}  // namespace

CreativeInlineContentAdList PaceAds(const CreativeInlineContentAdList& ads) {
  CreativeInlineContentAdList paced_ads = ads;

  const auto iter = std::remove_if(
      paced_ads.begin(), paced_ads.end(),
      [&](const CreativeInlineContentAdInfo& ad) { return ShouldPace(ad); });

  paced_ads.erase(iter, paced_ads.end());

  // TODO(tmancey): Decouple priority filter, and implement like pacing
  // const auto priority_filter =
  //     EligibleAdsFilterFactory::Build(EligibleAdsFilter::Type::kPriority);
  // DCHECK(priority_filter);
  // return priority_filter->Apply(paced_ads);

  return paced_ads;
}

}  // namespace inline_content_ads
}  // namespace ads
