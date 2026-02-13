#pragma once
#include <functional>
#include <unordered_map>
#include <vector>
#include <typeindex>
#include <memory>
#include <algorithm>

class System;
struct SystemContext;

/* Responsible for managing event subscriptions and notifications in CLECS.
 * Systems may subscribe to events by type, and the EventBus will notify them when events of that type are emitted.
 * Notifications are synchronous and happen immediately.
 * This means that an event sent in response to another will be processed first.
 */
class EventBus
{
public:
	EventBus() = default;
	~EventBus() = default;
	EventBus(const EventBus&) = delete;
	EventBus& operator=(const EventBus&) = delete;

	template<typename EventType>
	void Subscribe(const System* Subscriber, std::function<void(const SystemContext&, const EventType&)> Callback)
	{
		const auto TypeId = std::type_index(typeid(EventType));
		const auto Wrapper = [Callback](const SystemContext& Context, const void* EventData)
		{
			Callback(Context, *static_cast<const EventType*>(EventData));
		};
		Subscribers[TypeId].push_back({ Subscriber, Wrapper });
	}

	template<typename EventType>
	void Unsubscribe(const System* Subscriber)
	{
		const auto TypeId = std::type_index(typeid(EventType));
		const auto It = Subscribers.find(TypeId);
		if (It == Subscribers.end())
			return;

		auto& Callbacks = It->second;
		Callbacks.erase(
			std::remove_if(Callbacks.begin(), Callbacks.end(),
				[Subscriber](const SubscriptionEntry& Entry)
				{
					return Entry.Subscriber == Subscriber;
				}),
			Callbacks.end()
		);
	}

	template<typename EventType>
	void Notify(const SystemContext& Context, const EventType& Event) const
	{
		const auto TypeId = std::type_index(typeid(EventType));
		auto It = Subscribers.find(TypeId);
		if (It == Subscribers.end())
			return;

		// Prevent invalidating iterators if someone unsubscribes in response.
		std::vector<SubscriptionEntry> CallbacksCopy = It->second;

		for (const auto& Entry : CallbacksCopy)
		{
			Entry.Callback(Context, &Event);
		}
	}

private:
	using EventCallback = std::function<void(const SystemContext&, const void*)>;
	
	struct SubscriptionEntry
	{
		const System* Subscriber;
		EventCallback Callback;
	};

	std::unordered_map<std::type_index, std::vector<SubscriptionEntry>> Subscribers;
};