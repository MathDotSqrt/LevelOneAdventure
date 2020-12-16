#pragma once

namespace LOA {
	//Declare engine type to avoid circular includes
	//We include Engine.h in the cpp file
	class Engine;
}

namespace LOA::Systems {
	
	
	class BaseSystem {
	public:
		BaseSystem(LOA::Engine &engine) : engine(engine){}
		virtual ~BaseSystem() {}

		//Optional to override
		virtual void init() {
		
		}

		//Abstract function
		virtual void update(float delta) = 0;

	protected:
		LOA::Engine& engine;
	};
}