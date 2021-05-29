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
#include "chrome/browser/ui/views/location_bar/location_bar_bubble_delegate_view.h"
#include "chrome/grit/generated_resources.h"
#include "components/strings/grit/components_strings.h"
#include "content/public/browser/page_navigator.h"
#include "content/public/browser/web_contents.h"
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
#include "ui/views/metadata/metadata_impl_macros.h"
#include "ui/views/view.h"

namespace {

constexpr char kReaderMoreLearnMoreSeparator[] = " ";

const std::vector<std::string> kReaderFontFamilies = {
    "Georgia",
    "Times New Roman",
    "Times"
    "serif",
};

}  // anonymous namespace

// Material Design button, overriding the font list in the LabelButton.
class ReaderButton : public views::MdTextButton {
 public:
  explicit ReaderButton(PressedCallback callback = PressedCallback(),
                        const std::u16string& text = std::u16string(),
                        int button_context = views::style::CONTEXT_BUTTON_MD)
      : views::MdTextButton(callback, text, button_context) {
    label()->SetFontList(gfx::FontList(kReaderFontFamilies, gfx::Font::NORMAL,
                                       13, gfx::Font::Weight::SEMIBOLD));
  }

  void SetEnabledColor(SkColor color) { label()->SetEnabledColor(color); }
  ~ReaderButton() override = default;
};

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
      264, LocationBarBubbleDelegateView::CalculatePreferredSize().height());
}

void SpeedreaderBubbleGlobal::OnThemeChanged() {
  LocationBarBubbleDelegateView::OnThemeChanged();
  UpdateColors();
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
  constexpr int kRowPadding = 10;
  views::GridLayout* layout =
      SetLayoutManager(std::make_unique<views::GridLayout>());
  views::ColumnSet* c0 = layout->AddColumnSet(0);
  c0->AddColumn(
      /*h_align=*/views::GridLayout::FILL,
      /*v_align=*/views::GridLayout::FILL,
      /*resize_percent=*/1.0,
      /*size_type=*/views::GridLayout::ColumnSize::kUsePreferred,
      /*fixed_width=*/0,
      /*min_width=*/0);
  views::ColumnSet* c1 = layout->AddColumnSet(1);
  c1->AddColumn(
      /*h_align=*/views::GridLayout::LEADING,
      /*v_align=*/views::GridLayout::FILL,
      /*resize_percent=*/0.0,
      /*size_type=*/views::GridLayout::ColumnSize::kFixed,
      /*fixed_width=*/174,  // fixme: use math to get this
      /*min_width=*/0);
  c1->AddColumn(
      /*h_align=*/views::GridLayout::FILL,
      /*v_align=*/views::GridLayout::FILL,
      /*resize_percent=*/1.0,
      /*size_type=*/views::GridLayout::ColumnSize::kUsePreferred,
      /*fixed_width=*/0,  // fixme: use math to get th
      /*min_width=*/0);

  // Heading
  layout->StartRow(0, 1);
  heading_label_ = layout->AddView(std::make_unique<views::Label>(
      base::ASCIIToUTF16("wsj.com in Speedreader")));
  heading_label_->SetFontList(gfx::FontList(
      kReaderFontFamilies, gfx::Font::NORMAL, 14, gfx::Font::Weight::SEMIBOLD));
  heading_label_->SetHorizontalAlignment(gfx::ALIGN_LEFT);
  // fixme: for boldness we can do a style range on a label
  auto toggle_button =
      std::make_unique<views::ToggleButton>(base::BindRepeating(
          &SpeedreaderBubbleGlobal::OnButtonPressed, base::Unretained(this)));
  toggle_button->SetIsOn(true);
  layout->AddView(std::move(toggle_button));

  layout->AddPaddingRow(views::GridLayout::kFixedSize, kRowPadding);

  // Explanation text with link
  layout->StartRow(0, 0);
  std::u16string explanation_text = base::ASCIIToUTF16(
      "Articles automatically load in reader mode, saving you time.");
  explanation_text.append(base::ASCIIToUTF16(kReaderMoreLearnMoreSeparator));
  auto explanation_length = explanation_text.length();

  explanation_text.append(l10n_util::GetStringUTF16(IDS_SETTINGS_TITLE));
  auto explanation_label = std::make_unique<views::StyledLabel>();
  explanation_label->SetText(explanation_text);

  views::StyledLabel::RangeStyleInfo explanation_style_text;
  explanation_style_text.custom_font = gfx::FontList(
      kReaderFontFamilies, gfx::Font::NORMAL, 12, gfx::Font::Weight::NORMAL);
  explanation_label->AddStyleRange(gfx::Range(0, explanation_length),
                                   explanation_style_text);
  auto explanation_style_link =
      views::StyledLabel::RangeStyleInfo::CreateForLink(base::BindRepeating(
          &SpeedreaderBubbleGlobal::LearnMoreClicked, base::Unretained(this)));
  explanation_label->AddStyleRange(
      gfx::Range(explanation_length, explanation_text.length()),
      explanation_style_link);

  layout->AddView(std::move(explanation_label));
  // layout->AddPaddingRow(views::GridLayout::kFixedSize, kRowPadding);

  // Button
  // layout->StartRow(0, 0);
  /*
  button_ = layout->AddView(std::make_unique<ReaderButton>(
      base::BindRepeating(&SpeedreaderBubbleGlobal::OnButtonPressed,
                          base::Unretained(this)),
      base::ASCIIToUTF16("Turn on Speedreader"))); */
}

void SpeedreaderBubbleGlobal::UpdateColors() {
  const ui::ThemeProvider* theme_provider = GetThemeProvider();
  if (!theme_provider)
    return;

  // SkColor brave_reader_default_color = theme_provider->GetColor(
  //    BraveThemeProperties::COLOR_SIDEBAR_ARROW_NORMAL);
  // SkColor brave_reader_light_color =
  //    theme_provider->GetColor(BraveThemeProperties::COLOR_SIDEBAR_BUTTON_BASE);

  // heading
  // heading_label_->SetEnabledColor(brave_reader_default_color);

  // subtext
  // subtext_label_->SetEnabledColor(brave_reader_light_color);

  // button
  // button_->SetEnabledColor(brave_reader_default_color);
}

void SpeedreaderBubbleGlobal::OnButtonPressed(const ui::Event& event) {
  LOG(ERROR) << "button pressed";
}

void SpeedreaderBubbleGlobal::LearnMoreClicked(const ui::Event& event) {
  LOG(ERROR) << "button clicked";
  web_contents_->OpenURL(content::OpenURLParams(
      GURL(kSpeedreaderLearnMoreUrl), content::Referrer(),
      WindowOpenDisposition::NEW_FOREGROUND_TAB, ui::PAGE_TRANSITION_LINK,
      false));
}

BEGIN_METADATA(SpeedreaderBubbleGlobal, LocationBarBubbleDelegateView)
END_METADATA
