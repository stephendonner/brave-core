/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/ui/views/speedreader/speedreader_bubble_global.h"

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "base/bind.h"
#include "base/strings/utf_string_conversions.h"
#include "brave/browser/themes/theme_properties.h"
#include "brave/browser/ui/speedreader/speedreader_bubble_controller.h"
#include "brave/common/url_constants.h"
#include "brave/grit/brave_generated_resources.h"
#include "chrome/browser/ui/views/chrome_layout_provider.h"
#include "chrome/browser/ui/views/location_bar/location_bar_bubble_delegate_view.h"
#include "chrome/grit/generated_resources.h"
#include "components/strings/grit/components_strings.h"
#include "content/public/browser/page_navigator.h"
#include "content/public/browser/web_contents.h"
#include "content/public/browser/web_contents_user_data.h"
#include "content/public/browser/web_ui_controller.h"
#include "content/public/common/referrer.h"
#include "include/core/SkColor.h"
#include "ui/base/l10n/l10n_util.h"
#include "ui/base/theme_provider.h"
#include "ui/base/ui_base_types.h"
#include "ui/base/window_open_disposition.h"
#include "ui/events/event.h"
#include "ui/gfx/text_constants.h"
#include "ui/views/controls/button/md_text_button.h"
#include "ui/views/controls/button/toggle_button.h"
#include "ui/views/controls/label.h"
#include "ui/views/controls/link.h"
#include "ui/views/controls/styled_label.h"
#include "ui/views/layout/box_layout.h"
#include "ui/views/layout/grid_layout.h"
#include "ui/views/layout/layout_provider.h"
#include "ui/views/metadata/metadata_impl_macros.h"
#include "ui/views/view.h"

namespace {

constexpr int kLineHeight = 16;

constexpr char kTextSeparator[] = " ";

const std::vector<std::string> kReaderFontFamilies = {
    "Georgia",
    "Times New Roman",
    "Times"
    "serif",
};

std::unique_ptr<views::StyledLabel> BuildLabelWithEndingLink(
    const std::u16string& reg_text,
    const std::u16string& link_text,
    views::Link::ClickedCallback callback) {
  auto label = std::make_unique<views::StyledLabel>();
  std::u16string text = reg_text;
  text.append(base::ASCIIToUTF16(kTextSeparator));
  size_t default_format_offset = text.length();
  text.append(link_text);
  LOG(ERROR) << "text:" << text;
  label->SetText(text);

  // Setup styles
  views::StyledLabel::RangeStyleInfo style_link =
      views::StyledLabel::RangeStyleInfo::CreateForLink(callback);
  views::StyledLabel::RangeStyleInfo style_default;
  style_default.custom_font = gfx::FontList(
      kReaderFontFamilies, gfx::Font::NORMAL, 12, gfx::Font::Weight::NORMAL);

  // Apply styles
  label->AddStyleRange(gfx::Range(0, default_format_offset), style_default);
  label->AddStyleRange(gfx::Range(default_format_offset, text.length()),
                       style_link);
  return label;
}
}  // anonymous namespace

SpeedreaderBubbleGlobal::SpeedreaderBubbleGlobal(
    views::View* anchor_view,
    content::WebContents* web_contents,
    SpeedreaderBubbleController* controller)
    : LocationBarBubbleDelegateView(anchor_view, nullptr),
      web_contents_(web_contents),
      controller_(controller) {
  SetButtons(ui::DialogButton::DIALOG_BUTTON_NONE);
}

void SpeedreaderBubbleGlobal::Show() {
  LOG(ERROR) << "calling show on bubble!";
  ShowForReason(USER_GESTURE);
}

void SpeedreaderBubbleGlobal::Hide() {
  LOG(ERROR) << "calling hide on bubble!";
  if (controller_) {
    controller_->OnBubbleClosed();
    controller_ = nullptr;
  }
  CloseBubble();
}

gfx::Size SpeedreaderBubbleGlobal::CalculatePreferredSize() const {
  return gfx::Size(
      324, LocationBarBubbleDelegateView::CalculatePreferredSize().height());
}

bool SpeedreaderBubbleGlobal::ShouldShowCloseButton() const {
  return true;
}

void SpeedreaderBubbleGlobal::WindowClosing() {
  if (controller_) {
    controller_->OnBubbleClosed();
    controller_ = nullptr;
  }
}

void SpeedreaderBubbleGlobal::Init() {
  // fixme:
  //   - localize entire function
  //   - unique_ptr everything
  LOG(ERROR) << "calling init on bubble";
  /*
  SetLayoutManager(std::make_unique<views::BoxLayout>(
      views::BoxLayout::Orientation::kVertical, gfx::Insets(),
      ChromeLayoutProvider::Get()->GetDistanceMetric(
          views::DISTANCE_RELATED_CONTROL_VERTICAL))); */

  SetLayoutManager(std::make_unique<views::BoxLayout>(
      views::BoxLayout::Orientation::kVertical, gfx::Insets(), 10));

  // Create sublayout for button and site title
  auto site_toggle_view = std::make_unique<views::View>();
  views::BoxLayout* site_toggle_layout =
      site_toggle_view->SetLayoutManager(std::make_unique<views::BoxLayout>());

  // Extract site title from webcontents, bolden it
  // fixme: for boldness we can do a style range on a label
  auto site = base::ASCIIToUTF16(web_contents_->GetLastCommittedURL().host());
  site.append(base::ASCIIToUTF16(kTextSeparator));
  site.append(base::ASCIIToUTF16("in Speedreader"));
  auto site_title_label = std::make_unique<views::Label>(site);
  site_title_label->SetLineHeight(kLineHeight);
  site_title_label->SetFontList(gfx::FontList(
      kReaderFontFamilies, gfx::Font::NORMAL, 14, gfx::Font::Weight::SEMIBOLD));
  site_title_label->SetHorizontalAlignment(gfx::ALIGN_LEFT);
  site_title_label->SetMultiLine(true);
  site_title_label_ =
      site_toggle_view->AddChildView(std::move(site_title_label));
  site_toggle_layout->SetFlexForView(
      site_title_label_,
      1);  // show the button and force text to wrap

  // float button right
  site_toggle_layout->set_main_axis_alignment(
      views::BoxLayout::MainAxisAlignment::kEnd);
  auto site_toggle_button =
      std::make_unique<views::ToggleButton>(base::BindRepeating(
          &SpeedreaderBubbleGlobal::OnButtonPressed, base::Unretained(this)));
  site_toggle_button_ =
      site_toggle_view->AddChildView(std::move(site_toggle_button));

  AddChildView(std::move(site_toggle_view));

  auto site_toggle_explanation = BuildLabelWithEndingLink(
      l10n_util::GetStringUTF16(IDS_SPEEDREADER_DISABLE_THIS_SITE),
      l10n_util::GetStringUTF16(IDS_SETTINGS_TITLE),
      base::BindRepeating(&SpeedreaderBubbleGlobal::OnLinkClicked,
                          base::Unretained(this)));
  AddChildView(std::move(site_toggle_explanation));
}

void SpeedreaderBubbleGlobal::OnButtonPressed(const ui::Event& event) {
  LOG(ERROR) << "button pressed";
}

void SpeedreaderBubbleGlobal::OnLinkClicked(const ui::Event& event) {
  LOG(ERROR) << "link clicked";
  web_contents_->OpenURL(
      content::OpenURLParams(GURL("chrome://settings"), content::Referrer(),
                             WindowOpenDisposition::NEW_FOREGROUND_TAB,
                             ui::PAGE_TRANSITION_LINK, false));
}

BEGIN_METADATA(SpeedreaderBubbleGlobal, LocationBarBubbleDelegateView)
END_METADATA
