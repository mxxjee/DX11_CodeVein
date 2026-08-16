#include "Engine_Define.h"
#include "Serializable.h"

Serializable::~Serializable()
{
}

void Engine::Serializable::Free()
{
	__super::Free();
}
