#include "stdafx.h"
#include "Mirror.h"

Mirror::Mirror()
{
	
}

Mirror::~Mirror()
{

}

void Mirror::Update()
{
	UpdateRotation();
	UpdateView();
}
