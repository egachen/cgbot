#pragma once

#include <cassert>
#include <string>

#include "log/log.h"
#include "State.h"


namespace  CgBot{

	template <class entity_type>
	class StateMachine
	{
	private:

		//a pointer to the agent that owns this instance
		entity_type*          pOwner_;
		State<entity_type>*   pCurrentState_;
		State<entity_type>*   pPreviousState_;

		std::string           name_;

	public:

		StateMachine(entity_type* owner, std::string name) :pOwner_(owner),
			pCurrentState_(NULL), name_(name)
		{}

		virtual ~StateMachine(){}

		//use these methods to initialize the FSM
	    void SetCurrentState(State<entity_type>* s);

		//call this to update the FSM
		void  Update()const
		{
			//same for the current state
			if (pCurrentState_) pCurrentState_->Execute(pOwner_);
		}

		//change to a new state
		void  ChangeState(State<entity_type>* pNewState)
		{
			assert(pNewState &&
				"<StateMachine::ChangeState>: trying to change to NULL state");
			logger  << name_ << " change State from " << pCurrentState_->getName() << " to " << pNewState->getName() << std::endl;
			pPreviousState_ = pCurrentState_;
			//change state to the new state
			pCurrentState_ = pNewState;
		}

		//returns true if the current state's type is equal to the type of the
		//class passed as a parameter. 
		bool  isInState(const State<entity_type>& st)const
		{
			return typeid(*pCurrentState_) == typeid(st);
		}

		State<entity_type>*  CurrentState()  const{ return pCurrentState_; }
	};

	template <class entity_type> 
	inline
	void StateMachine<entity_type>::SetCurrentState(State<entity_type>* s){
		pCurrentState_ = s;
	};

}

