#include <asyncio/stream.h>
#include <arpa/inet.h>
#include <asyncio/runner.h>
#include <asyncio/start_server.h>
#include <asyncio/task.h>
#include <fmt/core.h>

using asyncio::Stream;
using asyncio::Task;
using asyncio::get_in_port;
using asyncio::get_in_addr;

int add_count = 0;
int rel_count = 0;

Task<> handle_echo(Stream stream) {
    auto sockinfo = stream.get_sock_info();
    char addr[INET6_ADDRSTRLEN] {};
    auto sa = reinterpret_cast<const sockaddr*>(&sockinfo);

    ++add_count;
    // fmt::print("connections: {}/{}\n", rel_count, add_count);
    while (true) {
        try {
            auto data = co_await stream.read(200);
            if (data.empty()) { break; }
            // fmt::print("Received: '{}' from '{}:{}'\n", data.data(),
                    // inet_ntop(sockinfo.ss_family, get_in_addr(sa), addr, sizeof addr),
                    // get_in_port(sa));
            co_await stream.write(data);
        } catch (...) {
            break;
        }
    }
    ++rel_count;
    // fmt::print("connections: {}/{}\n", rel_count, add_count);
    stream.close();
}

Task<> echo_server() {
    auto server = co_await asyncio::start_server(
            handle_echo, "127.0.0.1", 8888);

    fmt::print("Serving on 127.0.0.1:8888\n");

    co_await server.serve_forever();
}

int main() {
    asyncio::run(echo_server());
    return 0;
}


/*
   int main() 
    {
	    asyncio::run(echo_server());
	    return 0;
    }
	    
    template<concepts::Future Fut>
	decltype(auto) run(Fut&& main) 
	{
	    auto t = schedule_task(std::forward<Fut>(main));
	    
  //---------------------
  // accept
  //---------------------
  Task<void> serve_forever() 
  {
      Event ev { .fd = fd_, .flags = Event::Flags::EVENT_READ };
      auto& loop = get_event_loop();
      auto ev_awaiter = loop.wait_event(ev);
      std::list<ScheduledTask<Task<>>> connected;
      
      while (true) 
      {
          sockaddr_storage remoteaddr{};
          socklen_t addrlen = sizeof(remoteaddr);
          
          co_await ev_awaiter;       
          int clientfd = ::accept(fd_, reinterpret_cast<sockaddr*>(&remoteaddr), &addrlen);
          
          if (clientfd == -1) { continue; }
          connected.emplace_back(schedule_task(connect_cb_(Stream{clientfd, remoteaddr})));
          // garbage collect
          clean_up_connected(connected);
      }
  }
	    get_event_loop().run_until_complete();

	    if constexpr (std::is_lvalue_reference_v<Fut&&>) 
	    {
		return t.get_result();
	    } 
	    else 
	    {
		return std::move(t).get_result();
	    }
	}

    void EventLoop::run_until_complete() 
    {
   	while (! is_stop()) { run_once(); }
    }
    
    void EventLoop::run_once() 
    {
	    std::optional<MSDuration> timeout;
	    if (! ready_.empty()) {
		timeout.emplace(0);
	    } else if (! schedule_.empty()) {
		auto&& [when, _] = schedule_[0];
		timeout = std::max(when - time(), MSDuration(0));
	    }
	    
	    //---------------------
	    // select
	    //---------------------
	    auto event_lists = selector_.select(timeout.has_value() ? timeout->count() : -1);
	    for (auto&& event: event_lists) {
		ready_.push(event.handle_info);
	    }   
	    
            auto end_time = time();
            while (! schedule_.empty()) {
                auto&& [when, handle_info] = schedule_[0];
                if (when >= end_time) break;
                ready_.push(handle_info);
                ranges::pop_heap(schedule_, ranges::greater{}, &TimerHandle::first);
                schedule_.pop_back();
            }

            for (size_t ntodo = ready_.size(), i = 0; i < ntodo; ++i) 
            {
                auto [handle_id, handle] = ready_.front(); ready_.pop();
                if (auto iter = cancelled_.find(handle_id); iter != cancelled_.end()) 
                {
                    cancelled_.erase(iter);
                } 
                else 
                {
                    handle->set_state(Handle::UNSCHEDULED);
                    //---------------------
                    // void run() final 
                    // {
                    //    Resumes the execution of the coroutine to which *this refers, 
                    //    or does nothing if the coroutine is a no-op coroutine.
                    //
                    //    coro_handle::from_promise(*this).resume();
                    // }
                    //---------------------
                    handle->run();
                }
            }

            cleanup_delayed_call();
    }
    
    
    std::vector<Event> select(int timeout ) 
    {
	errno = 0;
	std::vector<epoll_event> events;
	events.resize(register_event_count_);
	
	int ndfs = epoll_wait(epfd_, events.data(), register_event_count_, timeout);
	
	std::vector<Event> result;
	for (size_t i = 0; i < ndfs; ++i) 
	{
	    auto handle_info = reinterpret_cast<HandleInfo*>(events[i].data.ptr);
	    if (handle_info->handle != nullptr && handle_info->handle != (Handle*)&handle_info->handle) {
	        result.emplace_back(Event {
	            .handle_info = *handle_info
	        });
	    } else {
	        // mark event ready, but has no response callback
	        handle_info->handle = (Handle*)&handle_info->handle;
	    }
	}
	return result;
    }
*/ 


