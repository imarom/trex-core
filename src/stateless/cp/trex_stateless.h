/*
 Itay Marom
 Cisco Systems, Inc.
*/

/*
Copyright (c) 2015-2015 Cisco Systems, Inc.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
#ifndef __TREX_STATELESS_H__
#define __TREX_STATELESS_H__

#include <stdint.h>
#include <string>
#include <stdexcept>

#include <mutex>

#include <trex_stream.h>
#include <trex_stateless_port.h>
#include <trex_stateless_dp_core.h>
#include <trex_rpc_server_api.h>

/**
 * generic exception for errors
 * TODO: move this to a better place
 */
class TrexException : public std::runtime_error 
{
public:
    TrexException() : std::runtime_error("") {

    }
    TrexException(const std::string &what) : std::runtime_error(what) {
    }
};

class TrexStatelessPort;

/**
 * unified stats
 * 
 * @author imarom (06-Oct-15)
 */
class TrexStatelessStats {
public:
    TrexStatelessStats() {
        m_stats = {0};
    }

    struct {
        double m_cpu_util;

        double m_tx_bps;
        double m_rx_bps;
       
        double m_tx_pps;
        double m_rx_pps;
       
        uint64_t m_total_tx_pkts;
        uint64_t m_total_rx_pkts;
       
        uint64_t m_total_tx_bytes;
        uint64_t m_total_rx_bytes;
       
        uint64_t m_tx_rx_errors;
    } m_stats;
};

/**
 * config object for stateless object
 * 
 * @author imarom (08-Oct-15)
 */
class TrexStatelessCfg {
public:
    /* default values */
    TrexStatelessCfg() {
        m_port_count          = 0;
        m_dp_core_count       = 0;
        m_rpc_req_resp_cfg    = NULL;
        m_rpc_async_cfg       = NULL;
        m_rpc_server_verbose = false;
    }

    const TrexRpcServerConfig  *m_rpc_req_resp_cfg;
    const TrexRpcServerConfig  *m_rpc_async_cfg;
    bool                        m_rpc_server_verbose;
    uint8_t                     m_port_count;
    uint8_t                     m_dp_core_count;
};

/**
 * defines the T-Rex stateless operation mode
 * 
 */
class TrexStateless {
public:

    /**
     * configure the stateless object singelton 
     * reconfiguration is not allowed
     * an exception will be thrown
     */
    static void configure(const TrexStatelessCfg &cfg);

    /**
     * destroy the instance
     * 
     */
    static void destroy();

    /**
     * singleton public get instance
     * 
     */
    static TrexStateless& get_instance() {
        TrexStateless& instance = get_instance_internal();

        if (!instance.m_is_configured) {
            throw TrexException("object is not configured");
        }

        return instance;
    }

    /**
     * starts the control plane side
     * 
     */
    void launch_control_plane();

    /**
     * launch on a single DP core
     * 
     */
    void launch_on_dp_core(uint8_t core_id);

    TrexStatelessPort * get_port_by_id(uint8_t port_id);
    uint8_t             get_port_count();

    uint8_t             get_dp_core_count();

    /**
     * update all the stats (deep update)
     * (include all the ports and global stats)
     * 
     */
    void               update_stats();

    /**
     * fetch all the stats
     * 
     */
    void               encode_stats(Json::Value &global);


protected:
    TrexStateless();

    static TrexStateless& get_instance_internal () {
        static TrexStateless instance;
        return instance;
    }

     /* c++ 2011 style singleton */
    TrexStateless(TrexStateless const&)      = delete;  
    void operator=(TrexStateless const&)     = delete;

    /* status */
    bool                 m_is_configured;

    /* RPC server array */
    TrexRpcServer        *m_rpc_server;

    /* ports */
    std::vector <TrexStatelessPort *>    m_ports;
    uint8_t                              m_port_count;

    /* cores */
    std::vector <TrexStatelessDpCore *>  m_dp_cores;
    uint8_t                              m_dp_core_count;

    /* stats */
    TrexStatelessStats   m_stats;

    std::mutex m_global_cp_lock;
};

#endif /* __TREX_STATELESS_H__ */
