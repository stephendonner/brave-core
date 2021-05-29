/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_APP_BRAVE_COMMAND_IDS_H_
#define BRAVE_APP_BRAVE_COMMAND_IDS_H_

// First brave id must be higher than last chrome command.
// Check chrome/app/chrome_command_ids.h when rebase.
// ID of IDC_BRAVE_COMANDS_START and first brave command should be same.
#define IDC_BRAVE_COMMANDS_START 56000
#define IDC_SHOW_BRAVE_REWARDS   56000
#define IDC_SHOW_BRAVE_ADBLOCK   56001
#define IDC_NEW_TOR_CONNECTION_FOR_SITE     56002
#define IDC_NEW_OFFTHERECORD_WINDOW_TOR 56003
#define IDC_CONTENT_CONTEXT_OPENLINKTOR 56004
#define IDC_SHOW_BRAVE_SYNC      56005
#define IDC_SHOW_BRAVE_WALLET    56006
#define IDC_ADD_NEW_PROFILE      56007
#define IDC_OPEN_GUEST_PROFILE   56008
#define IDC_SHOW_BRAVE_WEBCOMPAT_REPORTER   56009
#define IDC_TOGGLE_SPEEDREADER   56010
#define IDC_SIDEBAR_SHOW_OPTION_MENU 56011
#define IDC_SIDEBAR_SHOW_OPTION_ALWAYS 56012
#define IDC_SIDEBAR_SHOW_OPTION_MOUSEOVER 56013
#define IDC_SIDEBAR_SHOW_OPTION_ONCLICK 56014
#define IDC_SIDEBAR_SHOW_OPTION_NEVER 56015
#define IDC_CONTENT_CONTEXT_IMPORT_IPFS 56016
#define IDC_CONTENT_CONTEXT_IMPORT_LINK_IPFS 56017
#define IDC_CONTENT_CONTEXT_IMPORT_IPFS_PAGE 56018
#define IDC_CONTENT_CONTEXT_IMPORT_IMAGE_IPFS 56019
#define IDC_CONTENT_CONTEXT_IMPORT_AUDIO_IPFS 56020
#define IDC_CONTENT_CONTEXT_IMPORT_VIDEO_IPFS 56021
#define IDC_CONTENT_CONTEXT_IMPORT_SELECTED_TEXT_IPFS 56022
#define IDC_APP_MENU_IPFS 56023
#define IDC_APP_MENU_IPFS_IMPORT_LOCAL_FILE 56024
#define IDC_APP_MENU_IPFS_IMPORT_LOCAL_FOLDER 56025
#define IDC_SHOW_BRAVE_WALLET_PANEL 56026
#define IDC_CLOSE_BRAVE_WALLET_PANEL 56027
#define IDC_SHOW_SPEEDREADER_PANEL 56028

#define IDC_CONTENT_CONTEXT_IMPORT_IPNS_KEYS_START 56100
#define IDC_CONTENT_CONTEXT_IMPORT_IPNS_KEYS_END 56199

#define IDC_BRAVE_COMMANDS_LAST  57000

#endif  // BRAVE_APP_BRAVE_COMMAND_IDS_H_
