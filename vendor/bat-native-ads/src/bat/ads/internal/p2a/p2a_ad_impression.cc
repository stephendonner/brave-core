/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ads/internal/p2a/p2a_ad_impression.h"

#include <string>
#include <vector>

#include "base/strings/stringprintf.h"
#include "bat/ads/ad_info.h"
#include "bat/ads/internal/ad_targeting/ad_targeting_segment.h"
#include "bat/ads/internal/ad_targeting/ad_targeting_segment_util.h"
#include "bat/ads/internal/p2a/p2a.h"
#include "bat/ads/internal/string_util.h"

namespace ads {
namespace p2a {

namespace {

constexpr char kQuestionPrefix[] = "Brave.P2A.AdImpressionsPerSegment.";

std::vector<std::string> CreateAdImpressionQuestions(
    const std::string& segment) {
  std::vector<std::string> questions;

  if (!segment.empty()) {
    const std::string parent_segment = SplitSegment(segment).front();

    const std::string stripped_parent_segment =
        StripNonAlphaNumericCharacters(parent_segment);

    const std::string question = base::StringPrintf(
        "%s%s", kQuestionPrefix, stripped_parent_segment.c_str());

    questions.push_back(question);
  }

  questions.push_back("Brave.P2A.TotalAdImpressions");

  return questions;
}

}  // namespace

void RecordAdImpression(const AdInfo& ad) {
  const std::string type_as_string = std::string(ad.type);

  const std::string name =
      base::StringPrintf("%s_impression", type_as_string.c_str());

  const std::vector<std::string> questions =
      p2a::CreateAdImpressionQuestions(ad.segment);

  p2a::RecordEvent(name, questions);
}

}  // namespace p2a
}  // namespace ads
