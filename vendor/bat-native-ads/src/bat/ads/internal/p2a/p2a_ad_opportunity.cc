/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ads/internal/p2a/p2a_ad_opportunity.h"

#include <string>
#include <vector>

#include "base/strings/stringprintf.h"
#include "bat/ads/ad_type.h"
#include "bat/ads/internal/ad_targeting/ad_targeting_segment_util.h"
#include "bat/ads/internal/p2a/p2a.h"
#include "bat/ads/internal/string_util.h"

namespace ads {
namespace p2a {

namespace {

constexpr char kQuestionPrefix[] = "Brave.P2A.AdOpportunitiesPerSegment.";

std::vector<std::string> CreateAdOpportunityQuestions(
    const std::vector<std::string>& segments) {
  std::vector<std::string> questions;

  std::vector<std::string> parent_segments = GetParentSegments(segments);

  for (auto& segment : parent_segments) {
    const std::string stripped_segment =
        StripNonAlphaNumericCharacters(segment);

    const std::string question =
        base::StringPrintf("%s%s", kQuestionPrefix, stripped_segment.c_str());

    questions.push_back(question);
  }

  questions.push_back("Brave.P2A.TotalAdOpportunities");

  return questions;
}

}  // namespace

void RecordAdOpportunityForSegments(const AdType& ad_type,
                                    const SegmentList& segments) {
  const std::string type_as_string = std::string(ad_type);

  const std::string name =
      base::StringPrintf("%s_opportunity", type_as_string.c_str());

  const std::vector<std::string> questions =
      p2a::CreateAdOpportunityQuestions(segments);

  p2a::RecordEvent(name, questions);
}

}  // namespace p2a
}  // namespace ads
