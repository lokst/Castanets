// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_NETWORK_PROXY_CONFIG_SERVICE_MOJO_H_
#define CONTENT_NETWORK_PROXY_CONFIG_SERVICE_MOJO_H_

#include "base/macros.h"
#include "base/observer_list.h"
#include "base/optional.h"
#include "content/common/content_export.h"
#include "content/public/common/network_service.mojom.h"
#include "content/public/common/proxy_config.mojom.h"
#include "mojo/public/cpp/bindings/binding.h"
#include "net/proxy/proxy_config.h"
#include "net/proxy/proxy_config_service.h"

namespace net {
class ProxyConfig;
}

namespace content {

// ProxyConfigService that gets its proxy configuration over a Mojo pipe.
class CONTENT_EXPORT ProxyConfigServiceMojo : public net::ProxyConfigService,
                                              public mojom::ProxyConfigClient {
 public:
  // |proxy_config_client_request| is the Mojo pipe over which new
  // configurations are received. |initial_proxy_config| is the initial proxy
  // configuration. If nullptr, waits for the initial configuration over
  // |proxy_config_client_request|. Either |proxy_config_client_request| or
  // |initial_proxy_config| must be non-null. If |proxy_config_client_request|
  // and |proxy_poller_client| are non-null, calls into |proxy_poller_client|
  // whenever OnLazyPoll() is invoked.
  explicit ProxyConfigServiceMojo(
      mojom::ProxyConfigClientRequest proxy_config_client_request,
      base::Optional<net::ProxyConfig> initial_proxy_config,
      mojom::ProxyConfigPollerClientPtr proxy_poller_client);
  ~ProxyConfigServiceMojo() override;

  // net::ProxyConfigService implementation:
  void AddObserver(Observer* observer) override;
  void RemoveObserver(Observer* observer) override;
  ConfigAvailability GetLatestProxyConfig(net::ProxyConfig* config) override;
  void OnLazyPoll() override;

 private:
  // mojom::ProxyConfigClient implementation:
  void OnProxyConfigUpdated(const net::ProxyConfig& proxy_config) override;

  mojom::ProxyConfigPollerClientPtr proxy_poller_client_;

  net::ProxyConfig config_;
  bool config_pending_ = true;

  mojo::Binding<mojom::ProxyConfigClient> binding_;

  base::ObserverList<Observer> observers_;

  DISALLOW_COPY_AND_ASSIGN(ProxyConfigServiceMojo);
};

}  // namespace content

#endif  // CONTENT_NETWORK_PROXY_CONFIG_SERVICE_MOJO_H_
