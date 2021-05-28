#include "brave/browser/ui/views/reader_mode/brave_reader_mode_bubble.h"

#include <memory>
#include <vector>

#include "base/bind.h"
#include "brave/browser/themes/theme_properties.h"
#include "brave/browser/ui/reader_mode/brave_reader_mode_bubble_controller.h"
#include "chrome/browser/ui/views/location_bar/location_bar_bubble_delegate_view.h"
#include "content/public/browser/web_ui_controller.h"
#include "include/core/SkColor.h"
#include "ui/base/theme_provider.h"
#include "ui/base/ui_base_types.h"
#include "ui/events/event.h"
#include "ui/views/controls/button/md_text_button.h"
#include "ui/views/controls/label.h"
#include "ui/views/controls/link.h"
#include "ui/views/layout/box_layout.h"
#include "ui/views/layout/flex_layout.h"
#include "ui/views/metadata/metadata_impl_macros.h"
#include "ui/views/view.h"

#include "base/strings/utf_string_conversions.h"

namespace {

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

gfx::Size BraveReaderModeBubble::CalculatePreferredSize() const {
  return gfx::Size(
      264, LocationBarBubbleDelegateView::CalculatePreferredSize().height());
}

void BraveReaderModeBubble::OnThemeChanged() {
  LocationBarBubbleDelegateView::OnThemeChanged();
  UpdateColors();
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
  // fixme:
  //   - localize entire function
  //   - unique_ptr everything
  LOG(ERROR) << "calling init on bubble";
  // constexpr int kTopPadding = 47;
  views::BoxLayout* layout =
      SetLayoutManager(std::make_unique<views::BoxLayout>(
          views::BoxLayout::Orientation::kVertical));
  layout->set_between_child_spacing(10);
  heading_label_ = new views::Label(
      base::ASCIIToUTF16("Do you want to turn on Speedreader?"));
  heading_label_->SetMultiLine(true);
  heading_label_->SetFontList(gfx::FontList(
      kReaderFontFamilies, gfx::Font::NORMAL, 16, gfx::Font::Weight::SEMIBOLD));
  AddChildView(heading_label_);

  views::View* subtext_view = new views::View();
  subtext_view->SetLayoutManager(std::make_unique<views::BoxLayout>(
      views::BoxLayout::Orientation::kVertical));
  subtext_label_ = new views::Label(base::ASCIIToUTF16(
      "Articles automatically load in reader mode, saving you time."));
  subtext_label_->SetMultiLine(true);
  subtext_label_->SetFontList(gfx::FontList(
      kReaderFontFamilies, gfx::Font::NORMAL, 12, gfx::Font::Weight::NORMAL));
  learn_more_link_ = new views::Link(base::ASCIIToUTF16("Learn more"));
  learn_more_link_->SetFontList(gfx::FontList(
      kReaderFontFamilies, gfx::Font::NORMAL, 12, gfx::Font::Weight::NORMAL));
  learn_more_link_->SetMultiLine(true);
  subtext_view->AddChildView(subtext_label_);
  subtext_view->AddChildView(learn_more_link_);
  AddChildView(subtext_view);

  button_ = new ReaderButton(
      base::BindRepeating(&BraveReaderModeBubble::OnButtonPressed,
                          base::Unretained(this)),
      base::ASCIIToUTF16("Turn on Speedreader"));
  AddChildView(button_);
}

void BraveReaderModeBubble::UpdateColors() {
  const ui::ThemeProvider* theme_provider = GetThemeProvider();
  if (!theme_provider)
    return;

  // SkColor brave_reader_default_color = theme_provider->GetColor(
  //    BraveThemeProperties::COLOR_SIDEBAR_ARROW_NORMAL);
  SkColor brave_reader_light_color =
      theme_provider->GetColor(BraveThemeProperties::COLOR_SIDEBAR_BUTTON_BASE);

  // heading
  // heading_label_->SetEnabledColor(brave_reader_default_color);

  // subtext
  subtext_label_->SetEnabledColor(brave_reader_light_color);
  learn_more_link_->SetEnabledColor(brave_reader_light_color);

  // button
  // button_->SetEnabledColor(brave_reader_default_color);
}

void BraveReaderModeBubble::OnButtonPressed(const ui::Event& event) {
  LOG(ERROR) << "button pressed";
}

BEGIN_METADATA(BraveReaderModeBubble, LocationBarBubbleDelegateView)
END_METADATA
