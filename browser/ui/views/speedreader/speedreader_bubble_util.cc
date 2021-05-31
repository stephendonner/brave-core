#include "brave/browser/ui/views/speedreader/speedreader_bubble_util.h"

#include "ui/views/controls/styled_label.h"

namespace {

const std::vector<std::string> kReaderFontFamilies = {
    "Georgia",
    "Times New Roman",
    "Times"
    "serif",
};

}  // anonymous namespace

namespace speedreader {

const int kLineHeight = 16;

const int kBoxLayoutChildSpacing = 10;

const char kSpeedreaderSeparator[] = " ";

gfx::FontList GetFont(int font_size, gfx::Font::Weight weight) {
  return gfx::FontList(kReaderFontFamilies, gfx::Font::NORMAL, font_size,
                       weight);
}

// Create a StyledLabel that ends with a link.
std::unique_ptr<views::StyledLabel> BuildLabelWithEndingLink(
    const std::u16string& reg_text,
    const std::u16string& link_text,
    views::Link::ClickedCallback callback) {
  auto label = std::make_unique<views::StyledLabel>();
  std::u16string text = reg_text;
  text.append(base::ASCIIToUTF16(kSpeedreaderSeparator));
  size_t default_format_offset = text.length();
  text.append(link_text);
  label->SetText(text);

  // Setup styles
  views::StyledLabel::RangeStyleInfo style_link =
      views::StyledLabel::RangeStyleInfo::CreateForLink(callback);
  views::StyledLabel::RangeStyleInfo style_default;
  style_default.custom_font = GetFont(12);

  // Apply styles
  label->AddStyleRange(gfx::Range(0, default_format_offset), style_default);
  label->AddStyleRange(gfx::Range(default_format_offset, text.length()),
                       style_link);
  return label;
}

}  // namespace speedreader
