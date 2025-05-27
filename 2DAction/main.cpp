#include "GameLib/Framework.h"
#include "Sequence/Parent.h"


namespace GameLib 
{
	void Framework::update() 
	{
        Sequence::Parent::instance()->Update();
        Framework::setFrameRate(60);

		if (Framework::isKeyOn('q')) 
        {
            Framework::requestEnd();
		}
		if (Framework::isEndRequested())
		{
			Sequence::Parent::release();
		}
	}
}