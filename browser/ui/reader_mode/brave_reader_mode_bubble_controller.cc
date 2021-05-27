#include "brave/browser/ui/reader_mode/brave_reader_mode_bubble_controller.h"

#include "brave/browser/ui/brave_browser_window.h"
#include "chrome/browser/ui/browser.h"
#include "chrome/browser/ui/browser_finder.h"
#include "chrome/browser/ui/browser_window.h"
#include "content/public/browser/web_contents.h"
#include "content/public/browser/web_contents_user_data.h"

// static
BraveReaderModeBubbleController* BraveReaderModeBubbleController::Get(
    content::WebContents* web_contents) {
  BraveReaderModeBubbleController::CreateForWebContents(web_contents);
  BraveReaderModeBubbleController* controller =
      BraveReaderModeBubbleController::FromWebContents(web_contents);
  return controller;
}

BraveReaderModeBubbleController::BraveReaderModeBubbleController() = default;

BraveReaderModeBubbleController::BraveReaderModeBubbleController(
    content::WebContents* web_contents)
    : web_contents_(web_contents) {}

BraveReaderModeBubble*
BraveReaderModeBubbleController::reader_mode_bubble_view() const {
  return reader_mode_bubble_;
}

void BraveReaderModeBubbleController::OnBubbleClosed() {
  reader_mode_bubble_ = nullptr;
}

// Displays speedreader information
void BraveReaderModeBubbleController::ShowBubble() {
  // fixme
  // Browser* browser = chrome::FindBrowserWithWebContents(web_contents_);
  // reader_mode_bubble_ = browser->window()->
  Browser* browser = chrome::FindBrowserWithWebContents(web_contents_);
  DCHECK(browser);
  reader_mode_bubble_ = static_cast<BraveBrowserWindow*>(browser->window())
                            ->ShowReaderModeBubble(web_contents_, this);
}

// Hides speedreader information
void BraveReaderModeBubbleController::HideBubble() {
  // fixme
  reader_mode_bubble_ = nullptr;
}

// fixme: OnBubbleClosed nullptr

WEB_CONTENTS_USER_DATA_KEY_IMPL(BraveReaderModeBubbleController)
