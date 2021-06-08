/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/ephemeral_storage/ephemeral_storage_browsertest.h"

#include <string>

#include "base/path_service.h"
#include "base/threading/sequenced_task_runner_handle.h"
#include "base/time/time.h"
#include "brave/browser/ephemeral_storage/ephemeral_storage_tab_helper.h"
#include "brave/common/brave_paths.h"
#include "brave/components/brave_shields/browser/brave_shields_util.h"
#include "brave/components/brave_shields/common/brave_shield_constants.h"
#include "chrome/browser/content_settings/cookie_settings_factory.h"
#include "chrome/browser/content_settings/host_content_settings_map_factory.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/ui/browser.h"
#include "chrome/test/base/in_process_browser_test.h"
#include "chrome/test/base/ui_test_utils.h"
#include "components/content_settings/core/browser/cookie_settings.h"
#include "components/network_session_configurator/common/network_switches.h"
#include "components/prefs/pref_service.h"
#include "content/public/browser/notification_types.h"
#include "content/public/browser/render_frame_host.h"
#include "content/public/browser/storage_partition.h"
#include "content/public/browser/web_contents.h"
#include "content/public/test/browser_test.h"
#include "content/public/test/test_navigation_observer.h"
#include "net/base/features.h"
#include "net/dns/mock_host_resolver.h"
#include "net/test/embedded_test_server/default_handlers.h"
#include "url/gurl.h"

using content::RenderFrameHost;
using content::WebContents;

namespace {}  // namespace

class EphemeralStorage1pBrowserTest : public EphemeralStorageBrowserTest {
 public:
  EphemeralStorage1pBrowserTest() {
    scoped_feature_list_.InitAndEnableFeature(
        net::features::kBraveFirstPartyEphemeralStorage);
  }
  ~EphemeralStorage1pBrowserTest() override {}

  void SetCookieSetting(const GURL& url, ContentSetting content_setting) {
    auto cookie_settings =
        CookieSettingsFactory::GetForProfile(browser()->profile());
    cookie_settings->SetCookieSetting(url, content_setting);
  }

  void LoadIndexedDbHelper(RenderFrameHost* host) {
    const char kLoadIndexMinScript[] =
        "new Promise((resolve) => {"
        "  const script = document.createElement('script');"
        "  script.onload = () => {"
        "    resolve(true);"
        "  };"
        "  script.onerror = () => {"
        "    resolve(false);"
        "  };"
        "  script.src = '/ephemeral-storage/static/js/libs/index-min.js';"
        "  document.body.appendChild(script);"
        "});";

    ASSERT_EQ(true, content::EvalJs(host, kLoadIndexMinScript));
  }

  bool SetIDBValue(RenderFrameHost* host) {
    LoadIndexedDbHelper(host);
    content::EvalJsResult eval_js_result = content::EvalJs(
        host, "(async () => { await window.idbKeyval.set('a', 'a'); })()");
    return eval_js_result.error.empty();
  }

 private:
  base::test::ScopedFeatureList scoped_feature_list_;
};

IN_PROC_BROWSER_TEST_F(EphemeralStorage1pBrowserTest, FirstPartyIsEphemeral) {
  SetCookieSetting(a_site_ephemeral_storage_url_, CONTENT_SETTING_SESSION_ONLY);

  WebContents* first_party_tab = LoadURLInNewTab(a_site_ephemeral_storage_url_);

  // We set a value in the page where all the frames are first-party.
  SetValuesInFrames(first_party_tab, "a.com", "from=a.com");

  {
    ValuesFromFrames first_party_values = GetValuesFromFrames(first_party_tab);
    EXPECT_EQ("a.com", first_party_values.main_frame.local_storage);
    EXPECT_EQ("a.com", first_party_values.iframe_1.local_storage);
    EXPECT_EQ("a.com", first_party_values.iframe_2.local_storage);

    EXPECT_EQ("a.com", first_party_values.main_frame.session_storage);
    EXPECT_EQ("a.com", first_party_values.iframe_1.session_storage);
    EXPECT_EQ("a.com", first_party_values.iframe_2.session_storage);

    EXPECT_EQ("from=a.com", first_party_values.main_frame.cookies);
    EXPECT_EQ("from=a.com", first_party_values.iframe_1.cookies);
    EXPECT_EQ("from=a.com", first_party_values.iframe_2.cookies);
  }

  // After keepalive values should be cleared.
  ui_test_utils::NavigateToURL(browser(), b_site_ephemeral_storage_url_);
  WaitForCleanupAfterKeepAlive();
  ui_test_utils::NavigateToURL(browser(), a_site_ephemeral_storage_url_);

  ExpectValuesFromFramesAreEmpty(FROM_HERE,
                                 GetValuesFromFrames(first_party_tab));
}

IN_PROC_BROWSER_TEST_F(EphemeralStorage1pBrowserTest,
                       StorageIsPartitionedAndCleared) {
  SetCookieSetting(a_site_ephemeral_storage_url_, CONTENT_SETTING_SESSION_ONLY);
  SetCookieSetting(b_site_ephemeral_storage_url_, CONTENT_SETTING_SESSION_ONLY);
  SetCookieSetting(c_site_ephemeral_storage_url_, CONTENT_SETTING_SESSION_ONLY);

  WebContents* first_party_tab = LoadURLInNewTab(b_site_ephemeral_storage_url_);
  WebContents* site_a_tab1 = LoadURLInNewTab(a_site_ephemeral_storage_url_);
  WebContents* site_a_tab2 = LoadURLInNewTab(a_site_ephemeral_storage_url_);
  WebContents* site_c_tab = LoadURLInNewTab(c_site_ephemeral_storage_url_);

  EXPECT_EQ(browser()->tab_strip_model()->count(), 5);

  // We set a value in the page where all the frames are first-party.
  SetValuesInFrames(first_party_tab, "b.com - first party", "from=b.com");

  // The page this tab is loaded via a.com and has two b.com third-party
  // iframes. The third-party iframes should have ephemeral storage. That means
  // that their values should be shared by third-party b.com iframes loaded from
  // a.com.
  SetValuesInFrames(site_a_tab1, "a.com", "from=a.com");
  ValuesFromFrames site_a_tab1_values = GetValuesFromFrames(site_a_tab1);
  EXPECT_EQ("a.com", site_a_tab1_values.main_frame.local_storage);
  EXPECT_EQ("a.com", site_a_tab1_values.iframe_1.local_storage);
  EXPECT_EQ("a.com", site_a_tab1_values.iframe_2.local_storage);

  EXPECT_EQ("a.com", site_a_tab1_values.main_frame.session_storage);
  EXPECT_EQ("a.com", site_a_tab1_values.iframe_1.session_storage);
  EXPECT_EQ("a.com", site_a_tab1_values.iframe_2.session_storage);

  EXPECT_EQ("from=a.com", site_a_tab1_values.main_frame.cookies);
  EXPECT_EQ("from=a.com", site_a_tab1_values.iframe_1.cookies);
  EXPECT_EQ("from=a.com", site_a_tab1_values.iframe_2.cookies);

  // The second tab is loaded on the same domain, so should see the same
  // storage for the third-party iframes.
  ValuesFromFrames site_a_tab2_values = GetValuesFromFrames(site_a_tab2);
  EXPECT_EQ("a.com", site_a_tab2_values.main_frame.local_storage);
  EXPECT_EQ("a.com", site_a_tab2_values.iframe_1.local_storage);
  EXPECT_EQ("a.com", site_a_tab2_values.iframe_2.local_storage);

  EXPECT_EQ(nullptr, site_a_tab2_values.main_frame.session_storage);
  EXPECT_EQ(nullptr, site_a_tab2_values.iframe_1.session_storage);
  EXPECT_EQ(nullptr, site_a_tab2_values.iframe_2.session_storage);

  EXPECT_EQ("from=a.com", site_a_tab2_values.main_frame.cookies);
  EXPECT_EQ("from=a.com", site_a_tab2_values.iframe_1.cookies);
  EXPECT_EQ("from=a.com", site_a_tab2_values.iframe_2.cookies);

  // The storage in the first-party iframes should still reflect the
  // original value that was written in the non-ephemeral storage area.
  ValuesFromFrames first_party_values = GetValuesFromFrames(first_party_tab);
  EXPECT_EQ("b.com - first party", first_party_values.main_frame.local_storage);
  EXPECT_EQ("b.com - first party", first_party_values.iframe_1.local_storage);
  EXPECT_EQ("b.com - first party", first_party_values.iframe_2.local_storage);

  EXPECT_EQ("b.com - first party",
            first_party_values.main_frame.session_storage);
  EXPECT_EQ("b.com - first party", first_party_values.iframe_1.session_storage);
  EXPECT_EQ("b.com - first party", first_party_values.iframe_2.session_storage);

  EXPECT_EQ("from=b.com", first_party_values.main_frame.cookies);
  EXPECT_EQ("from=b.com", first_party_values.iframe_1.cookies);
  EXPECT_EQ("from=b.com", first_party_values.iframe_2.cookies);

  // Even though this page loads b.com iframes as third-party iframes, the TLD
  // differs, so it should get an entirely different ephemeral storage area.
  ValuesFromFrames site_c_tab_values = GetValuesFromFrames(site_c_tab);
  ExpectValuesFromFramesAreEmpty(FROM_HERE, site_c_tab_values);

  // Close 4 tabs.
  for (int i = 0; i < 4; ++i) {
    browser()->tab_strip_model()->CloseWebContentsAt(1,
                                                     TabStripModel::CLOSE_NONE);
  }

  WaitForCleanupAfterKeepAlive();

  ExpectValuesFromFramesAreEmpty(
      FROM_HERE,
      GetValuesFromFrames(LoadURLInNewTab(a_site_ephemeral_storage_url_)));
  ExpectValuesFromFramesAreEmpty(
      FROM_HERE,
      GetValuesFromFrames(LoadURLInNewTab(b_site_ephemeral_storage_url_)));
  ExpectValuesFromFramesAreEmpty(
      FROM_HERE,
      GetValuesFromFrames(LoadURLInNewTab(c_site_ephemeral_storage_url_)));
}

IN_PROC_BROWSER_TEST_F(EphemeralStorage1pBrowserTest,
                       IndexedDbUnavailableIn3p) {
  SetCookieSetting(a_site_ephemeral_storage_url_, CONTENT_SETTING_SESSION_ONLY);
  SetCookieSetting(b_site_ephemeral_storage_url_, CONTENT_SETTING_SESSION_ONLY);

  WebContents* site_a = LoadURLInNewTab(a_site_ephemeral_storage_url_);
  WebContents* site_b = LoadURLInNewTab(b_site_ephemeral_storage_url_);

  // Main frame and 1p frame.
  EXPECT_TRUE(SetIDBValue(site_a->GetMainFrame()));
  EXPECT_TRUE(SetIDBValue(content::ChildFrameAt(site_a->GetMainFrame(), 2)));
  // 3p frames.
  EXPECT_FALSE(SetIDBValue(content::ChildFrameAt(site_a->GetMainFrame(), 0)));
  EXPECT_FALSE(SetIDBValue(content::ChildFrameAt(site_a->GetMainFrame(), 1)));

  // 3p frame.
  EXPECT_FALSE(SetIDBValue(content::ChildFrameAt(site_b->GetMainFrame(), 2)));
}
