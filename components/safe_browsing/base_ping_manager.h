// Copyright (c) 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef COMPONENTS_SAFE_BROWSING_BASE_PING_MANAGER_H_
#define COMPONENTS_SAFE_BROWSING_BASE_PING_MANAGER_H_

// A class that reports basic safebrowsing statistics to Google's SafeBrowsing
// servers.
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "base/gtest_prod_util.h"
#include "base/macros.h"
#include "base/memory/ref_counted.h"
#include "components/safe_browsing/db/hit_report.h"
#include "components/safe_browsing/db/util.h"
#include "content/public/browser/permission_type.h"
#include "services/network/public/cpp/shared_url_loader_factory.h"
#include "url/gurl.h"

namespace network {
class SimpleURLLoader;
}  // namespace network

namespace safe_browsing {

class BasePingManager {
 public:
  virtual ~BasePingManager();

  // Create an instance of the safe browsing ping manager.
  static std::unique_ptr<BasePingManager> Create(
      scoped_refptr<network::SharedURLLoaderFactory> url_loader_factory,
      const SafeBrowsingProtocolConfig& config);

  void OnURLLoaderComplete(network::SimpleURLLoader* source,
                           std::unique_ptr<std::string> response_body);

  // Report to Google when a SafeBrowsing warning is shown to the user.
  // |hit_report.threat_type| should be one of the types known by
  // SafeBrowsingtHitUrl.
  void ReportSafeBrowsingHit(const safe_browsing::HitReport& hit_report);

  // Users can opt-in on the SafeBrowsing interstitial to send detailed
  // threat reports. |report| is the serialized report.
  void ReportThreatDetails(const std::string& report);

 protected:
  friend class BasePingManagerTest;
  // Constructs a BasePingManager that issues network requests
  // using |url_loader_factory|.
  BasePingManager(
      scoped_refptr<network::SharedURLLoaderFactory> url_loader_factory,
      const SafeBrowsingProtocolConfig& config);

 private:
  FRIEND_TEST_ALL_PREFIXES(BasePingManagerTest, TestSafeBrowsingHitUrl);
  FRIEND_TEST_ALL_PREFIXES(BasePingManagerTest, TestThreatDetailsUrl);
  FRIEND_TEST_ALL_PREFIXES(BasePingManagerTest, TestReportThreatDetails);
  FRIEND_TEST_ALL_PREFIXES(BasePingManagerTest, TestReportSafeBrowsingHit);

  typedef std::set<std::unique_ptr<network::SimpleURLLoader>> Reports;

  // Generates URL for reporting safe browsing hits.
  GURL SafeBrowsingHitUrl(const safe_browsing::HitReport& hit_report) const;

  // Generates URL for reporting threat details for users who opt-in.
  GURL ThreatDetailsUrl() const;

  // Current product version sent in each request.
  std::string version_;

  // The safe browsing client name sent in each request.
  std::string client_name_;

  // The URLLoaderFactory we use to issue network requests.
  scoped_refptr<network::SharedURLLoaderFactory> url_loader_factory_;

  // URL prefix where browser reports hits to the safebrowsing list and
  // sends detaild threat reports for UMA users.
  std::string url_prefix_;

  // Track outstanding SafeBrowsing report fetchers for clean up.
  // We add both "hit" and "detail" fetchers in this set.
  Reports safebrowsing_reports_;

  DISALLOW_COPY_AND_ASSIGN(BasePingManager);
};

}  // namespace safe_browsing

#endif  // COMPONENTS_SAFE_BROWSING_BASE_PING_MANAGER_H_
