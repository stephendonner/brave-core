// Copyright (c) 2019 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// you can obtain one at http://mozilla.org/MPL/2.0/.

#include "brave/browser/ui/views/reader_mode/brave_reader_mode_icon_view.h"

#include "brave/app/brave_command_ids.h"
#include "brave/app/vector_icons/vector_icons.h"
#include "brave/browser/ui/reader_mode/brave_reader_mode_bubble_controller.h"
#include "brave/browser/ui/views/reader_mode/brave_reader_mode_bubble.h"
#include "chrome/app/chrome_command_ids.h"
#include "chrome/browser/ui/views/translate/translate_bubble_view.cc"
#include "chrome/grit/generated_resources.h"
#include "ui/base/l10n/l10n_util.h"
#include "ui/views/metadata/metadata_impl_macros.h"

BraveReaderModeIconView::BraveReaderModeIconView(
    CommandUpdater* command_updater,
    IconLabelBubbleView::Delegate* icon_label_bubble_delegate,
    PageActionIconView::Delegate* page_action_icon_delegate,
    PrefService* pref_service)
    : PageActionIconView(command_updater,
                         IDC_TOGGLE_SPEEDREADER,
                         icon_label_bubble_delegate,
                         page_action_icon_delegate) {
  SetVisible(true);  // fixme
}
// pref_service_(pref_service) {}

BraveReaderModeIconView::~BraveReaderModeIconView() = default;

void BraveReaderModeIconView::UpdateImpl() {
  SetVisible(true);  // fixme: testing
}

const gfx::VectorIcon& BraveReaderModeIconView::GetVectorIcon() const {
  return kSpeedreaderIcon;
}

std::u16string BraveReaderModeIconView::GetTextForTooltipAndAccessibleName()
    const {
  return l10n_util::GetStringUTF16(GetActive() ? IDS_EXIT_DISTILLED_PAGE
                                               : IDS_DISTILL_PAGE);
}

void BraveReaderModeIconView::OnExecuting(
    PageActionIconView::ExecuteSource execute_source) {}

views::BubbleDialogDelegate* BraveReaderModeIconView::GetBubble() const {
  auto* web_contents = GetWebContents();
  if (!web_contents)
    return nullptr;

  auto* bubble_controller = BraveReaderModeBubbleController::Get(web_contents);
  if (!bubble_controller)
    return nullptr;

  return bubble_controller->reader_mode_bubble_view();
}

BEGIN_METADATA(BraveReaderModeIconView, PageActionIconView)
END_METADATA
