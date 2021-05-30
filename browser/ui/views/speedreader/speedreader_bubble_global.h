/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_BROWSER_UI_VIEWS_SPEEDREADER_SPEEDREADER_BUBBLE_GLOBAL_H_
#define BRAVE_BROWSER_UI_VIEWS_SPEEDREADER_SPEEDREADER_BUBBLE_GLOBAL_H_

#include <memory>

#include "brave/browser/ui/speedreader/speedreader_bubble_view.h"
#include "chrome/browser/ui/views/location_bar/location_bar_bubble_delegate_view.h"
#include "content/public/browser/web_contents.h"
#include "ui/views/controls/button/md_text_button.h"
#include "ui/views/metadata/metadata_header_macros.h"

namespace content {
class WebContents;
}  // namespace content

namespace ui {
class Event;
}  // namespace ui

namespace views {
class View;
class Label;
class Link;
class MdTextButton;
class ToggleButton;
}  // namespace views

class SpeedreaderBubbleController;

class SpeedreaderBubbleGlobal : public SpeedreaderBubbleView,
                                public LocationBarBubbleDelegateView {
 public:
  METADATA_HEADER(SpeedreaderBubbleGlobal);
  SpeedreaderBubbleGlobal(views::View* anchor_view,
                          content::WebContents* web_contents,
                          SpeedreaderBubbleController* controller);
  SpeedreaderBubbleGlobal(const SpeedreaderBubbleGlobal&) = delete;
  SpeedreaderBubbleGlobal& operator=(const SpeedreaderBubbleGlobal&) = delete;
  ~SpeedreaderBubbleGlobal() override = default;

  // SpeedreaderBubbleView:
  void Show() override;
  void Hide() override;

 private:
  // LocationBarBubbleDelegateView:
  void WindowClosing() override;
  bool ShouldShowCloseButton() const override;

  // views::BubbleDialogDelegateView:
  void Init() override;

  // views::View
  gfx::Size CalculatePreferredSize() const override;

  void OnButtonPressed(const ui::Event& event);
  void OnLinkClicked(const ui::Event& event);

  content::WebContents* web_contents_;
  SpeedreaderBubbleController* controller_;  // weak.

  // fixme: unique_ptr?
  views::Label* site_title_label_ = nullptr;
  views::ToggleButton* site_toggle_button_ = nullptr;
};

#endif  // BRAVE_BROWSER_UI_VIEWS_SPEEDREADER_SPEEDREADER_BUBBLE_GLOBAL_H_
