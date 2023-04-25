#pragma once

#include "../src/eventpp/Bus.h"


namespace AppSystem
{
	struct Vacia{};
	
	class MessageBroker
	{
		private:
		eventpp::Bus<Vacia>* _bus;
	public:
        MessageBroker() = default;
        MessageBroker(eventpp::Bus<Vacia> *bus)
        :_bus(bus){

        }
		virtual ~MessageBroker() = default;
		
		template<typename TEvent, typename Fn>
			void Add() {
				_bus->add<TEvent,Fn>();
			}

		template<typename TEvent>
			void Publish(std::string v){
				_bus->publish<TEvent>(v);
			}
		
		template<typename TEvent>
			void Publish(int v){
				_bus->publish<TEvent>(v);
			}
	};

}