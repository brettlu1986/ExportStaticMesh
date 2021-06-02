
#include "FIndexBuffer.h"

FIndexBuffer::FIndexBuffer()
: FRenderResource(E_FRESOURCE_TYPE::F_TYPE_INDEX_BUFFER)
, IndicesCount(0)
{

}

FIndexBuffer::FIndexBuffer(E_FRESOURCE_TYPE Type)
:FRenderResource(Type)
,IndicesCount(0)
{

}

FIndexBuffer::~FIndexBuffer()
{
}

void FIndexBuffer::Destroy()
{

}

void FIndexBuffer::Initialize()
{

}