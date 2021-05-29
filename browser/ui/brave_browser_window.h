/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_BROWSER_UI_BRAVE_BROWSER_WINDOW_H_
#define BRAVE_BROWSER_UI_BRAVE_BROWSER_WINDOW_H_

#include "brave/browser/ui/speedreader/speedreader_bubble_controller.h"
#include "brave/browser/ui/views/speedreader/speedreader_bubble_single_page.h"
#include "brave/browser/ui/views/speedreader/speedreader_bubble_global.h"
#include "brave/components/sidebar/buildflags/buildflags.h"
#include "chrome/browser/ui/browser_window.h"

namespace sidebar {
class Sidebar;
}  // namespace sidebar

//fixme class SpeedreaderBubbleSinglePage;
class SpeedreaderBubbleGlobal;
class SpeedreaderBubbleController;

class BraveBrowserWindow : public BrowserWindow {
 public:
  ~BraveBrowserWindow() override {}

  virtual void StartTabCycling() = 0;

  virtual SpeedreaderBubbleGlobal* ShowSpeedreaderBubble(
      content::WebContents* contents,
      SpeedreaderBubbleController* controller) = 0;

#if BUILDFLAG(ENABLE_SIDEBAR)
  virtual sidebar::Sidebar* InitSidebar() = 0;
#endif
};

#endif  // BRAVE_BROWSER_UI_BRAVE_BROWSER_WINDOW_H_
