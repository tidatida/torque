
#ifndef _REQ_HPP
#define _REQ_HPP

/*
*         OpenPBS (Portable Batch System) v2.3 Software License
*
* Copyright (c) 1999-2000 Veridian Information Solutions, Inc.
* All rights reserved.
*
* ---------------------------------------------------------------------------
* For a license to use or redistribute the OpenPBS software under conditions
* other than those described below, or to purchase support for this software,
* please contact Veridian Systems, PBS Products Department ("Licensor") at:
*
*    www.OpenPBS.org  +1 650 967-4675                  sales@OpenPBS.org
*                        877 902-4PBS (US toll-free)
* ---------------------------------------------------------------------------
*
* This license covers use of the OpenPBS v2.3 software (the "Software") at
* your site or location, and, for certain users, redistribution of the
* Software to other sites and locations.  Use and redistribution of
* OpenPBS v2.3 in source and binary forms, with or without modification,
* are permitted provided that all of the following conditions are met.
* After December 31, 2001, only conditions 3-6 must be met:
*
* 1. Commercial and/or non-commercial use of the Software is permitted
*    provided a current software registration is on file at www.OpenPBS.org.
*    If use of this software contributes to a publication, product, or
*    service, proper attribution must be given; see www.OpenPBS.org/credit.html
*
* 2. Redistribution in any form is only permitted for non-commercial,
*    non-profit purposes.  There can be no charge for the Software or any
*    software incorporating the Software.  Further, there can be no
*    expectation of revenue generated as a consequence of redistributing
*    the Software.
*
* 3. Any Redistribution of source code must retain the above copyright notice
*    and the acknowledgment contained in paragraph 6, this list of conditions
*    and the disclaimer contained in paragraph 7.
*
* 4. Any Redistribution in binary form must reproduce the above copyright
*    notice and the acknowledgment contained in paragraph 6, this list of
*    conditions and the disclaimer contained in paragraph 7 in the
*    documentation and/or other materials provided with the distribution.
*
* 5. Redistributions in any form must be accompanied by information on how to
*    obtain complete source code for the OpenPBS software and any
*    modifications and/or additions to the OpenPBS software.  The source code
*    must either be included in the distribution or be available for no more
*    than the cost of distribution plus a nominal fee, and all modifications
*    and additions to the Software must be freely redistributable by any party
*    (including Licensor) without restriction.
*
* 6. All advertising materials mentioning features or use of the Software must
*    display the following acknowledgment:
*
*     "This product includes software developed by NASA Ames Research Center,
*     Lawrence Livermore National Laboratory, and Veridian Information
*     Solutions, Inc.
*     Visit www.OpenPBS.org for OpenPBS software support,
*     products, and information."
*
* 7. DISCLAIMER OF WARRANTY
*
* THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND. ANY EXPRESS
* OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT
* ARE EXPRESSLY DISCLAIMED.
*
* IN NO EVENT SHALL VERIDIAN CORPORATION, ITS AFFILIATED COMPANIES, OR THE
* U.S. GOVERNMENT OR ANY OF ITS AGENCIES BE LIABLE FOR ANY DIRECT OR INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
* OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
* This license will be governed by the laws of the Commonwealth of Virginia,
* without reference to its choice of law rules.
*/

#include <string>
#include <vector>

#include "list_link.h"

extern const int USE_CORES;
extern const int USE_THREADS;
extern const int ALLOW_THREADS;
extern const int USE_FAST_CORES;

extern const int PLACE_NO_PREFERENCE;
extern const int PLACE_NODE;
extern const int PLACE_SOCKET;
extern const int PLACE_NUMA_CHIP;

extern const int ALL_EXECUTION_SLOTS;

// This class is to hold all of the information for a single req from a job
// The concept of req(s) is only available under the new syntax
class req
  {
    int               execution_slots;
    unsigned long     mem;
    unsigned long     swap;
    unsigned long     disk;
    int               nodes;
    int               socket;
    int               numa_chip;
    int               cores;
    int               threads;
    int               thread_usage_policy;
    std::string       thread_usage_str;
    int               gpus;
    int               mics;
    int               maxtpn;
    std::string       gres;
    std::string       os;
    std::string       arch;
    std::string       node_access_policy;
    std::string       features;
    std::string       placement_str;
    std::string       req_attr;
    std::string       gpu_mode;
    int               placement_type;
    int               task_count;
    bool              pack;
    bool              single_job_access;
    // these are not set by user request
    int               index;
    std::string       hostlist;   // set when the job is run

  public:
    req();
    req(const req &other);
    req(const std::string &resource_request);
    req &operator =(const req &other);

    int           set_place_value(const char *value);
    int           set_value_from_string(char *str);
    int           set_attribute(const char *str);
    int           set_name_value_pair(const char *name, const char *value);
    void          set_from_string(const std::string &req_str);
    int           set_from_submission_string(char *submission_str, std::string &error);
    void          set_index(int index);
    int           set_value(const char *name, const char *value);
    int           submission_string_precheck(char *str, std::string &error);
    bool          submission_string_has_duplicates(char *str, std::string &error);
    bool          has_conflicting_values(std::string &error);
    int           append_gres(const char *val);
    void          get_values(std::vector<std::string> &names, std::vector<std::string> &values) const;
    void          toString(std::string &str) const;
    int           getExecutionSlots() const;
    unsigned long getMemory() const;
    unsigned long getSwap() const;
    unsigned long getDisk() const;
    int           getMaxtpn() const;
    std::string   getGpuMode() const;
    std::string   getGres() const;
    std::string   getOS() const;
    std::string   getNodeAccessPolicy() const;
    std::string   getPlacementType() const;
    std::string   getReqAttr() const;
    int           getTaskCount() const;
    int           getIndex() const;
    std::string   getHostlist() const;
    std::string   getFeatures() const;
    std::string   getThreadUsageString() const;
    unsigned long get_memory_for_host(const std::string &host) const;
    unsigned long get_swap_for_host(const std::string &host) const;
  };

#endif /* _REQ_HPP */