// Copyright (c) 2021 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// you can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef BRAVE_BROWSER_UI_VIEWS_READER_MODE_BRAVE_READER_MODE_BUBBLE_H_
#define BRAVE_BROWSER_UI_VIEWS_READER_MODE_BRAVE_READER_MODE_BUBBLE_H_

#include "brave/browser/ui/reader_mode/brave_reader_mode_bubble_controller.h"
#include "chrome/browser/ui/views/location_bar/location_bar_bubble_delegate_view.h"
#include "content/public/browser/web_contents.h"
#include "ui/views/metadata/metadata_header_macros.h"

namespace views {
class View;
class Label;
}  // namespace views

class BraveReaderModeBubbleController;

class BraveReaderModeBubble : public LocationBarBubbleDelegateView {
 public:
  METADATA_HEADER(BraveReaderModeBubble);
  BraveReaderModeBubble(views::View* anchor_view,
                        content::WebContents* web_contents,
                        BraveReaderModeBubbleController* controller);
  BraveReaderModeBubble(const BraveReaderModeBubble&) = delete;
  BraveReaderModeBubble& operator=(const BraveReaderModeBubble&) = delete;
  // fixme: destructor

  void Show();
  void Hide();

 private:
  // LocationBarBubbleDelegateView:
  void WindowClosing() override;
  bool ShouldShowCloseButton() const override;

  // views::BubbleDialogDelegateView:
  void Init() override;

  BraveReaderModeBubbleController* controller_;  // weak.

  views::Label* label_ = nullptr;
};

#endif  // BRAVE_BROWSER_UI_VIEWS_READER_MODE_BRAVE_READER_MODE_BUBBLE_H_
