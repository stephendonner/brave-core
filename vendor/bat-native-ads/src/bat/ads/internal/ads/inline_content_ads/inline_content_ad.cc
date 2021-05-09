/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ads/internal/ads/inline_content_ads/inline_content_ad.h"

#include "bat/ads/inline_content_ad_info.h"
#include "bat/ads/internal/ad_events/inline_content_ads/inline_content_ad_event_factory.h"
#include "bat/ads/internal/ads/fire_event_util.h"
#include "bat/ads/internal/ads/inline_content_ads/inline_content_ad_frequency_capping.h"
#include "bat/ads/internal/bundle/creative_inline_content_ad_info.h"
#include "bat/ads/internal/database/tables/ad_events_database_table.h"
#include "bat/ads/internal/database/tables/creative_inline_content_ads_database_table.h"
#include "bat/ads/internal/logging.h"

namespace ads {

namespace {

InlineContentAdInfo CreateInlineContentAd(
    const std::string& uuid,
    const CreativeInlineContentAdInfo& ad) {
  InlineContentAdInfo inline_content_ad;

  inline_content_ad.type = AdType::kInlineContentAd;
  inline_content_ad.uuid = uuid;
  inline_content_ad.creative_instance_id = ad.creative_instance_id;
  inline_content_ad.creative_set_id = ad.creative_set_id;
  inline_content_ad.campaign_id = ad.campaign_id;
  inline_content_ad.advertiser_id = ad.advertiser_id;
  inline_content_ad.segment = ad.segment;
  inline_content_ad.target_url = ad.target_url;
  inline_content_ad.title = ad.title;
  inline_content_ad.description = ad.description;

  return inline_content_ad;
}

}  // namespace

InlineContentAd::InlineContentAd() = default;

InlineContentAd::~InlineContentAd() = default;

void InlineContentAd::AddObserver(InlineContentAdObserver* observer) {
  DCHECK(observer);
  observers_.AddObserver(observer);
}

void InlineContentAd::RemoveObserver(InlineContentAdObserver* observer) {
  DCHECK(observer);
  observers_.RemoveObserver(observer);
}

void InlineContentAd::FireEvent(const std::string& uuid,
                                const std::string& creative_instance_id,
                                const InlineContentAdEventType event_type) {
  if (uuid.empty() || creative_instance_id.empty()) {
    BLOG(1, "Failed to fire inline content ad event for uuid "
                << uuid << " and creative instance id "
                << creative_instance_id);

    NotifyInlineContentAdEventFailed(uuid, creative_instance_id, event_type);

    return;
  }

  database::table::CreativeInlineContentAds database_table;
  database_table.GetForCreativeInstanceId(
      creative_instance_id,
      [=](const Result result, const std::string& creative_instance_id,
          const CreativeInlineContentAdInfo& creative_inline_content_ad) {
        if (result != SUCCESS) {
          BLOG(1, "Failed to fire inline content ad event for uuid");

          NotifyInlineContentAdEventFailed(uuid, creative_instance_id,
                                           event_type);

          return;
        }

        const InlineContentAdInfo ad =
            CreateInlineContentAd(uuid, creative_inline_content_ad);

        FireEvent(ad, uuid, creative_instance_id, event_type);
      });
}

///////////////////////////////////////////////////////////////////////////////

void InlineContentAd::FireEvent(const InlineContentAdInfo& ad,
                                const std::string& uuid,
                                const std::string& creative_instance_id,
                                const InlineContentAdEventType event_type) {
  database::table::AdEvents database_table;
  database_table.GetAll([=](const Result result, const AdEventList& ad_events) {
    if (result != Result::SUCCESS) {
      BLOG(1, "inline content ad: Failed to get ad events");

      NotifyInlineContentAdEventFailed(uuid, creative_instance_id, event_type);

      return;
    }

    if (!ShouldFireEvent(ad, ad_events)) {
      BLOG(1, "inline content ad: Not allowed");

      NotifyInlineContentAdEventFailed(uuid, creative_instance_id, event_type);

      return;
    }

    const auto ad_event = inline_content_ads::AdEventFactory::Build(event_type);
    ad_event->FireEvent(ad);

    NotifyInlineContentAdEvent(ad, event_type);
  });
}

void InlineContentAd::NotifyInlineContentAdEvent(
    const InlineContentAdInfo& ad,
    const InlineContentAdEventType event_type) {
  switch (event_type) {
    case InlineContentAdEventType::kViewed: {
      NotifyInlineContentAdViewed(ad);
      break;
    }

    case InlineContentAdEventType::kClicked: {
      NotifyInlineContentAdClicked(ad);
      break;
    }
  }
}

void InlineContentAd::NotifyInlineContentAdViewed(
    const InlineContentAdInfo& ad) {
  for (InlineContentAdObserver& observer : observers_) {
    observer.OnInlineContentAdViewed(ad);
  }
}

void InlineContentAd::NotifyInlineContentAdClicked(
    const InlineContentAdInfo& ad) {
  for (InlineContentAdObserver& observer : observers_) {
    observer.OnInlineContentAdClicked(ad);
  }
}

void InlineContentAd::NotifyInlineContentAdEventFailed(
    const std::string& uuid,
    const std::string& creative_instance_id,
    const InlineContentAdEventType event_type) {
  for (InlineContentAdObserver& observer : observers_) {
    observer.OnInlineContentAdEventFailed(uuid, creative_instance_id,
                                          event_type);
  }
}

}  // namespace ads
