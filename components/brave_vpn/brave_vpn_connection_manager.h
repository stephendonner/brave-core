/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_BRAVE_VPN_BRAVE_VPN_CONNECTION_MANAGER_H_
#define BRAVE_COMPONENTS_BRAVE_VPN_BRAVE_VPN_CONNECTION_MANAGER_H_

#include <string>

#include "base/observer_list.h"
#include "brave/components/brave_vpn/brave_vpn_connection_info.h"

namespace brave_vpn {

// Interface for managing OS' vpn connection.
class BraveVPNConnectionManager {
 public:
  class Observer : public base::CheckedObserver {
   public:
    virtual void OnEntryAdded(const std::string& name) = 0;
    virtual void OnEntryRemoved(const std::string& name) = 0;
    virtual void OnEntryConnected(const std::string& name) = 0;
    virtual void OnEntryDisconected(const std::string& name) = 0;

   protected:
    ~Observer() override = default;
  };

  static BraveVPNConnectionManager* GetInstance();

  BraveVPNConnectionManager(const BraveVPNConnectionManager&) = delete;
  BraveVPNConnectionManager& operator=(const BraveVPNConnectionManager&) =
      delete;

  void AddObserver(Observer* observer);
  void RemoveObserver(Observer* observer);

  virtual bool CreateVPNConnection(const BraveVPNConnectionInfo& info) = 0;
  virtual bool UpdateVPNConnection(const BraveVPNConnectionInfo& info) = 0;
  virtual bool Connect(const std::string& name) = 0;
  virtual bool Disconnect(const std::string& name) = 0;
  virtual bool RemoveVPNConnection(const std::string& name) = 0;

 protected:
  BraveVPNConnectionManager();
  virtual ~BraveVPNConnectionManager();

  base::ObserverList<Observer> observers_;
};

}  // namespace brave_vpn

#endif  // BRAVE_COMPONENTS_BRAVE_VPN_BRAVE_VPN_CONNECTION_MANAGER_H_
