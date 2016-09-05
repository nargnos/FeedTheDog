#pragma once
#include <memory>
#include "ILayer.h"
__interface ILayerBuilder
{
	_STD unique_ptr<ILayer> Build() const;
};