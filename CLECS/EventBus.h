#pragma once
#include <functional>
#include <unordered_map>
#include <vector>
#include <typeindex>
#include <algorithm>

struct SystemContext;

/* Responsible for managing event subscriptions and notifications in CLECS.
 * Systems may subscribe to events by type, and the EventBus will notify them when events of that type are emitted.
 * Notifications are synchronous and happen immediately.
 * SubscriberId is the address of the system's Update function cast to const void*, providing a stable unique key without requiring a System base class instance.
 */
class EventBus
{
public:
	EventBus() = default;
	~EventBus() = default;
	EventBus(const EventBus&) = delete;
	EventBus& operator=(const EventBus&) = delete;

	template<typename EventType>
	void Subscribe(const void* SubscriberId, std::function<void(const SystemContext&, const EventType&)> Callback)
	{
		const auto TypeId = std::type_index(typeid(EventType));
		const auto Wrapper = [Callback](const SystemContext& Context, const void* EventData)
		{
			Callback(Context, *static_cast<const EventType*>(EventData));
		};
		Subscribers[TypeId].push_back({ SubscriberId, Wrapper });
	}

	template<typename EventType>
	void Unsubscribe(const void* SubscriberId)
	{
		const auto TypeId = std::type_index(typeid(EventType));
		const auto It = Subscribers.find(TypeId);
		if (It == Subscribers.end())
			return;

		auto& Callbacks = It->second;
		Callbacks.erase(
			std::remove_if(Callbacks.begin(), Callbacks.end(),
				[SubscriberId](const SubscriptionEntry& Entry)
				{
					return Entry.SubscriberId == SubscriberId;
				}),
			Callbacks.end()
		);
	}

	template<typename EventType>
	void Notify(const SystemContext& Context, const EventType& Event) const
	{
		const auto TypeId = std::type_index(typeid(EventType));
		const auto It = Subscribers.find(TypeId);
		if (It == Subscribers.end())
			return;

		// Prevent invalidating iterators if someone unsubscribes in response.
		const std::vector<SubscriptionEntry> CallbacksCopy = It->second;
		for (const auto& Entry : CallbacksCopy)
			Entry.Callback(Context, &Event);
	}

private:
	using EventCallback = std::function<void(const SystemContext&, const void*)>;

	struct SubscriptionEntry
	{
		const void* SubscriberId;
		EventCallback Callback;
	};

	std::unordered_map<std::type_index, std::vector<SubscriptionEntry>> Subscribers;
};