#pragma once

#include <string>

namespace CgBot {
	template <class entity_type>
	class State
	{
	public:

		virtual ~State(){}
		//this is the states normal update function
		virtual std::string getName() = 0;
		virtual void Execute(entity_type*) = 0;

	};

#define DECLARE_CLASS(className, entityClass) \
    class  className : public State<entityClass> \
				{ \
	public:  \
		className() {}; \
		className(const className&) = delete; \
		className& operator=(const className&) = delete; \
	public:  \
		virtual void Execute(entityClass*); \
		std::string getName() { return #className; }; \
	}; \
	extern className entityClass##className;

#define DEFINE_OBJECT(className, entityClass) className entityClass##className;
}