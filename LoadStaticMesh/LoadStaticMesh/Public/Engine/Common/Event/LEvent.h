#pragma once

#include "stdafx.h"
#include <map>
#include <functional>

enum class E_EVENT_KEY : int
{
	EVENT_INPUT = 0,

};

class IEvent
{
public:
	virtual const E_EVENT_KEY getKey() const = 0;

};


template <typename ..._args>
class LEvent : public IEvent
{
public:
	//Alias for template callback
	using _callback = std::function<void(_args...)>;

	//Ctor & Dtor
	//template< typename T_CB >
	explicit LEvent(const E_EVENT_KEY InKey, const _callback& cb) : _Key(InKey), _cbFunc(cb) {}
	~LEvent() {}

	//Accessors
	virtual const E_EVENT_KEY getKey() const override { return this->_Key; }

	//Methods
	void trigger(_args... a) { this->_cbFunc(a...); }
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
		for (auto el : _eventList)
		{
			for (auto e : el.second)
				delete e;
		}
	}

	void RegisterEvent(IEvent* event)
	{
		if (event)
			_eventList[event->getKey()].push_back(event);
	}

	template <typename ..._args>
	void DispatchEvent(E_EVENT_KEY EventKey, _args...a)
	{
		auto it_eventList = _eventList.find(EventKey);
		if (it_eventList == _eventList.end())
			return;
		for (auto ie : it_eventList->second)
		{
			if (LEvent<_args...>* event = dynamic_cast<LEvent<_args...>*>(ie))
				event->trigger(a...);
		}
	}

private:
	std::map<E_EVENT_KEY, std::vector<IEvent*>> _eventList;
};