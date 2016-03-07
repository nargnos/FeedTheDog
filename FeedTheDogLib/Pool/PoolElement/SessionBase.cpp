#include "SessionBase.h"
#include "..\..\Core\Core.h"
namespace FeedTheDog
{
	SessionBase::SessionBase(TCore * corePtr) :
		core(corePtr)
	{
	}
	SessionBase::TBufferType& SessionBase::GetBuffer()
	{
		return buffer;
	}
}  // namespace FeedTheDog


