// -*- mode:c++; c-style:k&r; c-basic-offset:4; indent-tabs-mode: nil; -*-
// vi:set et cin sw=4 cino=>se0n0f0{0}0^0\:0=sl1g0hspst0+sc3C0/0(0u0U0w0m0:

#ifndef __MEMORY_HPP__
#define __MEMORY_HPP__

#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>
#include "statistics.hpp"
#include "logger.hpp"
#include "random.hpp"
#include "dramController.hpp"
#include "messages.hpp"

#include "memory_types.hpp"

using namespace std;
//using namespace boost;

class memoryRequest {
public:
    /* for reads */
    memoryRequest(maddr_t maddr, uint32_t word_count);
    /* for writes */
    memoryRequest(maddr_t maddr, uint32_t word_count, boost::shared_array<uint32_t> wdata);
    ~memoryRequest();

    inline memReqStatus_t status() { return m_status; }
    inline maddr_t maddr() { return m_maddr; }
    inline boost::shared_array<uint32_t> data() { return m_data; }
    inline bool is_read() { return m_is_read; }
    inline uint32_t home() { return m_home; }
    inline uint32_t word_count() { return m_word_count; }

    friend class memory;

private:

    memReqStatus_t m_status;
    bool m_is_read;
    maddr_t m_maddr;
    uint32_t m_word_count;
    boost::shared_array<uint32_t> m_data;
    uint32_t m_home;

};

class memory {
public:
    memory(uint32_t numeric_id, 
           const uint64_t &system_time,
           boost::shared_ptr<tile_statistics> stats,
           logger &log,
           boost::shared_ptr<random_gen> ran);
    virtual ~memory();

    virtual void request(boost::shared_ptr<memoryRequest> req) = 0;
    virtual void tick_positive_edge() = 0;
    virtual void tick_negative_edge() = 0;

    virtual uint32_t number_of_mem_msg_types() = 0;

    void add_local_dram_controller(boost::shared_ptr<dram> connected_dram, 
                                   uint32_t dram_controller_latency, uint32_t offchip_oneway_latency, uint32_t dram_latency,
                                   uint32_t msg_header_size_in_words, uint32_t max_requests_in_flight, 
                                   uint32_t bandwidth_in_words_per_cycle, bool use_lock);
    void set_remote_dram_controller(uint32_t location);

    inline void set_core_send_queues(map<uint32_t, boost::shared_ptr<messageQueue> > queues) { m_core_send_queues = queues; }
    inline void set_core_receive_queues(map<uint32_t, boost::shared_ptr<messageQueue> > queues) { m_core_receive_queues = queues; }

protected:
    inline void set_req_status(boost::shared_ptr<memoryRequest> req, memReqStatus_t status) { req->m_status = status; }
    inline void set_req_data(boost::shared_ptr<memoryRequest> req, boost::shared_array<uint32_t> data) { req->m_data = data; }

    uint32_t m_id;
    const uint64_t &system_time;
    boost::shared_ptr<tile_statistics> stats;
    logger &log;
    boost::shared_ptr<random_gen> ran;

    dramController* m_dram_controller;
    uint32_t m_dram_controller_location;

    map<uint32_t/*msg type*/, boost::shared_ptr<messageQueue> > m_core_send_queues;
    map<uint32_t/*msg type*/, boost::shared_ptr<messageQueue> > m_core_receive_queues;

};

#endif
