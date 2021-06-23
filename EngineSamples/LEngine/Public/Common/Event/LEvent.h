#pragma once

#include "pch.h"


enum class E_EVENT_KEY : int
{
	EVENT_INPUT = 0,
	EVENT_ANIM_MACHINE_STATE,
};

class IEvent
{
public:
	virtual const E_EVENT_KEY GetKey() const = 0;

};


template <typename ..._args>
class LEvent : public IEvent
{
public:
	//Alias for template callback
	using _callback = function<void(_args...)>;

	//Ctor & Dtor
	//template< typename T_CB >
	explicit LEvent(const E_EVENT_KEY InKey, const _callback& cb) : _Key(InKey), _cbFunc(cb) {}
	~LEvent() {}

	//Accessors
	virtual const E_EVENT_KEY GetKey() const override { return this->_Key; }

	//Methods
	void Trigger(_args... a) { this->_cbFunc(a...); }
private:
	//Event identifier
	E_EVENT_KEY _Key;
	//Event callback - can't be changed inside event.
	_callback const _cbFunc;
};

class EventDispatcher
{
public:
	EventDispatcher() {}
	~EventDispatcher()
	{
		for (auto el : EventList)
		{
			for (auto e : el.second)
				delete e;
		}
	}

	void RegisterEvent(IEvent* event)
	{
		if (event)
			EventList[event->GetKey()].push_back(event);
	}

	template <typename ..._args>
	void DispatchEvent(E_EVENT_KEY EventKey, _args...a)
	{
		auto It_eventList = EventList.find(EventKey);
		if (It_eventList == EventList.end())
			return;
		for (auto ie : It_eventList->second)
		{
			if (LEvent<_args...>* event = dynamic_cast<LEvent<_args...>*>(ie))
				event->Trigger(a...);
		}
	}

private:
	map<E_EVENT_KEY, vector<IEvent*>> EventList;
};