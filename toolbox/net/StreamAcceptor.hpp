// The Reactive C++ Toolbox.
// Copyright (C) 2013-2019 Swirly Cloud Limited
// Copyright (C) 2019 Reactive Markets Limited
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef TOOLBOX_NET_STREAMACCEPTOR_HPP
#define TOOLBOX_NET_STREAMACCEPTOR_HPP

#include <toolbox/io/Event.hpp>
#include <toolbox/io/Reactor.hpp>
#include <toolbox/net/StreamSock.hpp>

namespace toolbox {
inline namespace net {

template <typename DerivedT>
class StreamAcceptor {
  public:
    using Protocol = StreamProtocol;
    using Endpoint = StreamEndpoint;

    StreamAcceptor(Reactor& r, const Endpoint& ep)
    : serv_{ep.protocol()}
    {
        serv_.set_so_reuse_addr(true);
        serv_.bind(ep);
        serv_.listen(SOMAXCONN);
        sub_ = r.subscribe(*serv_, EventIn, bind<&StreamAcceptor::on_io_event>(this));
    }

    // Copy.
    StreamAcceptor(const StreamAcceptor&) = delete;
    StreamAcceptor& operator=(const StreamAcceptor&) = delete;

    // Move.
    StreamAcceptor(StreamAcceptor&&) = delete;
    StreamAcceptor& operator=(StreamAcceptor&&) = delete;

  protected:
    ~StreamAcceptor() = default;

  private:
    void on_io_event(CyclTime now, int fd, unsigned events)
    {
        Endpoint ep;
        IoSock sock{os::accept(fd, ep), serv_.family()};
        static_cast<DerivedT*>(this)->do_accept(now, std::move(sock), ep);
    }

    StreamSockServ serv_;
    Reactor::Handle sub_;
};

} // namespace net
} // namespace toolbox

#endif // TOOLBOX_NET_STREAMACCEPTOR_HPP
