// Copyright (c) 2021 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// you can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef BRAVE_BROWSER_UI_READER_MODE_BRAVE_READER_MODE_BUBBLE_CONTROLLER_H_
#define BRAVE_BROWSER_UI_READER_MODE_BRAVE_READER_MODE_BUBBLE_CONTROLLER_H_

#include "content/public/browser/web_contents_user_data.h"

namespace content {
class WebContents;
}  // namespace content

class BraveReaderModeBubble;

class BraveReaderModeBubbleController
    : public content::WebContentsUserData<BraveReaderModeBubbleController> {
 public:
  BraveReaderModeBubbleController(const BraveReaderModeBubbleController&) =
      delete;
  BraveReaderModeBubbleController& operator=(
      const BraveReaderModeBubbleController&) = delete;

  static BraveReaderModeBubbleController* Get(
      content::WebContents* web_contents);

  // Displays speedreader information
  void ShowBubble();

  // Hides speedreader information
  void HideBubble();

  // returns nullptr if no bubble corruntly shown
  BraveReaderModeBubble* reader_mode_bubble_view() const;

  // Handler for when the bubble is dismissed.
  void OnBubbleClosed();

 protected:
  explicit BraveReaderModeBubbleController(content::WebContents* web_contents);

  // fixme: OnBubbleClosed
 private:
  BraveReaderModeBubbleController();

  BraveReaderModeBubble* reader_mode_bubble_ = nullptr;
  friend class content::WebContentsUserData<BraveReaderModeBubbleController>;

  content::WebContents* web_contents_;
  WEB_CONTENTS_USER_DATA_KEY_DECL();
};

#endif  // BRAVE_BROWSER_UI_READER_MODE_BRAVE_READER_MODE_BUBBLE_CONTROLLER_H_
