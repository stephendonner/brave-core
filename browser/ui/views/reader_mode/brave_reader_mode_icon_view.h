// Copyright (c) 2019 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// you can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef BRAVE_BROWSER_UI_VIEWS_READER_MODE_BRAVE_READER_MODE_ICON_VIEW_H_
#define BRAVE_BROWSER_UI_VIEWS_READER_MODE_BRAVE_READER_MODE_ICON_VIEW_H_

#include "chrome/browser/ui/views/page_action/page_action_icon_view.h"
#include "ui/views/metadata/metadata_header_macros.h"

class PrefService;

class BraveReaderModeIconView : public PageActionIconView {
 public:
  METADATA_HEADER(BraveReaderModeIconView);
  BraveReaderModeIconView(
      CommandUpdater* command_updater,
      IconLabelBubbleView::Delegate* icon_label_bubble_delegate,
      PageActionIconView::Delegate* page_action_icon_delegate,
      PrefService* pref_service);
  BraveReaderModeIconView(const BraveReaderModeIconView&) = delete;
  BraveReaderModeIconView& operator=(const BraveReaderModeIconView&) = delete;
  ~BraveReaderModeIconView() override;

  // PageActionIconView:
  views::BubbleDialogDelegate* GetBubble() const override;
  std::u16string GetTextForTooltipAndAccessibleName() const override;
  void UpdateImpl() override;

 protected:
  // PageActionIconView:
  const gfx::VectorIcon& GetVectorIcon() const override;
  void OnExecuting(PageActionIconView::ExecuteSource execute_source) override;

 private:
  // PrefService* pref_service_;
};

#endif  // BRAVE_BROWSER_UI_VIEWS_READER_MODE_BRAVE_READER_MODE_ICON_VIEW_H_
