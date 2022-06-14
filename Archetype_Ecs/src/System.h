#pragma once
#include "EcsUtils.h"

#include <assert.h>
#include <vector>
#include <algorithm>
namespace Ecs
{
	class System
	{
		friend class ECSWorld;
	public:
		System() = default;
		virtual ~System() = default;


	protected:
		void Run(ECSWorld* world) = 0;
	};
}