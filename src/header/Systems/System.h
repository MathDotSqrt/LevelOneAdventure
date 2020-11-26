#pragma once

namespace LOA {
	//Declare engine type to avoid circular includes
	//We include Engine.h in the cpp file
	class Engine;
}

namespace LOA::Systems {
	
	
	class BaseSystem {
	public:
		virtual ~BaseSystem() {}

		//Optional to override
		void init(Engine& engine) {
		
		}

		//Abstract function
		virtual void update(Engine& engine, float delta) = 0;
	};
}