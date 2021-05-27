#include "brave/browser/ui/views/reader_mode/brave_reader_mode_bubble.h"

#include <memory>

#include "brave/browser/ui/reader_mode/brave_reader_mode_bubble_controller.h"
#include "content/public/browser/web_ui_controller.h"
#include "ui/base/ui_base_types.h"
#include "ui/views/controls/label.h"
#include "ui/views/layout/box_layout.h"
#include "ui/views/metadata/metadata_impl_macros.h"
#include "ui/views/view.h"

#include "base/strings/utf_string_conversions.h"

BraveReaderModeBubble::BraveReaderModeBubble(
    views::View* anchor_view,
    content::WebContents* web_contents,
    BraveReaderModeBubbleController* controller)
    : LocationBarBubbleDelegateView(anchor_view, nullptr),
      controller_(controller) {
  SetButtons(ui::DialogButton::DIALOG_BUTTON_NONE);
}

void BraveReaderModeBubble::Show() {
  LOG(ERROR) << "calling show on bubble!";
  ShowForReason(USER_GESTURE);
}

void BraveReaderModeBubble::Hide() {
  LOG(ERROR) << "calling hide on bubble!";
  if (controller_) {
    controller_->OnBubbleClosed();
    controller_ = nullptr;
  }
  CloseBubble();
}

bool BraveReaderModeBubble::ShouldShowCloseButton() const {
  return true;
}

void BraveReaderModeBubble::WindowClosing() {
  if (controller_) {
    controller_->OnBubbleClosed();
    controller_ = nullptr;
  }
}

void BraveReaderModeBubble::Init() {
  LOG(ERROR) << "calling init on bubble";
  SetLayoutManager(std::make_unique<views::BoxLayout>(
      views::BoxLayout::Orientation::kVertical));
  label_ = new views::Label(base::ASCIIToUTF16("Hello, World!"));
  AddChildView(label_);
}

BEGIN_METADATA(BraveReaderModeBubble, LocationBarBubbleDelegateView)
END_METADATA
